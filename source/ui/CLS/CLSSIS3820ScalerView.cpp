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

#include "ui/CLS/CLSSIS3820ScalerChannelViewWithDarkCurrent.h"
#include "ui/CLS/CLSDarkCurrentWidget.h"

// CLSSIS3820ScalerView
///////////////////////////////////////////////

CLSSIS3820ScalerView::CLSSIS3820ScalerView(CLSSIS3820Scaler *scaler, bool enableDarkCurrentCorrection, QWidget *parent) :
	QWidget(parent)
{
	scaler_ = scaler;

	enableDarkCurrentCorrection_ = enableDarkCurrentCorrection;

	// Build the top part.
	modeChoice_ = new QComboBox;
	modeChoice_->addItems(QStringList() << "Single Shot" << "Continuous");
	modeChoice_->setCurrentIndex(0);
	connect(modeChoice_, SIGNAL(currentIndexChanged(int)), this, SLOT(setContinuous(int)));
	connect(scaler_, SIGNAL(continuousChanged(bool)), this, SLOT(onContinuousChanged(bool)));

	startButton_ = new QPushButton(QIcon(":/22x22/media-playback-start.png"), "Start");
	startButton_->setMaximumHeight(25);
	connect(scaler_, SIGNAL(scanningChanged(bool)), startButton_, SLOT(setDisabled(bool)));
	connect(startButton_, SIGNAL(clicked()), this, SLOT(startScanning()));

	stopButton_ = new QPushButton(QIcon(":/22x22/media-playback-stop.png"), "Stop");
	stopButton_->setMaximumHeight(25);
	connect(scaler_, SIGNAL(scanningChanged(bool)), stopButton_, SLOT(setEnabled(bool)));
	connect(stopButton_, SIGNAL(clicked()), this, SLOT(stopScanning()));

	startButton_->setDisabled(scaler_->isScanning());
	stopButton_->setEnabled(scaler_->isScanning());

	status_ = new QLabel;
	status_->setPixmap(QIcon(":/OFF.png").pixmap(25));
	connect(scaler_, SIGNAL(scanningChanged(bool)), this, SLOT(onStatusChanged(bool)));

	time_ = new QSpinBox;
	time_->setRange(0, 1000000);
	time_->setValue(1000);
	time_->setSuffix(" ms");
	time_->setFixedWidth(100);
	time_->setAlignment(Qt::AlignCenter);
	connect(time_, SIGNAL(editingFinished()), this, SLOT(setTime()));
	connect(scaler_, SIGNAL(dwellTimeChanged(double)), this, SLOT(onTimeChanged(double)));
	if(scaler_->isConnected())
		onTimeChanged(scaler_->dwellTime());

	QHBoxLayout *timeLayout = new QHBoxLayout;
	timeLayout->addWidget(new QLabel("Dwell Time:"), 0, Qt::AlignRight);
	timeLayout->addWidget(time_);

	scansPerBuffer_ = new QSpinBox;
	scansPerBuffer_->setRange(0, 10000);
	scansPerBuffer_->setValue(1);
	scansPerBuffer_->setFixedWidth(100);
	scansPerBuffer_->setAlignment(Qt::AlignCenter);
	connect(scansPerBuffer_, SIGNAL(editingFinished()), this, SLOT(setScansPerBuffer()));
	connect(scaler_, SIGNAL(scansPerBufferChanged(int)), scansPerBuffer_, SLOT(setValue(int)));
	if(scaler_->isConnected())
		scansPerBuffer_->setValue(scaler_->scansPerBuffer());

	QHBoxLayout *scansPerBufferLayout = new QHBoxLayout;
	scansPerBufferLayout->addWidget(new QLabel("Scans per Buffer:"), 0, Qt::AlignRight);
	scansPerBufferLayout->addWidget(scansPerBuffer_);

	totalScans_ = new QSpinBox;
	totalScans_->setRange(0, 10000);
	totalScans_->setValue(1);
	totalScans_->setFixedWidth(100);
	totalScans_->setAlignment(Qt::AlignCenter);
	connect(totalScans_, SIGNAL(editingFinished()), this, SLOT(setTotalNumberOfScans()));
	connect(scaler_, SIGNAL(totalScansChanged(int)), totalScans_, SLOT(setValue(int)));
	if(scaler_->isConnected())
		totalScans_->setValue(scaler_->totalScans());

	QHBoxLayout *totalScansLayout = new QHBoxLayout;
	totalScansLayout->addWidget(new QLabel("Total Scans"), 0, Qt::AlignRight);
	totalScansLayout->addWidget(totalScans_);

	QHBoxLayout *startAndStopButtons = new QHBoxLayout;
	startAndStopButtons->addWidget(startButton_);
	startAndStopButtons->addWidget(stopButton_);

	QVBoxLayout *statusAndModeLayout = new QVBoxLayout;
	statusAndModeLayout->addWidget(status_, 0, Qt::AlignCenter);
	statusAndModeLayout->addLayout(startAndStopButtons);
	statusAndModeLayout->addWidget(modeChoice_, 0, Qt::AlignCenter);

	QVBoxLayout *spinBoxLayout = new QVBoxLayout;
	spinBoxLayout->addLayout(timeLayout);
	spinBoxLayout->addLayout(scansPerBufferLayout);
	spinBoxLayout->addLayout(totalScansLayout);

	QHBoxLayout *topLayout = new QHBoxLayout;
	topLayout->addLayout(statusAndModeLayout);
	topLayout->addLayout(spinBoxLayout);

	// Dark current widget
	CLSDarkCurrentWidget *darkCurrentWidget = new CLSDarkCurrentWidget(scaler_, this);
	bool showDarkCurrentWidget = false;
	darkCurrentWidget->hide();

	QHBoxLayout *darkCurrentLayout = new QHBoxLayout();
	darkCurrentLayout->addWidget(darkCurrentWidget);
	darkCurrentLayout->addStretch();

	// Build the channel views.
	channelLayout_ = new QVBoxLayout;

	mainVL_ = new QVBoxLayout();
	mainVL_->addLayout(topLayout);
	mainVL_->addLayout(channelLayout_);
	mainVL_->addStretch();
	mainVL_->addLayout(darkCurrentLayout);

	setLayout(mainVL_);

	CLSSIS3820ScalerChannelView *channelView = 0;
	int channelCount = scaler_->channels().count();

	for (int i = 0; i < channelCount; i++){
		CLSSIS3820ScalerChannel *channel = scaler_->channelAt(i);

	if (!showDarkCurrentWidget && channel && channel->detector() && channel->detector()->canDoDarkCurrentCorrection() && enableDarkCurrentCorrection_)
            showDarkCurrentWidget = true;

		if(channel->detector() && channel->detector()->canDoDarkCurrentCorrection() && enableDarkCurrentCorrection_)
			channelView = new CLSSIS3820ScalerChannelViewWithDarkCurrent(channel);
		else
			channelView = new CLSSIS3820ScalerChannelView(scaler_->channelAt(i));

		channelViews_ << channelView;
		connect(channelView, SIGNAL(amplifierViewModeChanged(AMCurrentAmplifierView::ViewMode)), this, SLOT(onAmplifierViewChanged(AMCurrentAmplifierView::ViewMode)));
		connect(channelView, SIGNAL(outputViewModeChanged(CLSSIS3820ScalerChannelView::OutputViewMode)), this, SLOT(onOutputViewModeChanged(CLSSIS3820ScalerChannelView::OutputViewMode)));

		channelLayout_->addWidget(channelView);
		channelView->setVisible(!scaler_->channelAt(i)->customChannelName().isEmpty());
	}


	// if one of the detectors associated with a channel can perform dark current correction, show the 'do dark current' widget.
	if (showDarkCurrentWidget) {
		darkCurrentWidget->show();
	}
}

