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

#include <QPainter>
#include <QStyleOption>

AMDeadTimeButton::~AMDeadTimeButton(){}

AMDeadTimeButton::AMDeadTimeButton(QWidget *parent)
	: QToolButton(parent)
{
	inputCountSource_ = 0;
	outputCountSource_ = 0;
	goodReferencePoint_ = 0;
	badReferencecPoint_ = 0;
	displayPercent_ = true;
}

AMDeadTimeButton::AMDeadTimeButton(AMDataSource *inputCountSource, AMDataSource *outputCountSource, double goodReferencePoint, double badReferencePoint, bool displayPercent, QWidget *parent)
	: QToolButton(parent)
{
	inputCountSource_ = inputCountSource;
	outputCountSource_ = outputCountSource;
	goodReferencePoint_ = goodReferencePoint;
	badReferencecPoint_ = badReferencePoint;
	displayPercent_ = displayPercent;

	if (inputCountSource_)
		connect(inputCountSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onDeadTimeUpdated()));

	if (outputCountSource_)
		connect(outputCountSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onDeadTimeUpdated()));
}

void AMDeadTimeButton::onDeadTimeUpdated()
{
	if (displayPercent_ && inputCountSource_ && outputCountSource_)
		setToolTip(QString("%1%").arg(100*(1 - double(outputCountSource_->value(AMnDIndex()))/double(inputCountSource_->value(AMnDIndex()))), 0, 'f', 0));
	else if (!displayPercent_ && inputCountSource_)
		setToolTip(QString("%1 counts").arg(double(inputCountSource_->value(AMnDIndex()))));

	update();
}

void AMDeadTimeButton::paintEvent(QPaintEvent *e)
{
	Q_UNUSED(e)

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	// Main button box.  Uses the standand push button style for the basic aspects of the box.
	QStyleOptionButton option;
	option.initFrom(this);
	option.state = QStyle::State_Off;

	if (!isEnabled())
		option.palette = QPalette(Qt::black, QColor(170, 170, 170, 100), Qt::gray, Qt::gray, QColor(170, 170, 170), Qt::gray, Qt::gray, Qt::gray, QColor(170, 170, 170));

	else if (!isChecked()){

		if (hasDeadTimeSources() || hasICRDataSource()) {
			double newValue = 0;

			if (hasDeadTimeSources()){
				newValue = 100*(1 - double(outputCountSource_->value(AMnDIndex()))/double(inputCountSource_->value(AMnDIndex())));

			} else if (hasICRDataSource()) {
				newValue = inputCountSource_->value(AMnDIndex());
			}

			if (newValue < goodReferencePoint_)
				option.palette = QPalette(Qt::black, QColor(20, 220, 20), Qt::gray, Qt::darkGray, QColor(170, 170, 170), Qt::black, Qt::red, Qt::green, QColor(0, 200, 0));
			else if (newValue >= goodReferencePoint_ && newValue < badReferencecPoint_)
				option.palette = QPalette(Qt::black, QColor(220, 220, 20), Qt::gray, Qt::darkGray, QColor(170, 170, 170), Qt::black, Qt::red, Qt::yellow, QColor(200, 200, 0));
			else
				option.palette = QPalette(Qt::black, QColor(220, 20, 20), Qt::gray, Qt::darkGray, QColor(170, 170, 170), Qt::black, Qt::red, Qt::red, QColor(200, 0, 0));
		}

		else
			option.palette = QPalette(Qt::black, QColor(20, 220, 20), Qt::gray, Qt::darkGray, QColor(170, 170, 170), Qt::black, Qt::red, Qt::green, QColor(0, 200, 0));
	}

	else
		option.palette = QPalette(Qt::black, QColor(225, 225, 225, 100), Qt::gray, Qt::gray, QColor(225, 225, 225), Qt::gray, Qt::gray, Qt::gray, QColor(225, 225, 225));

	style()->drawControl(QStyle::CE_PushButton, &option, &painter, this);
}

void AMDeadTimeButton::setGoodReferencePoint(double newReference)
{
	goodReferencePoint_ = newReference;
	update();
}

void AMDeadTimeButton::setBadReferencePoint(double newReference)
{
	badReferencecPoint_ = newReference;
	update();
}

void AMDeadTimeButton::setDisplayAsPercent(bool showPercent)
{
	displayPercent_ = showPercent;
	onDeadTimeUpdated();
}

void AMDeadTimeButton::setDeadTimeSources(AMDataSource *inputCountSource, AMDataSource *outputCountSource)
{
	disconnect(inputCountSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(update()));
	disconnect(outputCountSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(update()));
	inputCountSource_ = inputCountSource;
	outputCountSource_ = outputCountSource;
	connect(inputCountSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(update()));
	connect(outputCountSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(update()));
}

bool AMDeadTimeButton::hasDeadTimeSources() const
{
	return !(outputCountSource_ == 0 || inputCountSource_ == 0);
}

bool AMDeadTimeButton::hasICRDataSource() const
{
	return !(inputCountSource_ == 0);
}
