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


#include "CLSSynchronizedDwellTimeView.h"

#include "AMQEvents.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QMenu>
#include <QAction>

 CLSSynchronizedDwellTimeElementView::~CLSSynchronizedDwellTimeElementView(){}
CLSSynchronizedDwellTimeElementView::CLSSynchronizedDwellTimeElementView(CLSSynchronizedDwellTimeElement *dwellTimeElement, QWidget *parent)
	: QWidget(parent)
{
	element_ = dwellTimeElement;

	status_ = new QLabel;
	status_->setPixmap(QIcon(":/32x32/greenLEDOff.png").pixmap(25, 25));
	connect(element_, SIGNAL(statusChanged(bool)), this, SLOT(onStatusChanged(bool)));

	QLabel *name = new QLabel(element_->name());
	connect(element_, SIGNAL(nameChanged(QString)), name, SLOT(setText(QString)));

	enable_ = new QCheckBox;
	enable_->setChecked(false);
	connect(element_, SIGNAL(enabledChanged(bool)), enable_, SLOT(setChecked(bool)));
	connect(enable_, SIGNAL(toggled(bool)), this, SLOT(setEnabledState(bool)));

	time_ = new QDoubleSpinBox;
	time_->setDecimals(2);
	time_->setValue(0);
	time_->setMinimum(0);
	time_->setMaximum(1e6);
	time_->setAlignment(Qt::AlignCenter);
	time_->setSuffix(" " + element_->units());
	connect(element_, SIGNAL(timeChanged(double)), time_, SLOT(setValue(double)));
	connect(element_, SIGNAL(timeChanged(double)), this, SLOT(checkTimeUnits()));
	connect(time_, SIGNAL(editingFinished()), this, SLOT(setTime()));

	elementLayout_ = new QGridLayout();
	elementLayout_->addWidget(enable_, 0, 0);
	elementLayout_->addWidget(name, 0, 1);
	elementLayout_->addWidget(time_,0 , 2);
	elementLayout_->addWidget(status_, 0, 3);




	if(element_->isConnected()){
		onStatusChanged(element_->status());
		enable_->setChecked(element_->isEnabled());
		time_->setValue(element_->time());
		checkTimeUnits();
	}

	setLayout(elementLayout_);
}

void CLSSynchronizedDwellTimeElementView::checkTimeUnits()
{
	if (element_->units() != "?"){

		time_->setSuffix(" " + element_->units());
		disconnect(element_, SIGNAL(timeChanged(double)), this, SLOT(checkTimeUnits()));
	}
}

void CLSSynchronizedDwellTimeElementView::resizeEvent(QResizeEvent *re)
{

	elementLayout_->setColumnMinimumWidth(0, int(re->size().width() * 0.05));
	elementLayout_->setColumnMinimumWidth(1, int(re->size().width() * 0.35));
	elementLayout_->setColumnMinimumWidth(2, int(re->size().width() * 0.4));
	elementLayout_->setColumnMinimumWidth(3, int(re->size().width() * 0.05));

}

void CLSSynchronizedDwellTimeElementView::setTime()
{
	double newTime = time_->value();

	if (element_->units() == "ms")
		newTime /= 1000;

	if (newTime != element_->time())
		element_->setTime(newTime);
}

 CLSSynchronizedDwellTimeView::~CLSSynchronizedDwellTimeView(){}
CLSSynchronizedDwellTimeView::CLSSynchronizedDwellTimeView(CLSSynchronizedDwellTime *dwellTime, QWidget *parent)
	: QWidget(parent)
{
	dwellTime_ = dwellTime;
	advancedView_ = false;
	QGridLayout* layout = new QGridLayout();

	statusLabel_ = new QLabel();

	startButton_ = new QPushButton(QIcon(":/22x22/media-playback-start.png"), "Start");
	stopButton_ = new QPushButton(QIcon(":/22x22/media-playback-stop.png"), "Stop");
	modeComboBox_ = new QComboBox();
	modeComboBox_->addItems(QStringList() << "Single Shot" << "Continuous");
	dwellTimeLabel_ = new QLabel("Dwell Time:");
	dwellTimeSpinBox_ = new QSpinBox();

	dwellTimeSpinBox_->setSuffix(" s");
	dwellTimeSpinBox_->setMinimum(0);
	dwellTimeSpinBox_->setMaximum(1e6);

	layout->addWidget(startButton_, 0, 0, 1, 2);
	layout->addWidget(stopButton_, 0, 2, 1, 2);
	layout->addWidget(statusLabel_, 0, 4, 1, 1, Qt::AlignCenter);
	layout->addWidget(modeComboBox_, 1, 0, 1, 3);
	layout->addWidget(dwellTimeLabel_, 1, 3, 1, 1);
	layout->addWidget(dwellTimeSpinBox_, 1, 4, 1, 1);


	for(int iElement=0; iElement < dwellTime_->elementCount(); iElement++)
	{
		elViews_ << new CLSSynchronizedDwellTimeElementView(dwellTime_->elementAt(iElement));
		elViews_.last()->hide();
		layout->addWidget(elViews_.last(), iElement + 4, 0, 1, 5);
	}

	if(dwellTime_->isConnected())
	{
		dwellTimeSpinBox_->setValue(dwellTime_->time());
		startButton_->setEnabled(!dwellTime_->isScanning());
		stopButton_->setEnabled(dwellTime_->isScanning());
		statusLabel_->setPixmap(QIcon(dwellTime_->isScanning() ? ":/32x32/greenLEDOn.png" : ":/32x32/greenLEDOff.png").pixmap(25));

		if(dwellTime_->mode() == CLSSynchronizedDwellTime::SingleShot)
			modeComboBox_->setCurrentIndex(0);
		else
			modeComboBox_->setCurrentIndex(1);
	}

	setMaximumSize(400, 105);

	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)));
	connect(dwellTime_, SIGNAL(timeChanged(double)), this, SLOT(onDwellTimeChanged(double)));
	connect(dwellTime_, SIGNAL(scanningChanged(bool)), this, SLOT(onScanningChanged(bool)));
	connect(dwellTime_, SIGNAL(modeChanged(CLSSynchronizedDwellTime::Mode)), this, SLOT(onModeChanged(CLSSynchronizedDwellTime::Mode)));
	connect(dwellTime_, SIGNAL(connected(bool)), this, SLOT(onDwellTimeConnected(bool)));
	connect(dwellTime_, SIGNAL(statusChanged(bool)), this, SLOT(onStatusChanged(bool)));

	connect(dwellTimeSpinBox_, SIGNAL(valueChanged(int)), this, SLOT(onDwellTimeSpinBoxValueChanged()));
	connect(startButton_, SIGNAL(clicked()), this, SLOT(onStartButtonClicked()));
	connect(stopButton_, SIGNAL(clicked()), this, SLOT(onStopButtonClicked()));
	connect(modeComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onModeComboBoxCurrentIndexChanged(int)));

	setContextMenuPolicy(Qt::CustomContextMenu);

	this->setLayout(layout);
}

