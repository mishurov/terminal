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


#include "rightpane.h"
#include "ui_rightpane.h"

#include "frontend/chartpane/finchartview.h"
#include "frontend/chartpane/crosshair.h"
#include "backend/controller/preferences.h"

#include <QtWidgets/QStyle>
#include <QtCore/QFile>


RightPane::RightPane(QWidget *parent)
	:QWidget(parent)
	, ui(new Ui::RightPane)
{
	ui->setupUi(this);
	loadStyleSheet();

	connect(ui->inputPosition, QOverload<int>::of(&QSpinBox::valueChanged),
		this, &RightPane::positionChanged);

	connect(ui->inputStopPercents, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
		this, &RightPane::stopPercentChanged);
	connect(ui->inputTargetPercents, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
		this, &RightPane::targetPercentChanged);

	connect(ui->inputStopDollars, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
		this, &RightPane::stopValueChanged);
	connect(ui->inputTargetDollars, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
		this, &RightPane::targetValueChanged);
	
	connect(ui->groupLongShort, QOverload<QAbstractButton *, bool>::of(&QButtonGroup::buttonToggled),
		this, &RightPane::longShortToggled);

	auto finChartView = window()->findChild<FinChartView *>("finChartView");

	m_stopLoss = finChartView->stopLoss();
	connect(m_stopLoss, &CrossHair::dragValueChanged, this, &RightPane::dragStopChanged);

	m_takeProfit = finChartView->takeProfit();
	connect(m_takeProfit, &CrossHair::dragValueChanged, this, &RightPane::dragTargetChanged);
}

RightPane::~RightPane()
{
	delete ui;
}

void RightPane::dragStopChanged(QPointF value)
{
	ui->inputStopDollars->setValue(value.y());
}

void RightPane::dragTargetChanged(QPointF value)
{
	ui->inputTargetDollars->setValue(value.y());
}

void RightPane::longShortToggled(QAbstractButton *button, bool checked) {
	if (!checked)
		return;
	stopPercentChanged();
	targetPercentChanged();
}

void RightPane::stopPercentChanged()
{
	auto ratio = ui->inputStopPercents->value() / 100;
	auto stopRatio = ui->buttonShort->isChecked() ? 1 + ratio : 1 - ratio;
	auto value = m_price * stopRatio;
	ui->inputStopDollars->setValue(value);

	updateRiskReturn();

	if (!m_stopLoss->isDragged())
		m_stopLoss->setValue(QPointF(0, value));
}

void RightPane::stopValueChanged()
{
	auto ratio = ui->inputStopDollars->value() / m_price;
	auto stopRatio = ui->buttonShort->isChecked() ? ratio - 1 : 1 - ratio;
	ui->inputStopPercents->setValue(stopRatio * 100);
}

void RightPane::targetPercentChanged()
{
	auto ratio = ui->inputTargetPercents->value() / 100;
	auto targetRatio = ui->buttonShort->isChecked() ? 1 - ratio : 1 + ratio;
	auto value = m_price * targetRatio;
	ui->inputTargetDollars->setValue(value);

	updateRiskReturn();
	
	if (!m_takeProfit->isDragged())
		m_takeProfit->setValue(QPointF(0, value));
}

void RightPane::targetValueChanged()
{
	auto ratio = ui->inputTargetDollars->value() / m_price;
	auto targetRatio = ui->buttonShort->isChecked() ? 1 - ratio: ratio - 1;
	ui->inputTargetPercents->setValue(targetRatio * 100);
}


void RightPane::updateRiskReturn()
{
	auto stop = ui->inputStopPercents->value();
	auto target = ui->inputTargetPercents->value();
	auto ratio = target / stop;

	QString text = "%1 / 1";
	text = text.arg(ratio, 0, 'f', 1);
	ui->labelRiskReturnValue->setText(text);
}

void RightPane::positionChanged()
{
	auto value = ui->inputPosition->value() * m_price;
	ui->labelPositionValue->setText(QString::number(value, 'f', 2));
}

void RightPane::updatePriceChange()
{
	if (!m_prevClose) {
		ui->labelMarketPriceChange->setText("");
		return;
	}

	float diff = m_price - m_prevClose;
	float percent = diff / m_price * 100;
	ui->labelMarketPriceChange->setProperty("dir", diff >= 0 ? "up" : "down");

	QString text;
	text = text.asprintf("%+.2f (%+.2f%%)", diff, percent);

	ui->labelMarketPriceChange->setText(text);
	ui->labelMarketPriceChange->style()->unpolish(ui->labelMarketPriceChange);
	ui->labelMarketPriceChange->style()->polish(ui->labelMarketPriceChange);
}

void RightPane::updateControls()
{
	ui->labelMarketPriceValue->setText(QString::number(m_price, 'f', 2));
	updatePriceChange();
	positionChanged();

	stopPercentChanged();
	targetPercentChanged();

	bool shrt = ui->buttonShort->isChecked();
	m_takeProfit->setDragYLimit(shrt ? +m_price : -m_price);
	m_stopLoss->setDragYLimit(shrt ? -m_price : +m_price);
}

void RightPane::resetControls()
{
	auto data = preferences.read();
	ui->inputPosition->setValue(0);
	// TODO: should take actual portfolio position from API
	ui->inputStopPercents->setValue(data->risk);
	ui->inputTargetPercents->setValue(data->target);
	ui->buttonLong->setChecked(true);

	m_reset = true;
}

void RightPane::setCurrent(double price)
{
	m_price = price;
	updateControls();
}

void RightPane::setPrevClose(double price)
{
	m_prevClose = price;
	resetControls();
}

void RightPane::loadStyleSheet()
{
	QFile file(":/rightpane.qss");
	file.open(QFile::ReadOnly|QFile::Text);
	QTextStream stream(&file);
	setStyleSheet(stream.readAll());
}
