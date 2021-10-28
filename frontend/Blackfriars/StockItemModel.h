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


#ifndef TICKERMODEL_H
#define TICKERMODEL_H

#include "backend/Brokers/AbstractBroker.h"
#include <QtCore/QSortFilterProxyModel>
#include <QtQml/qqml.h>


enum StockItemRole {
	ticker = Qt::UserRole,
	name,
	figi
};


class StockItemModel : public QSortFilterProxyModel {
	Q_OBJECT
	QML_ELEMENT
public:
	StockItemModel(QObject* parent = nullptr);
	Q_INVOKABLE void sortResults();
	void setStocks(const QList<Stock *> &stocks);
	void clear();
private:
	bool filterAcceptsRow(int sourceRow,
		const QModelIndex &sourceParent) const override;
};


#endif // TICKERMODEL_H
