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

AMDeadTimeButton::AMDeadTimeButton(QWidget *parent)
	: AMToolButton(parent)
{
	inputCountSource_ = 0;
	outputCountSource_ = 0;
	goodReferencePoint_ = 0;
	badReferencePoint_ = 0;
	displayPercent_ = true;
}

AMDeadTimeButton::AMDeadTimeButton(AMDataSource *inputCountSource, AMDataSource *outputCountSource, double goodReferencePoint, double badReferencePoint, bool displayPercent, QWidget *parent)
	: AMToolButton(parent)
{
	inputCountSource_ = 0;
	outputCountSource_ = 0;
	goodReferencePoint_ = goodReferencePoint;
	badReferencePoint_ = badReferencePoint;
	displayPercent_ = displayPercent;

	setDeadTimeSources(inputCountSource, outputCountSource);
}

AMDeadTimeButton::~AMDeadTimeButton()
{

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

void AMDeadTimeButton::setDisplayAsPercent(bool showPercent)
{
	displayPercent_ = showPercent;
	updateToolTip();
}

void AMDeadTimeButton::updateColorState()
{
	ColorState newState = AMToolButton::None;

	if (hasDeadTimeSources() || hasICRDataSource()) {

		// Identify the value used for color state comparison.

		double newValue = badReferencePoint_;

		if (hasDeadTimeSources())
			newValue = 100*(1 - double(outputCountSource_->value(AMnDIndex()))/double(inputCountSource_->value(AMnDIndex())));
		else if (hasICRDataSource())
			newValue = double(inputCountSource_->value(AMnDIndex()));

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
	if (displayPercent_ && inputCountSource_ && outputCountSource_)
		setToolTip(QString("%1%").arg(100*(1 - double(outputCountSource_->value(AMnDIndex()))/double(inputCountSource_->value(AMnDIndex()))), 0, 'f', 0));
	else if (!displayPercent_ && inputCountSource_)
		setToolTip(QString("%1 counts").arg(double(inputCountSource_->value(AMnDIndex()))));

	update();
}

bool AMDeadTimeButton::hasDeadTimeSources() const
{
	return !(outputCountSource_ == 0 || inputCountSource_ == 0);
}

bool AMDeadTimeButton::hasICRDataSource() const
{
	return !(inputCountSource_ == 0);
}
