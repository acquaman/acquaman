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
	status_->setPixmap(QIcon(":/OFF.png").pixmap(25, 25));
	connect(element_, SIGNAL(statusChanged(bool)), this, SLOT(onStatusChanged(bool)));

	QLabel *name = new QLabel(element_->name());
	connect(element_, SIGNAL(nameChanged(QString)), name, SLOT(setText(QString)));

	enable_ = new QPushButton;
	enable_->setCheckable(true);
	enable_->setChecked(false);
	enable_->setPalette(QPalette(Qt::red));
	connect(element_, SIGNAL(enabledChanged(bool)), enable_, SLOT(setChecked(bool)));
	connect(element_, SIGNAL(enabledChanged(bool)), this, SLOT(onEnabledChanged(bool)));
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

	QHBoxLayout *elementLayout = new QHBoxLayout;
	elementLayout->addWidget(status_);
	elementLayout->addWidget(name);
	elementLayout->addWidget(enable_);
	elementLayout->addWidget(time_);

	if(element_->isConnected()){
		onStatusChanged(element_->status());
		enable_->setChecked(element_->isEnabled());
		onEnabledChanged(element_->isEnabled());
		time_->setValue(element_->time());
		checkTimeUnits();
	}

	setLayout(elementLayout);
}

void CLSSynchronizedDwellTimeElementView::checkTimeUnits()
{
	if (element_->units() != "?"){

		time_->setSuffix(" " + element_->units());
		disconnect(element_, SIGNAL(timeChanged(double)), this, SLOT(checkTimeUnits()));
	}
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

	overallStatus_ = new QLabel;
	overallStatus_->setPixmap(QIcon(":/OFF.png").pixmap(30, 30));
	connect(dwellTime_, SIGNAL(statusChanged(bool)), this, SLOT(onStatusChanged(bool)));

	mode_ = new QPushButton;
	mode_->setCheckable(true);
	mode_->setChecked(false);
	connect(dwellTime_, SIGNAL(modeChanged(CLSSynchronizedDwellTime::Mode)), this, SLOT(onModeChanged(CLSSynchronizedDwellTime::Mode)));
	if(dwellTime_->isConnected())
		onModeChanged(dwellTime_->mode());
	connect(mode_, SIGNAL(toggled(bool)), this, SLOT(setMode(bool)));

	masterTime_ = new QDoubleSpinBox;
	masterTime_->setDecimals(2);
	masterTime_->setValue(0);
	masterTime_->setMinimum(0);
	masterTime_->setMaximum(1000);
	masterTime_->setSuffix(" s");
	masterTime_->setAlignment(Qt::AlignCenter);
	connect(dwellTime_, SIGNAL(timeChanged(double)), masterTime_, SLOT(setValue(double)));
	if(dwellTime_->isConnected())
		masterTime_->setValue(dwellTime_->time());
	connect(masterTime_, SIGNAL(editingFinished()), this, SLOT(setTime()));

	scanning_ = new QPushButton;
	scanning_->setCheckable(true);
	scanning_->setChecked(false);
	connect(dwellTime_, SIGNAL(scanningChanged(bool)), this, SLOT(onScanningChanged(bool)));
	if(dwellTime_->isConnected())
		onScanningChanged(dwellTime_->isScanning());
	connect(scanning_, SIGNAL(clicked()), this, SLOT(toggleScanning()));

	QHBoxLayout *topRow = new QHBoxLayout;
	topRow->addWidget(overallStatus_, 0, Qt::AlignCenter);
	topRow->addWidget(scanning_);
	topRow->addWidget(mode_);
	topRow->addWidget(masterTime_);

	QVBoxLayout *groupBoxLayout = new QVBoxLayout;
	groupBoxLayout->addLayout(topRow);

	for (int i = 0; i < dwellTime_->elementCount(); i++){

		elViews_ << new CLSSynchronizedDwellTimeElementView(dwellTime_->elementAt(i));
		elViews_.last()->hide();
		groupBoxLayout->addWidget(elViews_.last());
	}

	QGroupBox *dwellTimeBox = new QGroupBox("Synchronized Dwell Time");
	dwellTimeBox->setLayout(groupBoxLayout);

	QVBoxLayout *dwellTimeLayout = new QVBoxLayout;
	dwellTimeLayout->addWidget(dwellTimeBox);

	setMaximumSize(400, 105);

	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)));
	setContextMenuPolicy(Qt::CustomContextMenu);

	setLayout(dwellTimeLayout);
}

void CLSSynchronizedDwellTimeView::setAdvancedViewVisible(bool advancedViewVisible){
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

void CLSSynchronizedDwellTimeView::setTime()
{
	dwellTime_->blockSignals(true);
	dwellTime_->setTime(masterTime_->value());
	dwellTime_->blockSignals(false);
}

void CLSSynchronizedDwellTimeView::setMode(bool mode)
{
	dwellTime_->blockSignals(true);

	if (mode && dwellTime_->mode() != CLSSynchronizedDwellTime::Continuous){

		dwellTime_->setMode(CLSSynchronizedDwellTime::Continuous);
		onModeChanged(CLSSynchronizedDwellTime::Continuous);
	}
	else if (!mode && dwellTime_->mode() != CLSSynchronizedDwellTime::SingleShot){

		dwellTime_->setMode(CLSSynchronizedDwellTime::SingleShot);
		onModeChanged(CLSSynchronizedDwellTime::SingleShot);
	}

	dwellTime_->blockSignals(false);
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
		/*
		advancedView_ = !advancedView_;

		if (advancedView_)
			setMaximumHeight(105 + 50*dwellTime_->elementCount());
		else
			setMaximumHeight(105);

		for (int i = 0; i < elViews_.size(); i++)
			elViews_.at(i)->setVisible(advancedView_);
		*/
	}
}
