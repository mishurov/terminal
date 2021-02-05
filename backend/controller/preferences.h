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


#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QtCore/QObject>
#include <QtCore/QStringList>


extern QStringList brokers;

struct PrefData {
	int broker;
	QString credentials;
	double risk;
	double target;
};

extern PrefData defaults;

class Preferences : public QObject
{
	Q_OBJECT

public:
	Preferences(QObject *parent = nullptr);
	~Preferences();

	const PrefData* read();
	void save(const PrefData *prefs);
	
	// TODO signals don't work;
	//void setController(QObject* controller);

signals:
	void brokerChanged(int broker);

private:
	PrefData* m_data;
	QObject* m_controller;
};


extern Preferences preferences;

#endif
