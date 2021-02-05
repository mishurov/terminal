/****************************************************************************
**
** This file is part of the Blackfriars Trading Terminal
**
** Copyright (C) 2021 Alexander Mishurov
**
** GNU General Public License Usage
** This file may be used under the terms of the GNU
** General Public License version 3. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/


#include "tickerinput.h"
#include "backend/brokers/abstractbroker.h"

#include <QtCore/QTextStream>
#include <QtWidgets/QAction>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QCompleter>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtGui/QMouseEvent>
#include <QtGui/QStandardItemModel>

#include <QtCore/QDebug>


#define MAX_COMPLETER_ITEMS 15


void StyledItemDelegate::paint(QPainter *painter,
		const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItem opt(option);

	TableView *view = qobject_cast<TableView *>(opt.styleObject);

	// ugly hack to use qss for styling
	if (index.column() == 0)
		view->setProperty("column", "ticker");

	if (index.column() == 1)
		view->setProperty("column", "listing");

	view->style()->unpolish(view);
	view->style()->polish(view);

	opt.widget = view;
	opt.font = view->font();

	if (view->hoveredRow() == index.row() && !(opt.state & QStyle::State_Selected))
		opt.state |= QStyle::State_MouseOver;
	else
		opt.state &= ~(QStyle::State_MouseOver);

	QStyledItemDelegate::paint(painter, opt, index);
}


TableView::TableView(QWidget *parent)
    : QTableView (parent)
{
	setMouseTracking(true);
	setSelectionBehavior(SelectRows);
	setSelectionMode(SingleSelection);

	auto delegate = new StyledItemDelegate;
	setItemDelegateForColumn(0, delegate);
	setItemDelegateForColumn(1, delegate);

	setShowGrid(false);
	setWordWrap(false);

	verticalHeader()->hide();
	horizontalHeader()->hide();
	verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void TableView::updateRow(int row)
{
	for (int i = 0; i < model()->columnCount(); ++i)
		update(model()->index(row, i));
}

void TableView::mouseMoveEvent(QMouseEvent *event)
{
	QTableView::mouseMoveEvent(event);
	QModelIndex index = indexAt(event->pos());
	updateRow(m_hoveredRow);
	m_hoveredRow = selectionModel()->isRowSelected(index.row()) ? -1 : index.row();
}

void TableView::wheelEvent(QWheelEvent *event)
{
	QTableView::wheelEvent(event);
	QModelIndex index = indexAt(event->position().toPoint());
	updateRow(m_hoveredRow);
	m_hoveredRow = selectionModel()->isRowSelected(index.row()) ? -1 : index.row();
}

void TableView::leaveEvent(QEvent *event)
{
	QTableView::leaveEvent(event);
	updateRow(m_hoveredRow);
	m_hoveredRow = -1;
}

int TableView::firstColumnWidth()
{
	return columnWidth(0);
}

void TableView::setfirstColumnWidth(int w)
{
	horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
	setColumnWidth(0, w);
}


TickerInput::TickerInput(QWidget *parent)
{
	layout = new QHBoxLayout(this);
	layout->setSizeConstraint(QLayout::SetFixedSize);
	layout->setMargin(0);
	setLayout(layout);

	setupTickerLabel();
	setupInput();
	setupListingLabel();

	QFile file(":/tickerinput.qss");
	file.open(QFile::ReadOnly|QFile::Text);
	QTextStream stream(&file);
	setStyleSheet(stream.readAll());
}

void TickerInput::setupTickerLabel()
{
	auto labelTicker = new QLabel("Ticker");
	labelTicker->setObjectName("labelTicker");
	layout->addWidget(labelTicker);
}

void TickerInput::setupListingLabel()
{
	labelListing = new QLabel();
	labelListing->setObjectName("labelListing");
	layout->addWidget(labelListing);
}

void TickerInput::setupCompleter()
{
	completer = new QCompleter(this);
	completer->setCompletionColumn(0);

	auto *tableView = new TableView(input);
	completer->setPopup(tableView);

	completer->setMaxVisibleItems(MAX_COMPLETER_ITEMS);
	completer->setCompletionMode(QCompleter::PopupCompletion);
	completer->setCaseSensitivity(Qt::CaseInsensitive);

	connect(completer, QOverload<const QModelIndex &>::of(&QCompleter::activated),
		this, &TickerInput::confirm);
}

void TickerInput::setupInput()
{
	input = new QLineEdit;
	input->setObjectName("input");
	auto validator = new UpperCaseValidator(input);
	input->setValidator(validator);

	connect(input, &QLineEdit::returnPressed, this, &TickerInput::returnPressed);
	QAction *actionInput = input->addAction(QIcon(":/magnifier.svg"),
		QLineEdit::TrailingPosition);
	connect(actionInput, &QAction::triggered, this, &TickerInput::confirm);

	setupCompleter();
	input->setCompleter(completer);

	layout->addWidget(input);
}

void TickerInput::returnPressed()
{
	auto p = completer->popup();

	// prevent calling confirm() twice from input and completer
	if (!p->isVisible() || p->currentIndex().row() < 0)
		confirm();
}

void TickerInput::confirm()
{
	// TODO handle no matching entries, when something already selected and so on
	QString text = input->text();
	if (text.isEmpty()) {
		m_figi = "";
		return;
	}

	auto model = (QStandardItemModel *)completer->model();
	auto matches = model->findItems(text);
	if (matches.empty()) {
		labelListing->setText("");
		return;
	}

	auto match = matches.first();
	auto name = model->item(match->row(), 1)->text();
	m_figi =  model->item(match->row(), 2)->text();

	emit confirmed();

	// TODO: set label after candles recieved?
	input->clearFocus();
	labelListing->setText(name);
}


void TickerInput::setModel(const QList<Stock *> &stocks)
{
	auto *m = new QStandardItemModel(stocks.count(), 3, (QObject *)completer);
	for (int i = 0; i < stocks.count(); ++i) {
		m->setData(m->index(i, 0), stocks.at(i)->ticker);
		m->setData(m->index(i, 1), stocks.at(i)->name);
		m->setData(m->index(i, 2), stocks.at(i)->figi);
	}
	completer->setModel(m);
	auto tv = (TableView *)completer->popup();
	tv->setStyleSheet(styleSheet());
	tv->setColumnHidden(2, true);
}

QString TickerInput::figi() { return m_figi; }
