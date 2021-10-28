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


#ifndef TINKOFFBROKER_H
#define TINKOFFBROKER_H

#include "AbstractBroker.h"
#include <QtCore/QUrl>
#include <QtWebSockets/QWebSocket>

class QNetworkAccessManager;
class QNetworkReply;
class Controller;


class TinkoffBroker : public AbstractBroker
{
	Q_OBJECT

public:
	TinkoffBroker(Controller *parent = nullptr);
	~TinkoffBroker();

	void requestBrokerData() override;
	void requestCandles(QString figi, QDateTime to, int interval, int count) override;

	void brokerChanged() override;

private:
	QString intervalToString(int interval);
	int intervalFromString(QString intervalName);
	QDateTime intervalToStartDate(QDateTime to, int interval);
	void makeCandlesRequest(QString figi, QDateTime to, int interval);

	QNetworkRequest authRequest(QUrl url);

	void responseReceived(QNetworkReply *reply);
	void parseStocks(QJsonDocument json);
	void parseCandles(QJsonDocument json);

	void subscribe();

	void parseOrderBook(QJsonObject json);
	void parseCandle(QJsonObject json);

	void onSslErrors(const QList<QSslError> &errors);
	void onTextMessageReceived(const QString &message);
	void onConnected();

private:
	QNetworkAccessManager *m_manager;
	QString m_token;

	QWebSocket m_webSocket;
	QUrl m_stocksUrl;
	QUrl m_candlesUrl;
	QUrl m_websocketUrl;

	int m_intervalReq;
	int m_countReq;
	QList<QFCandle *> m_recievedCandles;
	QString m_figiReq;
	bool isSubsequentRequest = false;
	bool isPrevDayRequest = false;
	bool hasNoCandles = false;
};


#endif //TINKOFFBROKER_H
