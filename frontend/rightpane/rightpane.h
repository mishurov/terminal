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


#ifndef RIGHTPANE_H
#define RIGHTPANE_H

#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class RightPane; }
class QAbstractButton;
QT_END_NAMESPACE

class CrossHair;


class RightPane : public QWidget
{
	Q_OBJECT

public:
	RightPane(QWidget *parent=0);
	~RightPane();
	Ui::RightPane *ui;

	void setCurrent(double price);
	void setPrevClose(double price);

private:
	void stopValueChanged();
	void stopPercentChanged();
	void targetValueChanged();
	void targetPercentChanged();
	void longShortToggled(QAbstractButton *button, bool checked);
	void positionChanged();

	void dragStopChanged(QPointF value);
	void dragTargetChanged(QPointF value);

	void loadStyleSheet();
	void resetControls();
	void updateControls();
	void updatePriceChange();

	void updateRiskReturn();

	CrossHair* m_stopLoss;
	CrossHair* m_takeProfit;
	double m_price = 0;
	double m_prevClose = 0;

	bool m_reset = true;
};
#endif // RIGHTPANE_H