void CLSSynchronizedDwellTimeView::onStatusChanged(bool status)
{
	statusLabel_->setPixmap(QIcon(status ? ":/32x32/greenLEDOn.png" : ":/32x32/greenLEDOff.png").pixmap(25));
}

void CLSSynchronizedDwellTimeView::setAdvancedViewVisible(bool advancedViewVisible)
{
	if(advancedView_ == advancedViewVisible)
		return;

	advancedView_ = advancedViewVisible;

	if (advancedView_)
		setMaximumHeight(105 + 50*dwellTime_->elementCount());
	else
		setMaximumHeight(105);

	for (int i = 0; i < elViews_.size(); i++)
		elViews_.at(i)->setVisible(advancedView_);
}

void CLSSynchronizedDwellTimeView::onCustomContextMenuRequested(QPoint pos)
{
	QMenu popup(this);

	QAction *temp = popup.addAction("Advanced View");
	temp->setCheckable(true);
	temp->setChecked(advancedView_);

	temp = popup.exec(mapToGlobal(pos));

	// If a valid action was selected.
	if (temp && temp->text() == "Advanced View"){

		setAdvancedViewVisible(!advancedView_);
	}
}

void CLSSynchronizedDwellTimeView::onDwellTimeChanged(double value)
{
	dwellTimeSpinBox_->setValue(value);
}

void CLSSynchronizedDwellTimeView::onScanningChanged(bool value)
{
	startButton_->setEnabled(!value);
	stopButton_->setEnabled(value);
}

void CLSSynchronizedDwellTimeView::onDwellTimeConnected(bool value)
{
	if(value)
	{
		dwellTimeSpinBox_->setValue(dwellTime_->time());
		startButton_->setEnabled(!dwellTime_->isScanning());
		stopButton_->setEnabled(dwellTime_->isScanning());
		statusLabel_->setPixmap(QIcon(dwellTime_->isScanning() ? ":/32x32/greenLEDOn.png" : ":/32x32/greenLEDOff.png").pixmap(25));
		if(dwellTime_->mode() == CLSSynchronizedDwellTime::SingleShot)
			modeComboBox_->setCurrentIndex(0);
		else
			modeComboBox_->setCurrentIndex(1);
	}
}

void CLSSynchronizedDwellTimeView::onModeChanged(CLSSynchronizedDwellTime::Mode)
{
	modeComboBox_->blockSignals(true);
	if(dwellTime_->mode() == CLSSynchronizedDwellTime::SingleShot)
		modeComboBox_->setCurrentIndex(0);
	else
		modeComboBox_->setCurrentIndex(1);
	modeComboBox_->blockSignals(false);
}

void CLSSynchronizedDwellTimeView::onDwellTimeSpinBoxValueChanged()
{
	dwellTime_->blockSignals(true);

	dwellTime_->setTime(dwellTimeSpinBox_->value());

	dwellTime_->blockSignals(false);
}

void CLSSynchronizedDwellTimeView::onStartButtonClicked()
{
	dwellTime_->blockSignals(true);

	if(!dwellTime_->isScanning())
		dwellTime_->startScanning(true);

	dwellTime_->blockSignals(false);
}

void CLSSynchronizedDwellTimeView::onStopButtonClicked()
{
	dwellTime_->blockSignals(true);

	if(dwellTime_->isScanning())
		dwellTime_->startScanning(false);

	dwellTime_->blockSignals(false);
}

void CLSSynchronizedDwellTimeView::onModeComboBoxCurrentIndexChanged(int index)
{
	dwellTime_->blockSignals(true);

	if(index == 0)
		dwellTime_->setMode(CLSSynchronizedDwellTime::SingleShot);
	else
		dwellTime_->setMode(CLSSynchronizedDwellTime::Continuous);

	dwellTime_->blockSignals(false);
}
