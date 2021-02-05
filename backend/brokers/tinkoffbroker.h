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

#include "abstractbroker.h"
#include <QtCore/QUrl>
#include <QtWebSockets/QWebSocket>

QT_BEGIN_NAMESPACE
class QNetworkAccessManager;
class QNetworkReply;
QT_END_NAMESPACE


class TinkoffBroker : public AbstractBroker
{
	Q_OBJECT

public:
	TinkoffBroker(QObject *parent = nullptr);
	~TinkoffBroker();

	void requestStocks() override;
	void requestCandles(QString figi, int interval) override;

	void brokerChanged() override;
private:
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


	QNetworkAccessManager *m_manager;
	QString m_token;

	QWebSocket m_webSocket;
	QUrl m_stocksUrl;
	QUrl m_candlesUrl;
	QUrl m_websocketUrl;

	int m_intervalReq;
	QString m_figiReq;
};


#endif //TINKOFFBROKER_H
