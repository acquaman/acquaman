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
#include "util/AMErrorMonitor.h"

#include <QDebug>

CLSSIS3820ScalerChannelViewWithDarkCurrent::CLSSIS3820ScalerChannelViewWithDarkCurrent(CLSSIS3820ScalerChannel *channel, QWidget *parent) :
	CLSSIS3820ScalerChannelView(channel, parent)
{
	// Initialize member variables.

	darkCurrentViewMode_ = Hide;

	// Create UI elements.

	darkCurrentValueLabel_ = new QLabel();
	darkCurrentCorrectedLabel_ = new QLabel();

	// Create and set layout.

	QVBoxLayout *darkCurrentLayout = new QVBoxLayout();
	darkCurrentLayout->addWidget(darkCurrentValueLabel_);
	darkCurrentLayout->addWidget(darkCurrentCorrectedLabel_);

	channelLayout_->addLayout(darkCurrentLayout);

	// Initial settings.

	hide();

	// Current settings.

	setOutputViewMode(viewMode_);
}

CLSSIS3820ScalerChannelViewWithDarkCurrent::~CLSSIS3820ScalerChannelViewWithDarkCurrent()
{

}

void CLSSIS3820ScalerChannelViewWithDarkCurrent::show()
{
	// we can only show the dark current view if the channel detector is capable of dark current corrections
	// and the view mode is in counts.

	if (viewMode_ == CLSSIS3820ScalerChannelView::Counts && channel_ && channel_->isEnabled() && channel_->detector() && channel_->detector()->canDoDarkCurrentCorrection()) {

		setDarkCurrentValue(channel_->detector()->darkCurrentValue());
		setDarkCurrentCorrected(lastMeasurement() - channel_->detector()->darkCurrentValue());
		setDarkCurrentState(channel_->detector()->darkCurrentValidState());

		connect( channel_->detector(), SIGNAL(darkCurrentValueChanged(double)), this, SLOT(onDetectorDarkCurrentValueChanged(double)) );
		connect( channel_->detector(), SIGNAL(darkCurrentValidStateChanged(bool)), this, SLOT(setDarkCurrentState(bool)) );

		darkCurrentValueLabel_->show();
		darkCurrentCorrectedLabel_->show();

		setDarkCurrentViewMode(Show);
	}
}

void CLSSIS3820ScalerChannelViewWithDarkCurrent::hide()
{
	// we can always hide the display.

	if (channel_ && channel_->detector())
		disconnect( channel_->detector(), 0, this, 0 );

	darkCurrentValueLabel_->hide();
	setDarkCurrentValue(0);

	darkCurrentCorrectedLabel_->hide();
	setDarkCurrentCorrected(0);

	setDarkCurrentViewMode(Hide);
}

void CLSSIS3820ScalerChannelViewWithDarkCurrent::setOutputViewMode(CLSSIS3820ScalerChannelView::OutputViewMode mode)
{
	CLSSIS3820ScalerChannelView::setOutputViewMode(mode);

	if (mode == CLSSIS3820ScalerChannelView::Counts)
		show();
	else
		hide();
}

void CLSSIS3820ScalerChannelViewWithDarkCurrent::setDarkCurrentValue(double newValue)
{
	darkCurrentValueLabel_->setText(QString("Dark current value : %1").arg((int)newValue));
}

void CLSSIS3820ScalerChannelViewWithDarkCurrent::setDarkCurrentCorrected(double newValue)
{
	darkCurrentCorrectedLabel_->setText(QString("Corrected measurement : %1").arg((int)newValue));
}

void CLSSIS3820ScalerChannelViewWithDarkCurrent::setDarkCurrentState(bool valid)
{
	if (valid)
		darkCurrentValueLabel_->setStyleSheet("color: blue;");

	else
		darkCurrentValueLabel_->setStyleSheet("color: red;");
}

void CLSSIS3820ScalerChannelViewWithDarkCurrent::onDetectorDarkCurrentValueChanged(double newValue)
{
	// only need to update this view if it is shown.
	// this condition should only be Show if the channel is enabled and valid, and the channel detector is valid and can do dark current correction.

	if (darkCurrentViewMode_ == Show) {
		// set the displayed dark current value.
		setDarkCurrentValue(newValue);

		// update the displayed corrected value to incorporate the new dark current.
		setDarkCurrentCorrected(lastMeasurement() - newValue);
	}
}

void CLSSIS3820ScalerChannelViewWithDarkCurrent::onReadingChanged()
{
	CLSSIS3820ScalerChannelView::onReadingChanged();

	// only need to update this view if it is shown.
	// this condition should only be Show if the channel is enabled and valid, and the channel detector is valid and can do dark current correction.

	if (darkCurrentViewMode_ == Show) {
		// update the displayed corrected measurement, taking into account the detector's dark current value.
		setDarkCurrentCorrected(lastMeasurement() - channel_->detector()->darkCurrentValue());
	}
}

double CLSSIS3820ScalerChannelViewWithDarkCurrent::lastMeasurement()
{
	double result = 0;

	if (channel_ && channel_->detector()) {
		double reading = channel_->reading();
		double dwellTime = channel_->detector()->acquisitionTime();

		if (dwellTime > 0) {
			result = reading / dwellTime;
		}
	}

	return result;
}

void CLSSIS3820ScalerChannelViewWithDarkCurrent::setDarkCurrentViewMode(DarkCurrentViewMode newMode)
{
	if (darkCurrentViewMode_ != newMode) {
		darkCurrentViewMode_ = newMode;
		emit darkCurrentViewModeChanged(darkCurrentViewMode_);
	}
}
