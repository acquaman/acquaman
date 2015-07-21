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


#include "CLSSIS3820ScalerView.h"

#include <QVBoxLayout>

#include "beamline/CLS/CLSSIS3820Scaler.h"

#include "ui/CLS/CLSSIS3820ScalerDarkCurrentWidget.h"

// CLSSIS3820ScalerView
///////////////////////////////////////////////

CLSSIS3820ScalerView::CLSSIS3820ScalerView(CLSSIS3820Scaler *scaler, QWidget *parent) :
	QWidget(parent)
{
	scaler_ = scaler;

	// Build the top scaler controls.
	controlsView_ = new CLSSIS3820ScalerControlsView(scaler_);

	// Build the channel views.
	channelLayout_ = new QVBoxLayout;

	mainVL_ = new QVBoxLayout();
	mainVL_->addWidget(controlsView_);
	mainVL_->addLayout(channelLayout_);

	setLayout(mainVL_);

	CLSSIS3820ScalerChannelView *channelView = 0;
	int channelCount = scaler_->channels().count();

	for (int i = 0; i < channelCount; i++) {
		channelView = new CLSSIS3820ScalerChannelView(scaler_->channelAt(i));
		channelViews_ << channelView;

		connect(channelView, SIGNAL(amplifierViewModeChanged(AMCurrentAmplifierView::ViewMode)), this, SLOT(onAmplifierViewChanged(AMCurrentAmplifierView::ViewMode)));
		connect(channelView, SIGNAL(outputViewModeChanged(CLSSIS3820ScalerChannelView::OutputViewMode)), this, SLOT(onOutputViewModeChanged(CLSSIS3820ScalerChannelView::OutputViewMode)));

		channelLayout_->addWidget(channelView);
		channelView->setVisible(!scaler_->channelAt(i)->customChannelName().isEmpty());
	}
}

CLSSIS3820ScalerView::~CLSSIS3820ScalerView(){}

void CLSSIS3820ScalerView::onAmplifierViewChanged(AMCurrentAmplifierView::ViewMode mode)
{
	foreach (CLSSIS3820ScalerChannelView *channel, channelViews_){

		channel->blockSignals(true);
		channel->setAmplifierViewMode(mode);
		channel->blockSignals(false);
	}
}

void CLSSIS3820ScalerView::onOutputViewModeChanged(CLSSIS3820ScalerChannelView::OutputViewMode mode)
{
	foreach (CLSSIS3820ScalerChannelView *channel, channelViews_){

		channel->blockSignals(true);
		channel->setOutputViewMode(mode);
		channel->blockSignals(false);
	}
}

void CLSSIS3820ScalerView::setAmplifierViewPrecision(int newPrecision)
{
	foreach (CLSSIS3820ScalerChannelView *view, channelViews_)
	{
		if(view->hasAmplifierView())
			view->setAmplifierViewPrecision(newPrecision);
	}
}

void CLSSIS3820ScalerView::setAmplifierViewFormat(char newFormat)
{
	foreach (CLSSIS3820ScalerChannelView *view, channelViews_)
	{
		if(view->hasAmplifierView())
			view->setAmplifierViewFormat(newFormat);
	}
}

// CLSSIS3820ScalerChannelView
//////////////////////////////////////////////////////////////

CLSSIS3820ScalerChannelView::CLSSIS3820ScalerChannelView(CLSSIS3820ScalerChannel *channel, QWidget *parent)
	: QWidget(parent)
{
	channel_ = channel;
	viewMode_ = Counts;

	enableBox_ = new QCheckBox;

	if(channel_->isConnected() && channel_->isEnabled())
		enableBox_->setChecked(true);

	connect(channel_, SIGNAL(enabledChanged(bool)), enableBox_, SLOT(setChecked(bool)));
	connect(enableBox_, SIGNAL(toggled(bool)), channel_, SLOT(setEnabled(bool)));

	channelName_ = new QLabel(channel_->customChannelName());
	channelName_->setFixedWidth(80);

	amplifierView_ = 0;

	connect(channel_, SIGNAL(currentAmplifierAttached()), this, SLOT(onNewCurrentAmplifierAttached()));

	if (channel_->currentAmplifier()){

		amplifierView_ = channel_->currentAmplifier()->createView();
		connect( amplifierView_, SIGNAL(viewModeChanged(AMCurrentAmplifierView::ViewMode)), this, SIGNAL(amplifierViewModeChanged(AMCurrentAmplifierView::ViewMode)) );

	}

	scalerOutput_ = new QToolButton;
	scalerOutput_->setMaximumWidth(100);
	scalerOutput_->setAutoRaise(true);


	connect(channel_, SIGNAL(readingChanged(int)), this, SLOT(onReadingChanged()));
	connect(scalerOutput_, SIGNAL(clicked()), this, SLOT(onScalerOutputClicked()));

	statusLabel_ = new QLabel;
	statusLabel_->setPixmap(QIcon(":/22x22/greenLEDOff.png").pixmap(22));
	connect(channel_, SIGNAL(voltageRangeChanged(AMRange)), this, SLOT(updateStatusLabel()));

	if(channel_->isConnected())
		onReadingChanged();

	channelLayout_ = new QHBoxLayout;
	channelLayout_->addWidget(enableBox_, 0, Qt::AlignLeft);
	channelLayout_->addWidget(channelName_, 0, Qt::AlignCenter);

	if (amplifierView_)
		channelLayout_->addWidget(amplifierView_, 0, Qt::AlignCenter);

	channelLayout_->addWidget(scalerOutput_, 0, Qt::AlignCenter);
	channelLayout_->addWidget(statusLabel_, 0, Qt::AlignRight);
	channelLayout_->setContentsMargins(1, 1, 1, 1);

	setLayout(channelLayout_);

	statusLabel_->setVisible(channel_->voltageRange().isValid());
}

