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


#ifndef TICKERINPUT_H
#define TICKERINPUT_H

#include <QtWidgets/QWidget>
#include <QtGui/QValidator>
#include <QtWidgets/QTableView>
#include <QtWidgets/QStyledItemDelegate>

QT_BEGIN_NAMESPACE
// move to backend ?
class QAbstractItemModel;
class QPushButton;
class QCompleter;
class QLineEdit;
class QLabel;
class QHBoxLayout;
QT_END_NAMESPACE


class StyledItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	StyledItemDelegate() : QStyledItemDelegate () {}

public:
	using QStyledItemDelegate::QStyledItemDelegate;

	void paint(QPainter *painter, const QStyleOptionViewItem &option,
			const QModelIndex &index) const override;
};


class TableView : public QTableView
{
	Q_OBJECT
	Q_PROPERTY(int fixedWidth READ width WRITE setFixedWidth)
	Q_PROPERTY(int firstColumnWidth READ firstColumnWidth WRITE setfirstColumnWidth)

public:
	TableView(QWidget *parent = nullptr);

	int hoveredRow() const {
		return m_hoveredRow;
	}

protected:
	void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
	void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;

private:
	int m_hoveredRow = -1;

private:
	void updateRow(int row);
	int firstColumnWidth();
	void setfirstColumnWidth(int w);
};


class UpperCaseValidator : public QValidator
{
public:
	UpperCaseValidator(QObject* parent=nullptr): QValidator(parent) {}

	State validate(QString& input, int&) const override {
		input = input.toUpper();
		return QValidator::Acceptable;
	}
};

struct Stock;

class TickerInput : public QWidget
{
	Q_OBJECT

public:
	TickerInput(QWidget *parent = nullptr);
	
	void setModel(const QList<Stock *> &stocks);
	
	QString figi();
	// maybe should be private
	QHBoxLayout *layout = nullptr;
	QLineEdit *input = nullptr;
	QCompleter *completer = nullptr;
	QLabel *labelListing = nullptr;

signals:
	void confirmed();

private:
	void returnPressed();
	void confirm();

	void setupTickerLabel();
	void setupListingLabel();
	void setupCompleter();
	void setupInput();

	QString m_figi;
};


#endif // TICKERINPUT_H
