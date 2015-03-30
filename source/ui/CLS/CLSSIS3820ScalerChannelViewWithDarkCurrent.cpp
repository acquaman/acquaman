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


#include "CLSSIS3820ScalerChannelViewWithDarkCurrent.h"

#include <QDebug>

CLSSIS3820ScalerChannelViewWithDarkCurrent::CLSSIS3820ScalerChannelViewWithDarkCurrent(CLSSIS3820ScalerChannel *channel, QWidget *parent) :
	CLSSIS3820ScalerChannelView(channel, parent)
{
	darkCurrentValueLabel_ = new QLabel();
	setDarkCurrentValue(0);

	darkCurrentCorrectedLabel_ = new QLabel();
	setDarkCurrentCorrected(0);

	content_ = new QVBoxLayout();
	content_->addWidget(darkCurrentValueLabel_);
	content_->addWidget(darkCurrentCorrectedLabel_);

	if (channel_ && channel_->isEnabled() && channel_->detector() && channel_->detector()->canDoDarkCurrentCorrection()) {
		setDarkCurrentValue(channel_->detector()->darkCurrentValue());

		connect( channel_->detector(), SIGNAL(darkCurrentValueChanged(double)), this, SLOT(setDarkCurrentValue(double)) );
		connect( channel_->detector(), SIGNAL(darkCurrentValidStateChanged(bool)), this, SLOT(setDarkCurrentState(bool)) );
		connect( channel_->detector(), SIGNAL(acquisitionStateChanged(AMDetector::AcqusitionState)), this, SLOT(onDetectorAcquisitionStateChanged(AMDetector::AcqusitionState)) );

		setDarkCurrentViewMode(Show);

	} else {
		setDarkCurrentViewMode(Hide);
	}
}

CLSSIS3820ScalerChannelViewWithDarkCurrent::~CLSSIS3820ScalerChannelViewWithDarkCurrent()
{

}

void CLSSIS3820ScalerChannelViewWithDarkCurrent::setDarkCurrentViewMode(DarkCurrentViewMode newMode)
{
	if (channel_ && channel_->detector() && channel_->detector()->canDoDarkCurrentCorrection()) {

		if (darkCurrentViewMode_ != newMode) {
			darkCurrentViewMode_ = newMode;

			if (darkCurrentViewMode_ == Hide) {
				channelLayout_->removeItem(content_);

			} else if (darkCurrentViewMode_ == Show) {
				channelLayout_->addLayout(content_);

			} else {
				darkCurrentViewMode_ = Hide;
			}

			emit darkCurrentViewModeChanged(darkCurrentViewMode_);
		}
	}
}

void CLSSIS3820ScalerChannelViewWithDarkCurrent::setDarkCurrentValue(double newValue)
{
	darkCurrentValueLabel_->setText(QString("Dark current value : %1").arg(newValue, 0, 'f', 2));
}

void CLSSIS3820ScalerChannelViewWithDarkCurrent::setDarkCurrentCorrected(double newValue)
{
	darkCurrentCorrectedLabel_->setText(QString("Corrected measurement : %1").arg(newValue, 0, 'f', 2));
}

void CLSSIS3820ScalerChannelViewWithDarkCurrent::setDarkCurrentState(bool valid)
{
	if (valid)
		darkCurrentValueLabel_->setStyleSheet("color: blue;");

	else
		darkCurrentValueLabel_->setStyleSheet("color: red;");
}

void CLSSIS3820ScalerChannelViewWithDarkCurrent::onDetectorAcquisitionStateChanged(AMDetector::AcqusitionState newState)
{
	switch (newState) {
	case AMDetector::Succeeded:
		onDetectorAcquisitionSucceeded();
		break;
	default:
		break;
	}
}

void CLSSIS3820ScalerChannelViewWithDarkCurrent::onDetectorAcquisitionSucceeded()
{
//	if (channel_ && channel_->isEnabled() && channel_->detector() && channel_->detector()->canDoDarkCurrentCorrection()) {

//		// we arrive at a corrected value by dividing the number of counts acquired by the measurement dwell time, and then subtracting the dark current.

//		double *newValue = 0;
//		channel_->detector()->data(newValue);

//		double measurement = *newValue / channel_->detector()->acquisitionTime();

//		double correctedMeasurement = measurement - channel_->detector()->darkCurrentValue();

//		qDebug() << "CLSSIS3820ScalerChannelView has new DC corrected value : " << correctedMeasurement;

//		setDarkCurrentCorrected(correctedMeasurement);
//	}
}