CLSSIS3820ScalerChannelView::~CLSSIS3820ScalerChannelView(){}

bool CLSSIS3820ScalerChannelView::hasAmplifierView() const
{
	return amplifierView_ != 0;
}

void CLSSIS3820ScalerChannelView::onReadingChanged()
{
	if (viewMode_ == Counts)
		scalerOutput_->setText(QString("%1 counts").arg(channel_->reading()));

	else if (viewMode_ == Voltage)
		scalerOutput_->setText(QString("%1 V").arg(channel_->voltage(), 0, 'f', 2));

	updateStatusLabel();
}

void CLSSIS3820ScalerChannelView::updateStatusLabel()
{
	if (channel_->withinLinearRange()){

		statusLabel_->setPixmap(QIcon(":/22x22/greenLEDOn.png").pixmap(22));
		statusLabel_->setToolTip("");
	}

	else if (channel_->voltageTooLow()){

		statusLabel_->setPixmap(QIcon(":/22x22/redLEDOn.png").pixmap(22));
		statusLabel_->setToolTip(QString("The voltage is too low!\nIncrease the sensitivity until the voltage is higher than %1 V.").arg(channel_->minimumVoltage()));
	}

	else if (channel_->voltageTooHigh()){

		statusLabel_->setPixmap(QIcon(":/22x22/redLEDOn.png").pixmap(22));
		statusLabel_->setToolTip(QString("The voltage is too high!\nDecrease the sensitivity until the voltage is lower than %1 V.").arg(channel_->maximumVoltage()));
	}
}

void CLSSIS3820ScalerChannelView::onScalerOutputClicked()
{
	if (viewMode_ == Counts){

		viewMode_ = Voltage;
		disconnect(channel_, SIGNAL(readingChanged(int)), this, SLOT(onReadingChanged()));
		connect(channel_, SIGNAL(voltageChanged(double)), this, SLOT(onReadingChanged()));
	}

	else if (viewMode_ == Voltage){

		viewMode_ = Counts;
		connect(channel_, SIGNAL(readingChanged(int)), this, SLOT(onReadingChanged()));
		disconnect(channel_, SIGNAL(voltageChanged(double)), this, SLOT(onReadingChanged()));
	}

	emit outputViewModeChanged(viewMode_);
	onReadingChanged();
}

void CLSSIS3820ScalerChannelView::setAmplifierViewMode(AMCurrentAmplifierView::ViewMode newMode)
{
	if (channel_->currentAmplifier())
		amplifierView_->setViewMode(newMode);
}

void CLSSIS3820ScalerChannelView::setOutputViewMode(CLSSIS3820ScalerChannelView::OutputViewMode mode)
{
	if (viewMode_ != mode){

		viewMode_ = mode;
		onReadingChanged();
		emit outputViewModeChanged(viewMode_);
	}
}

void CLSSIS3820ScalerChannelView::setEnableCheckBoxVisibility(bool visible)
{
	enableBox_->setVisible(visible);
}

void CLSSIS3820ScalerChannelView::setCustomNameVisibility(bool visible)
{
	channelName_->setVisible(visible);
}

void CLSSIS3820ScalerChannelView::setSR570Visibility(bool visible)
{
	if (amplifierView_)
		amplifierView_->setVisible(visible);
}

void CLSSIS3820ScalerChannelView::setOutputVisibility(bool visible)
{
	scalerOutput_->setVisible(visible);
}

void CLSSIS3820ScalerChannelView::setStatusLabelVisibility(bool visible)
{
	statusLabel_->setVisible(visible);
}

void CLSSIS3820ScalerChannelView::onNewCurrentAmplifierAttached()
{
	// If one already exists, lets get rid of it before doing anything else.
	if (amplifierView_)
		delete channelLayout_->takeAt(channelLayout_->indexOf(amplifierView_));

	amplifierView_ = channel_->currentAmplifier()->createView();
	channelLayout_->insertWidget(2, amplifierView_, 0, Qt::AlignCenter);
}

void CLSSIS3820ScalerChannelView::setAmplifierViewPrecision(int newPrecision)
{
	if(amplifierView_)
		amplifierView_->setPrecision(newPrecision);
}

void CLSSIS3820ScalerChannelView::setAmplifierViewFormat(char newFormat)
{
	if(amplifierView_)
		amplifierView_->setFormat(newFormat);
}