CLSSIS3820ScalerView::~CLSSIS3820ScalerView(){}

void CLSSIS3820ScalerView::startScanning()
{
	scaler_->setScanning(true);
}

void CLSSIS3820ScalerView::stopScanning()
{
	scaler_->setScanning(false);
}

void CLSSIS3820ScalerView::setContinuous(int index)
{
	scaler_->blockSignals(true);

	if (index == 1 && !scaler_->isContinuous())
		scaler_->setContinuous(true);

	else if (index == 0 && scaler_->isContinuous())
		scaler_->setContinuous(false);

	scaler_->blockSignals(false);
}

void CLSSIS3820ScalerView::onContinuousChanged(bool isContinuous)
{
	scaler_->blockSignals(true);
	modeChoice_->setCurrentIndex(isContinuous ? 1 : 0);
	scaler_->blockSignals(false);
}

void CLSSIS3820ScalerView::setTime()
{
	scaler_->blockSignals(true);

	if ((double)time_->value()/1000.0 != scaler_->dwellTime())
		scaler_->setDwellTime((double)time_->value()/1000.0);

	scaler_->blockSignals(false);
}

void CLSSIS3820ScalerView::onTimeChanged(double time)
{
	time_->setValue(int(time*1000));
}

void CLSSIS3820ScalerView::onStatusChanged(bool status)
{
	status_->setPixmap(QIcon(status ? ":/ON.png" : ":/OFF.png").pixmap(25));
}

void CLSSIS3820ScalerView::setScansPerBuffer()
{
	scaler_->blockSignals(true);

	if (scansPerBuffer_->value() != scaler_->scansPerBuffer())
		scaler_->setScansPerBuffer(scansPerBuffer_->value());

	scaler_->blockSignals(false);
}

void CLSSIS3820ScalerView::setTotalNumberOfScans()
{
	scaler_->blockSignals(true);

	if (totalScans_->value() != scaler_->totalScans())
		scaler_->setTotalScans(totalScans_->value());

	scaler_->blockSignals(false);
}

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

bool CLSSIS3820ScalerView::darkCurrentCorrectionEnabled() const
{
	return enableDarkCurrentCorrection_;
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

	amplifierView_ = 0;

	connect(channel_, SIGNAL(currentAmplifierAttached()), this, SLOT(onNewCurrentAmplifierAttached()));

	if (channel_->currentAmplifier()){

		amplifierView_ = channel_->currentAmplifier()->createView();
		connect( amplifierView_, SIGNAL(viewModeChanged(AMCurrentAmplifierView::ViewMode)), this, SIGNAL(amplifierViewModeChanged(AMCurrentAmplifierView::ViewMode)) );

	}

	scalerOutput_ = new QToolButton;
	scalerOutput_->setAutoRaise(true);


	connect(channel_, SIGNAL(readingChanged(int)), this, SLOT(onReadingChanged()));
	connect(scalerOutput_, SIGNAL(clicked()), this, SLOT(onScalerOutputClicked()));

	statusLabel_ = new QLabel;
	statusLabel_->setPixmap(QIcon(":/OFF.png").pixmap(22));
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

		statusLabel_->setPixmap(QIcon(":/ON.png").pixmap(22));
		statusLabel_->setToolTip("");
	}

	else if (channel_->voltageTooLow()){

		statusLabel_->setPixmap(QIcon(":/RED.png").pixmap(22));
		statusLabel_->setToolTip(QString("The voltage is too low!\nIncrease the sensitivity until the voltage is higher than %1 V.").arg(channel_->minimumVoltage()));
	}

	else if (channel_->voltageTooHigh()){

		statusLabel_->setPixmap(QIcon(":/RED.png").pixmap(22));
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
