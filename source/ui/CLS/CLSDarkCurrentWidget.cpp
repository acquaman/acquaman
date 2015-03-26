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


#include "CLSDarkCurrentWidget.h"

CLSDarkCurrentWidget::CLSDarkCurrentWidget(CLSSIS3820Scaler *scaler, QWidget *parent) :
    QGroupBox("Dark current correction", parent)
{
	// Initialize member variables.

	scaler_ = 0;

	// Create UI elements.

	QLabel *timePrompt = new QLabel("Dwell time: ");

	timeEntry_ = new QSpinBox();
	timeEntry_->setRange(DARK_CURRENT_DWELL_TIME_MIN, DARK_CURRENT_DWELL_TIME_MAX);
	timeEntry_->setValue(0);
	timeEntry_->setSuffix(" ms");

	collectButton_ = new QPushButton("Collect");

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addWidget(timePrompt);
	layout->addWidget(timeEntry_);
	layout->addWidget(collectButton_);

	setLayout(layout);

	// Make connections.

	connect( collectButton_, SIGNAL(clicked()), this, SLOT(onCollectButtonClicked()) );

	// Current settings.

	setScaler(scaler);
}

CLSDarkCurrentWidget::~CLSDarkCurrentWidget()
{

}

void CLSDarkCurrentWidget::setScaler(CLSSIS3820Scaler *newScaler)
{
	if (scaler_ != newScaler) {

		if (scaler_) {
			disconnect( scaler_, 0, this, 0 );
		}

		scaler_ = newScaler;

		if (scaler_) {
			connect( scaler_, SIGNAL(scanningChanged(bool)), this, SLOT(onScalerScanningChanged()) );

			timeEntry_->setValue(scaler_->dwellTime() * 1000);
		}

		emit scalerChanged(scaler_);
	}
}

void CLSDarkCurrentWidget::onScalerScanningChanged()
{
	if (scaler_ && !scaler_->isScanning())
		collectButton_->setEnabled(true);
	else
		collectButton_->setEnabled(false);
}

void CLSDarkCurrentWidget::onCollectButtonClicked()
{
	double secondsEntered = timeEntry_->value() / 1000.0;

	if (scaler_ && secondsEntered > 0) {
		scaler_->doDarkCurrentCorrection(secondsEntered);
	}
}
