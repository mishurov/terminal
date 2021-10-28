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


#ifndef MARKETCONTROLLER_H
#define MARKETCONTROLLER_H

#include <QtCore/QObject>
#include <QtCore/QDateTime>

class Facade;
class AbstractBroker;

class Controller : public QObject
{
	Q_OBJECT

public:
	Controller(QObject *parent = nullptr);
	Facade* facade();

	void brokerDataRequested();
	void candlesRequested(QString figi, QDateTime to, int interval, int count);

	void connectBroker();
	void brokerChanged(int broker);
private:
	QList<AbstractBroker*> brokers;
	AbstractBroker* m_broker;

	Facade* m_facade;
};


#endif //MARKETCONTROLLER
