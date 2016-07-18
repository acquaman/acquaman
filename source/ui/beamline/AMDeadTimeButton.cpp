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


#include "AMDeadTimeButton.h"
#include "beamline/AMControl.h"

AMDeadTimeButton::AMDeadTimeButton(QWidget *parent)
	: AMToolButton(parent)
{
	inputCountSource_ = 0;
	outputCountSource_ = 0;
	goodReferencePoint_ = 0;
	badReferencePoint_ = 0;
	countsMode_ = Percent;
	acquireTimeControl_ = 0;

	index_ = 0;
	indexSet_ = false;
}

AMDeadTimeButton::AMDeadTimeButton(AMDataSource *inputCountSource, AMDataSource *outputCountSource, double goodReferencePoint, double badReferencePoint, CountsMode countsMode, QWidget *parent)
	: AMToolButton(parent)
{
	inputCountSource_ = 0;
	outputCountSource_ = 0;
	goodReferencePoint_ = goodReferencePoint;
	badReferencePoint_ = badReferencePoint;
	countsMode_ = countsMode;
	acquireTimeControl_ = 0;

	index_ = 0;
	indexSet_ = false;

	setDeadTimeSources(inputCountSource, outputCountSource);
}

AMDeadTimeButton::~AMDeadTimeButton()
{

}

bool AMDeadTimeButton::canDisplayPercentage() const
{
	return hasDeadTimeSources();
}

bool AMDeadTimeButton::canDisplayCounts() const
{
	return hasICRDataSource();
}

bool AMDeadTimeButton::canDisplayCountRate() const
{
	return hasICRDataSource() && hasAcquireTime();
}

bool AMDeadTimeButton::canDisplayIndex() const
{
	return indexSet_;
}

void AMDeadTimeButton::setDeadTimeSources(AMDataSource *inputCountSource, AMDataSource *outputCountSource)
{
	if (inputCountSource_) {
		disconnect(inputCountSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(updateColorState()));
		disconnect(inputCountSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(updateToolTip()));
	}

	if (outputCountSource_) {
		disconnect(outputCountSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(updateColorState()));
		disconnect(outputCountSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(updateToolTip()));
	}

	inputCountSource_ = inputCountSource;
	outputCountSource_ = outputCountSource;

	if (inputCountSource_) {
		connect(inputCountSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(updateColorState()));
		connect(inputCountSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(updateToolTip()));
	}

	if (outputCountSource_) {
		connect(outputCountSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(updateColorState()));
		connect(outputCountSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(updateToolTip()));
	}

	updateColorState();
	updateToolTip();
}

void AMDeadTimeButton::setGoodReferencePoint(double newReference)
{
	goodReferencePoint_ = newReference;
	updateColorState();
}

void AMDeadTimeButton::setBadReferencePoint(double newReference)
{
	badReferencePoint_ = newReference;
	updateColorState();
}

void AMDeadTimeButton::setCountsMode(CountsMode newMode)
{
	countsMode_ = newMode;
	updateToolTip();
}

void AMDeadTimeButton::setAcquireTimeControl(AMControl *newControl)
{
	if (acquireTimeControl_ != newControl) {

		if (acquireTimeControl_)
			disconnect( acquireTimeControl_, 0, this, 0 );

		acquireTimeControl_ = newControl;

		if (acquireTimeControl_) {
			connect( acquireTimeControl_, SIGNAL(connected(bool)), this, SLOT(updateColorState()) );
			connect( acquireTimeControl_, SIGNAL(connected(bool)), this, SLOT(updateToolTip()) );
			connect( acquireTimeControl_, SIGNAL(valueChanged(double)), this, SLOT(updateColorState()) );
			connect( acquireTimeControl_, SIGNAL(valueChanged(double)), this, SLOT(updateToolTip()) );
		}

		updateColorState();
		updateToolTip();
	}
}

void AMDeadTimeButton::setIndex(int newIndex)
{
	if (index_ != newIndex || !indexSet_) {
		index_ = newIndex;
		indexSet_ = true;

		updateToolTip();
	}

}

void AMDeadTimeButton::updateColorState()
{
	ColorState newState = AMToolButton::None;

	if (hasDeadTimeSources() || hasICRDataSource()) {

		// Identify the value used for color state comparison.

		double newValue = badReferencePoint_;

		if (countsMode_ == Percent && canDisplayPercentage())
			newValue = getPercent();
		else if (countsMode_ == Counts && canDisplayCounts())
			newValue = getCounts();
		else if (countsMode_ == CountRate && canDisplayCountRate())
			newValue = getCountRate();

		// Identify the new color state.

		if (newValue < goodReferencePoint_)
			newState = AMToolButton::Good;
		else if (newValue >= goodReferencePoint_ && newValue < badReferencePoint_)
			newState = AMToolButton::Neutral;
		else
			newState = AMToolButton::Bad;
	}

	setColorState(newState);
}

void AMDeadTimeButton::updateToolTip()
{
	QString toolTip = "";

	// Add the index, if we can display index information.

	if (canDisplayIndex())
		toolTip.append(QString("%1").arg(index_));

	// Add separator, if needed.

	if (canDisplayIndex() && isEnabled() && (canDisplayPercentage() || canDisplayCounts() || canDisplayCountRate()))
		toolTip.append(": ");

	// Add the counts information.

	if (countsMode_ == Percent && canDisplayPercentage())
		toolTip.append(QString("%1%").arg(getPercent(), 0, 'f', 0));
	else if (countsMode_ == Counts && canDisplayCounts())
		toolTip.append(QString("%1 counts").arg(getCounts()));
	else if (countsMode_ == CountRate && canDisplayCountRate())
		toolTip.append(QString("%1 counts/s").arg(getCountRate()));

	setToolTip(toolTip);
}

bool AMDeadTimeButton::hasDeadTimeSources() const
{
	return !(outputCountSource_ == 0 || inputCountSource_ == 0);
}

bool AMDeadTimeButton::hasICRDataSource() const
{
	return !(inputCountSource_ == 0);
}

bool AMDeadTimeButton::hasAcquireTime() const
{
	return !(acquireTimeControl_ == 0);
}

double AMDeadTimeButton::getPercent() const
{
	double result = 0;

	if (outputCountSource_ && inputCountSource_)
		result = 100*(1 - double(outputCountSource_->value(AMnDIndex()))/double(inputCountSource_->value(AMnDIndex())));

	return result;
}

double AMDeadTimeButton::getCounts() const
{
	double result = 0;

	if (inputCountSource_)
		result = double(inputCountSource_->value(AMnDIndex()));

	return result;
}

double AMDeadTimeButton::getCountRate() const
{
	double result = 0;

	if (inputCountSource_ && acquireTimeControl_)
		result = double(inputCountSource_->value(AMnDIndex())) / acquireTimeControl_->value();

	return result;
}

double AMDeadTimeButton::getAcquireTime() const
{
	double result = 0;

	if (acquireTimeControl_)
		result = acquireTimeControl_->value();

	return result;
}
