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


#include "StockItemModel.h"

#include <QtGui/QStandardItemModel>


StockItemModel::StockItemModel(QObject* parent)
	:QSortFilterProxyModel(parent)
{
	auto *m = new QStandardItemModel();
	QHash<int, QByteArray> names;
	names[StockItemRole::ticker] = "ticker";
	names[StockItemRole::name] = "name";
	names[StockItemRole::figi] = "figi";
	m->setItemRoleNames(names);
	setSourceModel(m);
	setSortRole(StockItemRole::ticker);
	setFilterCaseSensitivity(Qt::CaseInsensitive);
	setSortCaseSensitivity(Qt::CaseInsensitive);
}


void StockItemModel::clear()
{
	QStandardItemModel *m = qobject_cast<QStandardItemModel *>(sourceModel());
	m->clear();
	m->setColumnCount(1);
}


void StockItemModel::setStocks(const QList<Stock *> &stocks)
{
	clear();
	QStandardItemModel *m = qobject_cast<QStandardItemModel *>(sourceModel());

	for (int i = 0; i < stocks.count(); i++) {
		QStandardItem* item = new QStandardItem();

		item->setData(stocks.at(i)->ticker, StockItemRole::ticker);
		item->setData(stocks.at(i)->name, StockItemRole::name);
		item->setData(stocks.at(i)->figi, StockItemRole::figi);

		m->appendRow(item);
	}
}

void StockItemModel::sortResults()
{
	sort(0);
}

bool StockItemModel::filterAcceptsRow(
	int sourceRow, const QModelIndex &sourceParent) const
{
	QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);

	QString ticker = sourceModel()->data(index0, StockItemRole::ticker).toString();
	QString name = sourceModel()->data(index0, StockItemRole::name).toString();
	
	return (ticker.startsWith(filterRegularExpression().pattern(), Qt::CaseInsensitive) ||
		name.contains(filterRegularExpression()));
}
