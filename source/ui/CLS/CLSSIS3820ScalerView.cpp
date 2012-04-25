/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QLabel>

#include "beamline/CLS/CLSSIS3820Scaler.h"

// CLSSIS3820ScalerView
///////////////////////////////////////////////

CLSSIS3820ScalerView::CLSSIS3820ScalerView(CLSSIS3820Scaler *scaler, QWidget *parent) :
	QWidget(parent)
{
	scaler_ = scaler;

	if(scaler_ && scaler_->isConnected()){

		// Build the top part.
		modeButton_ = new QPushButton("Single Shot");
		modeButton_->setCheckable(true);
		modeButton_->setChecked(false);
		connect(modeButton_, SIGNAL(toggled(bool)), this, SLOT(setContinuous(bool)));
		connect(scaler_, SIGNAL(continuousChanged(bool)), modeButton_, SLOT(setChecked(bool)));

		scanningButton_ = new QPushButton("Stopped");
		scanningButton_->setCheckable(true);
		scanningButton_->setChecked(false);
		scanningButton_->setPalette(QPalette(Qt::red));
		connect(scanningButton_, SIGNAL(toggled(bool)), this, SLOT(setScanning(bool)));

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

		QHBoxLayout *totalScansLayout = new QHBoxLayout;
		totalScansLayout->addWidget(new QLabel("Total Scans"), 0, Qt::AlignRight);
		totalScansLayout->addWidget(totalScans_);

		QVBoxLayout *statusAndModeLayout = new QVBoxLayout;
		statusAndModeLayout->addWidget(status_, 0, Qt::AlignCenter);
		statusAndModeLayout->addWidget(scanningButton_, 0, Qt::AlignCenter);
		statusAndModeLayout->addWidget(modeButton_, 0, Qt::AlignCenter);

		QVBoxLayout *spinBoxLayout = new QVBoxLayout;
		spinBoxLayout->addLayout(timeLayout);
		spinBoxLayout->addLayout(scansPerBufferLayout);
		spinBoxLayout->addLayout(totalScansLayout);

		QHBoxLayout *topLayout = new QHBoxLayout;
		topLayout->addLayout(statusAndModeLayout);
		topLayout->addLayout(spinBoxLayout);

		// Build the channel views.
		QVBoxLayout *channelLayoutLeftTop = new QVBoxLayout;
		QVBoxLayout *channelLayoutRightTop = new QVBoxLayout;
		QVBoxLayout *channelLayoutLeftBottom = new QVBoxLayout;
		QVBoxLayout *channelLayoutRightBottom = new QVBoxLayout;

		CLSSIS3820ScalerChannelView *channelView = 0;
		int channelCount = scaler_->channels().count();

		for(int x = 0; x < channelCount; x++){

			channelView = new CLSSIS3820ScalerChannelView(scaler_->channelAt(x));

			if(x < channelCount/4)
				channelLayoutLeftTop->addWidget(channelView);

			else if(x < channelCount/2 && x >= channelCount/4)
				channelLayoutRightTop->addWidget(channelView);

			else if(x >= channelCount/2 && x < 3*channelCount/4)
				channelLayoutLeftBottom->addWidget(channelView);

			else
				channelLayoutRightBottom->addWidget(channelView);
		}

		QHBoxLayout *bottomHL1 = new QHBoxLayout();
		bottomHL1->addLayout(channelLayoutLeftTop);
		bottomHL1->addLayout(channelLayoutRightTop);
		bottomHL1->setContentsMargins(10, 0, 10, 5);

		QHBoxLayout *bottomHL2 = new QHBoxLayout();
		bottomHL2->addLayout(channelLayoutLeftBottom);
		bottomHL2->addLayout(channelLayoutRightBottom);
		bottomHL2->setContentsMargins(10, 5, 10, 10);

		QVBoxLayout *mainVL = new QVBoxLayout();
		mainVL->addLayout(topLayout);
		mainVL->addLayout(bottomHL1);
		mainVL->addLayout(bottomHL2);

		QHBoxLayout *constrainingHL = new QHBoxLayout();
		constrainingHL->addLayout(mainVL);
		constrainingHL->addStretch(10);

		setLayout(constrainingHL);
	}
}

void CLSSIS3820ScalerView::setScanning(bool scanning)
{
	scaler_->blockSignals(true);

	if (scanning != scaler_->isScanning())
		scaler_->setScanning(scanning);

	scaler_->blockSignals(false);
}

void CLSSIS3820ScalerView::setContinuous(bool isContinuous)
{
	scaler_->blockSignals(true);

	if (isContinuous != scaler_->isContinuous())
		scaler_->setContinuous(isContinuous);

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
	if (status){

		status_->setPixmap(QIcon(":/ON.png").pixmap(25));
		scanningButton_->setText("Scanning");
		scanningButton_->setPalette(QPalette(Qt::green));
	}
	else{

		status_->setPixmap(QIcon(":/OFF.png").pixmap(25));
		scanningButton_->setText("Stopped");
		scanningButton_->setPalette(QPalette(Qt::red));
		if(scanningButton_->isChecked())
			scanningButton_->setChecked(false);
	}
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

// CLSSIS3820ScalerChannelView
//////////////////////////////////////////////////////////////

CLSSIS3820ScalerChannelView::CLSSIS3820ScalerChannelView(CLSSIS3820ScalerChannel *channel, QWidget *parent)
	: QWidget(parent)
{
	channel_ = channel;

	QCheckBox *enableBox = new QCheckBox;
	if(channel_->isConnected() && channel_->isEnabled())
		enableBox->setChecked(true);
	connect(channel_, SIGNAL(enabledChanged(bool)), enableBox, SLOT(setChecked(bool)));
	connect(enableBox, SIGNAL(toggled(bool)), channel_, SLOT(setEnabled(bool)));

	readingLabel_ = new QLabel;
	if(channel_->isConnected())
		onReadingChanged(channel_->reading());
	connect(channel_, SIGNAL(readingChanged(int)), this, SLOT(onReadingChanged(int)));

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(new QLabel(QString("%1)").arg(channel_->index())), 0, Qt::AlignLeft);
	layout->addSpacing((channel_->index() >= 10) ? 0 : 8);
	layout->addWidget(enableBox, 0, Qt::AlignLeft);
	layout->addStretch();
	layout->addWidget(readingLabel_, 0, Qt::AlignCenter);
	if( (channel_->index() < 8) || (channel_->index() > 15 && channel_->index() < 24) )
		layout->setContentsMargins(0,0,5,0);
	else
		layout->setContentsMargins(5,0,0,0);

	setLayout(layout);
}

void CLSSIS3820ScalerChannelView::onReadingChanged(int reading)
{
	readingLabel_->setText(QString("%1 counts").arg(reading));
}
