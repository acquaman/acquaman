/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "CLSSIS3820ScalerDarkCurrentWidget.h"

CLSSIS3820ScalerDarkCurrentWidget::CLSSIS3820ScalerDarkCurrentWidget(CLSSIS3820Scaler *scaler, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	scaler_ = 0;

	// Create UI elements.

	QLabel *timePrompt = new QLabel("Dwell time: ");

	timeBox_ = new QDoubleSpinBox();

	collectButton_ = new QPushButton("Collect");

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->addWidget(timePrompt);
	layout->addWidget(timeBox_);
	layout->addWidget(collectButton_);

	setLayout(layout);

	// Make connections.

	connect( collectButton_, SIGNAL(clicked()), this, SLOT(onCollectButtonClicked()) );

	// Current settings.

	setScaler(scaler);
}

CLSSIS3820ScalerDarkCurrentWidget::~CLSSIS3820ScalerDarkCurrentWidget()
{

}

void CLSSIS3820ScalerDarkCurrentWidget::refresh()
{
	updateTimeBox();
	updateCollectButton();
}

void CLSSIS3820ScalerDarkCurrentWidget::setScaler(CLSSIS3820Scaler *newScaler)
{
	if (scaler_ != newScaler) {

		if (scaler_) {
			disconnect( scaler_, 0, this, 0 );
		}

		scaler_ = newScaler;

		if (scaler_) {
			connect( scaler_, SIGNAL(connectedChanged(bool)), this, SLOT(refresh()) );
			connect( scaler_, SIGNAL(dwellTimeChanged(double)), this, SLOT(updateTimeBox()) );
			connect( scaler_, SIGNAL(scanningChanged(bool)), this, SLOT(updateCollectButton()) );
		}

		refresh();

		emit scalerChanged(scaler_);
	}
}

void CLSSIS3820ScalerDarkCurrentWidget::updateTimeBox()
{
	if (scaler_ && scaler_->isConnected()) {

		timeBox_->blockSignals(true);
		timeBox_->setEnabled(true);
		timeBox_->setRange(0, INT_MAX);
		timeBox_->setSuffix(" s");
		timeBox_->setValue(scaler_->dwellTime());
		timeBox_->blockSignals(false);

	} else {

		timeBox_->clear();
		timeBox_->setEnabled(false);
	}
}

void CLSSIS3820ScalerDarkCurrentWidget::updateCollectButton()
{
	if (scaler_ && scaler_->isConnected() && !scaler_->isScanning()) {
		collectButton_->setEnabled(true);
		collectButton_->setToolTip("");

	} else {

		collectButton_->setEnabled(false);
		collectButton_->setToolTip("Cannot take dark current measurement while scaler is scanning.");
	}
}

void CLSSIS3820ScalerDarkCurrentWidget::onCollectButtonClicked()
{
	if (scaler_)
		scaler_->measureDarkCurrent(timeBox_->value());
}
