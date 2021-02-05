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


#include "preferences.h"
#include "backend/controller/market.h"
#include <QtCore/QSettings>

#include <QtCore/QDebug>


QStringList brokers = {
	"Monte Carlo",
	"Tinkoff Investments"
};


PrefData defaults {
	.broker = 0,
	.credentials = "",
	.risk = 2,
	.target = 5
};


Preferences preferences;


Preferences::Preferences(QObject *parent)
	: QObject(parent)
{
	m_data = new PrefData;
}

Preferences::~Preferences()
{
	delete m_data;
}

const PrefData* Preferences::read()
{
	QSettings s("Mishurov", "Terminal");
	
	m_data->broker = s.value("Preferences/broker", defaults.broker).toInt();
	m_data->credentials = s.value("Preferences/credentials", defaults.credentials).toString();
	m_data->risk = s.value("Preferences/risk", defaults.risk).toDouble();
	m_data->target = s.value("Preferences/target", defaults.target).toDouble();

	return m_data;
}

void Preferences::save(const PrefData *data)
{
	QSettings s("Mishurov", "Terminal");
	
	auto broker = s.value("Preferences/broker", defaults.broker).toInt();

	if (broker != data->broker)
		emit brokerChanged(data->broker);

	s.setValue("Preferences/broker", data->broker);
	s.setValue("Preferences/credentials", data->credentials);
	s.setValue("Preferences/risk", data->risk);
	s.setValue("Preferences/target", data->target);
}
