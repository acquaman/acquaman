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


#include "SGMAmptekCoordinator.h"

#include "beamline/AMPVControl.h"

 SGMAmptekCoordinator::~SGMAmptekCoordinator(){}
SGMAmptekCoordinator::SGMAmptekCoordinator(QObject *parent) :
	QObject(parent)
{
	amptekConnectedOnce_ = false;

	AmptekAllStartAcquisitionControl_ = new AMPVControl("AmptekAllStartAcquisition", "amptek:sdd:all:spectrum:start", "amptek:sdd:all:spectrum:start", QString(), this, 0.5);
	AmptekAllSpectrumClearControl_ = new AMPVControl("AmptekAllSpectrumClear", "amptek:sdd:all:spectrum:clear", "amptek:sdd:all:spectrum:clear", QString(), this, 0.5);
	AmptekAllPresetTimeControl_ = new AMPVControl("AmptekAllPresetTime", "amptek:sdd:all:spectrum:dwellTime", "amptek:sdd:all:spectrum:dwellTime", QString(), this, 0.1);
	AmptekAllSpectrumStateControl_ = new AMPVControl("AmptekAllSpectrumState", "amptek:sdd:all:spectrum:dwellState", "amptek:sdd:all:spectrum:dwellState", QString(), this, 0.5);

	AmptekSDD1IsAvailableControl_ = new AMReadOnlyPVControl("AmptekSDD1IsAvailable", "amptek:sdd1:spectrum:isAvailable", this);
	AmptekSDD2IsAvailableControl_ = new AMReadOnlyPVControl("AmptekSDD2IsAvailable", "amptek:sdd2:spectrum:isAvailable", this);
	AmptekSDD3IsAvailableControl_ = new AMReadOnlyPVControl("AmptekSDD3IsAvailable", "amptek:sdd3:spectrum:isAvailable", this);
	AmptekSDD4IsAvailableControl_ = new AMReadOnlyPVControl("AmptekSDD4IsAvailable", "amptek:sdd4:spectrum:isAvailable", this);
	AmptekSDD5IsAvailableControl_ = new AMReadOnlyPVControl("AmptekSDD5IsAvailable", "amptek:sdd5:spectrum:isAvailable", this);
	AmptekSDD1IsAvailableControl_->setTolerance(0.5);
	AmptekSDD2IsAvailableControl_->setTolerance(0.5);
	AmptekSDD3IsAvailableControl_->setTolerance(0.5);
	AmptekSDD4IsAvailableControl_->setTolerance(0.5);
	AmptekSDD5IsAvailableControl_->setTolerance(0.5);
	AmptekSDD1IsRequestedControl_ = new AMReadOnlyPVControl("AmptekSDD1IsRequested", "amptek:sdd1:spectrum:isRequested", this);
	AmptekSDD2IsRequestedControl_ = new AMReadOnlyPVControl("AmptekSDD2IsRequested", "amptek:sdd2:spectrum:isRequested", this);
	AmptekSDD3IsRequestedControl_ = new AMReadOnlyPVControl("AmptekSDD3IsRequested", "amptek:sdd3:spectrum:isRequested", this);
	AmptekSDD4IsRequestedControl_ = new AMReadOnlyPVControl("AmptekSDD4IsRequested", "amptek:sdd4:spectrum:isRequested", this);
	AmptekSDD5IsRequestedControl_ = new AMReadOnlyPVControl("AmptekSDD5IsRequested", "amptek:sdd5:spectrum:isRequested", this);
	AmptekSDD1IsRequestedControl_->setTolerance(0.5);
	AmptekSDD2IsRequestedControl_->setTolerance(0.5);
	AmptekSDD3IsRequestedControl_->setTolerance(0.5);
	AmptekSDD4IsRequestedControl_->setTolerance(0.5);
	AmptekSDD5IsRequestedControl_->setTolerance(0.5);
	AmptekSDD1SpectrumStateControl_ = new AMReadOnlyPVControl("AmptekSDD1SpectrumState", "amptek:sdd1:spectrum:dwellState", this);
	AmptekSDD2SpectrumStateControl_ = new AMReadOnlyPVControl("AmptekSDD2SpectrumState", "amptek:sdd2:spectrum:dwellState", this);
	AmptekSDD3SpectrumStateControl_ = new AMReadOnlyPVControl("AmptekSDD3SpectrumState", "amptek:sdd3:spectrum:dwellState", this);
	AmptekSDD4SpectrumStateControl_ = new AMReadOnlyPVControl("AmptekSDD4SpectrumState", "amptek:sdd4:spectrum:dwellState", this);
	AmptekSDD5SpectrumStateControl_ = new AMReadOnlyPVControl("AmptekSDD5SpectrumState", "amptek:sdd5:spectrum:dwellState", this);
	AmptekSDD1SpectrumStateControl_->setTolerance(0.5);
	AmptekSDD2SpectrumStateControl_->setTolerance(0.5);
	AmptekSDD3SpectrumStateControl_->setTolerance(0.5);
	AmptekSDD4SpectrumStateControl_->setTolerance(0.5);
	AmptekSDD5SpectrumStateControl_->setTolerance(0.5);

	AmptekSDD1StartAcquisitionControl_ = new AMPVControl("AmptekSDD1StartAcquisition", "amptek:sdd1:spectrum:start", "amptek:sdd1:spectrum:start", QString(), this, 0.5);
	AmptekSDD2StartAcquisitionControl_ = new AMPVControl("AmptekSDD2StartAcquisition", "amptek:sdd2:spectrum:start", "amptek:sdd2:spectrum:start", QString(), this, 0.5);
	AmptekSDD3StartAcquisitionControl_ = new AMPVControl("AmptekSDD3StartAcquisition", "amptek:sdd3:spectrum:start", "amptek:sdd3:spectrum:start", QString(), this, 0.5);
	AmptekSDD4StartAcquisitionControl_ = new AMPVControl("AmptekSDD4StartAcquisition", "amptek:sdd4:spectrum:start", "amptek:sdd4:spectrum:start", QString(), this, 0.5);
	AmptekSDD5StartAcquisitionControl_ = new AMPVControl("AmptekSDD5StartAcquisition", "amptek:sdd5:spectrum:start", "amptek:sdd5:spectrum:start", QString(), this, 0.5);

	AmptekSDD1SpectrumClearControl_ = new AMPVControl("AmptekSDD1SpectrumClear", "amptek:sdd1:spectrum:clear", "amptek:sdd1:spectrum:clear", QString(), this, 0.5);
	AmptekSDD2SpectrumClearControl_ = new AMPVControl("AmptekSDD2SpectrumClear", "amptek:sdd2:spectrum:clear", "amptek:sdd2:spectrum:clear", QString(), this, 0.5);
	AmptekSDD3SpectrumClearControl_ = new AMPVControl("AmptekSDD3SpectrumClear", "amptek:sdd3:spectrum:clear", "amptek:sdd3:spectrum:clear", QString(), this, 0.5);
	AmptekSDD4SpectrumClearControl_ = new AMPVControl("AmptekSDD4SpectrumClear", "amptek:sdd4:spectrum:clear", "amptek:sdd4:spectrum:clear", QString(), this, 0.5);
	AmptekSDD5SpectrumClearControl_ = new AMPVControl("AmptekSDD5SpectrumClear", "amptek:sdd5:spectrum:clear", "amptek:sdd5:spectrum:clear", QString(), this, 0.5);

	AmptekSDD1PresetTimeControl_ = new AMPVControl("AmptekSDD1PresetTime", "amptek:sdd1:spectrum:dwellTime", "amptek:sdd1:spectrum:dwellTime", QString(), this, 0.1);
	AmptekSDD2PresetTimeControl_ = new AMPVControl("AmptekSDD2PresetTime", "amptek:sdd2:spectrum:dwellTime", "amptek:sdd2:spectrum:dwellTime", QString(), this, 0.1);
	AmptekSDD3PresetTimeControl_ = new AMPVControl("AmptekSDD3PresetTime", "amptek:sdd3:spectrum:dwellTime", "amptek:sdd3:spectrum:dwellTime", QString(), this, 0.1);
	AmptekSDD4PresetTimeControl_ = new AMPVControl("AmptekSDD4PresetTime", "amptek:sdd4:spectrum:dwellTime", "amptek:sdd4:spectrum:dwellTime", QString(), this, 0.1);
	AmptekSDD5PresetTimeControl_ = new AMPVControl("AmptekSDD5PresetTime", "amptek:sdd5:spectrum:dwellTime", "amptek:sdd5:spectrum:dwellTime", QString(), this, 0.1);


	allAmptekControls_ = new AMControlSet(this);
	allAmptekControls_->addControl(AmptekAllStartAcquisitionControl_);
	allAmptekControls_->addControl(AmptekAllSpectrumClearControl_);
	allAmptekControls_->addControl(AmptekAllPresetTimeControl_);
	allAmptekControls_->addControl(AmptekAllSpectrumStateControl_);
	allAmptekControls_->addControl(AmptekSDD1IsAvailableControl_);
	allAmptekControls_->addControl(AmptekSDD2IsAvailableControl_);
	allAmptekControls_->addControl(AmptekSDD3IsAvailableControl_);
	allAmptekControls_->addControl(AmptekSDD4IsAvailableControl_);
	allAmptekControls_->addControl(AmptekSDD5IsAvailableControl_);
	allAmptekControls_->addControl(AmptekSDD1IsRequestedControl_);
	allAmptekControls_->addControl(AmptekSDD2IsRequestedControl_);
	allAmptekControls_->addControl(AmptekSDD3IsRequestedControl_);
	allAmptekControls_->addControl(AmptekSDD4IsRequestedControl_);
	allAmptekControls_->addControl(AmptekSDD5IsRequestedControl_);
	allAmptekControls_->addControl(AmptekSDD1SpectrumStateControl_);
	allAmptekControls_->addControl(AmptekSDD2SpectrumStateControl_);
	allAmptekControls_->addControl(AmptekSDD3SpectrumStateControl_);
	allAmptekControls_->addControl(AmptekSDD4SpectrumStateControl_);
	allAmptekControls_->addControl(AmptekSDD5SpectrumStateControl_);
	allAmptekControls_->addControl(AmptekSDD1StartAcquisitionControl_);
	allAmptekControls_->addControl(AmptekSDD2StartAcquisitionControl_);
	allAmptekControls_->addControl(AmptekSDD3StartAcquisitionControl_);
	allAmptekControls_->addControl(AmptekSDD4StartAcquisitionControl_);
	allAmptekControls_->addControl(AmptekSDD5StartAcquisitionControl_);
	allAmptekControls_->addControl(AmptekSDD1SpectrumClearControl_);
	allAmptekControls_->addControl(AmptekSDD2SpectrumClearControl_);
	allAmptekControls_->addControl(AmptekSDD3SpectrumClearControl_);
	allAmptekControls_->addControl(AmptekSDD4SpectrumClearControl_);
	allAmptekControls_->addControl(AmptekSDD5SpectrumClearControl_);
	allAmptekControls_->addControl(AmptekSDD1PresetTimeControl_);
	allAmptekControls_->addControl(AmptekSDD2PresetTimeControl_);
	allAmptekControls_->addControl(AmptekSDD3PresetTimeControl_);
	allAmptekControls_->addControl(AmptekSDD4PresetTimeControl_);
	allAmptekControls_->addControl(AmptekSDD5PresetTimeControl_);

	connect(AmptekAllStartAcquisitionControl_, SIGNAL(valueChanged(double)), this, SLOT(onAmptekAllStartAcquisitionControlChanged(double)));
	connect(AmptekAllSpectrumClearControl_, SIGNAL(valueChanged(double)), this, SLOT(onAmptekAllSpectrumClearControlChanged(double)));
	connect(AmptekAllPresetTimeControl_, SIGNAL(valueChanged(double)), this, SLOT(onAmptekAllPresetTimeControlChanged(double)));

	connect(AmptekSDD1SpectrumStateControl_, SIGNAL(valueChanged(double)), this, SLOT(onAmptekSpectrumStateControlsChanged(double)));
	connect(AmptekSDD2SpectrumStateControl_, SIGNAL(valueChanged(double)), this, SLOT(onAmptekSpectrumStateControlsChanged(double)));
	connect(AmptekSDD3SpectrumStateControl_, SIGNAL(valueChanged(double)), this, SLOT(onAmptekSpectrumStateControlsChanged(double)));
	connect(AmptekSDD4SpectrumStateControl_, SIGNAL(valueChanged(double)), this, SLOT(onAmptekSpectrumStateControlsChanged(double)));
	connect(AmptekSDD5SpectrumStateControl_, SIGNAL(valueChanged(double)), this, SLOT(onAmptekSpectrumStateControlsChanged(double)));

	connect(allAmptekControls_, SIGNAL(connected(bool)), this, SLOT(onAllAmptekControlsConnected(bool)));
}

void SGMAmptekCoordinator::onAllAmptekControlsConnected(bool connected){
	if(connected){
		amptekConnectedOnce_ = true;
		qDebug() << "Have all amptek controls";
	}
}

void SGMAmptekCoordinator::onAmptekAllStartAcquisitionControlChanged(double startAcquisition){
	Q_UNUSED(startAcquisition)
	if(!amptekConnectedOnce_)
		return;

	qDebug() << "Want to take master start to " << AmptekAllStartAcquisitionControl_->value();
	if(AmptekAllStartAcquisitionControl_->withinTolerance(0.0))
		return;

	if(AmptekSDD1IsAvailableControl_->withinTolerance(1) && AmptekSDD1IsRequestedControl_->withinTolerance(1)){
		qDebug() << "Starting SDD1 acquisition";
		AmptekSDD1StartAcquisitionControl_->move(1);
	}
	if(AmptekSDD2IsAvailableControl_->withinTolerance(1) && AmptekSDD2IsRequestedControl_->withinTolerance(1)){
		qDebug() << "Starting SDD2 acquisition";
		AmptekSDD2StartAcquisitionControl_->move(1);
	}
	if(AmptekSDD3IsAvailableControl_->withinTolerance(1) && AmptekSDD3IsRequestedControl_->withinTolerance(1)){
		qDebug() << "Starting SDD3 acquisition";
		AmptekSDD3StartAcquisitionControl_->move(1);
	}
	if(AmptekSDD4IsAvailableControl_->withinTolerance(1) && AmptekSDD4IsRequestedControl_->withinTolerance(1)){
		qDebug() << "Starting SDD4 acquisition";
		AmptekSDD4StartAcquisitionControl_->move(1);
	}
	if(AmptekSDD5IsAvailableControl_->withinTolerance(1) && AmptekSDD5IsRequestedControl_->withinTolerance(1)){
		qDebug() << "Starting SDD5 acquisition";
		AmptekSDD5StartAcquisitionControl_->move(1);
	}
}

void SGMAmptekCoordinator::onAmptekAllSpectrumClearControlChanged(double spectrumClear){
	Q_UNUSED(spectrumClear)
	if(!amptekConnectedOnce_)
		return;

	if(AmptekAllSpectrumClearControl_->withinTolerance(0.0))
		return;

	if(AmptekSDD1IsAvailableControl_->withinTolerance(1)){
		qDebug() << "Clearing SDD1 spectrum";
		AmptekSDD1SpectrumClearControl_->move(1);
	}
	if(AmptekSDD2IsAvailableControl_->withinTolerance(1)){
		qDebug() << "Clearing SDD2 spectrum";
		AmptekSDD2SpectrumClearControl_->move(1);
	}
	if(AmptekSDD3IsAvailableControl_->withinTolerance(1)){
		qDebug() << "Clearing SDD3 spectrum";
		AmptekSDD3SpectrumClearControl_->move(1);
	}
	if(AmptekSDD4IsAvailableControl_->withinTolerance(1)){
		qDebug() << "Clearing SDD4 spectrum";
		AmptekSDD4SpectrumClearControl_->move(1);
	}
	if(AmptekSDD5IsAvailableControl_->withinTolerance(1)){
		qDebug() << "Clearing SDD5 spectrum";
		AmptekSDD5SpectrumClearControl_->move(1);
	}
}

void SGMAmptekCoordinator::onAmptekAllPresetTimeControlChanged(double presetTime){
	if(!amptekConnectedOnce_)
		return;

	if(AmptekAllPresetTimeControl_->withinTolerance(-2.0)){
		qDebug() << "Detected preset time startup value, do not set";
		return;
	}

	if(AmptekSDD1IsAvailableControl_->withinTolerance(1) && !AmptekSDD1PresetTimeControl_->withinTolerance(presetTime)){
		qDebug() << "Changing SDD1 Preset Time to " << presetTime;
		AmptekSDD1PresetTimeControl_->move(presetTime);
	}
	if(AmptekSDD2IsAvailableControl_->withinTolerance(1) && !AmptekSDD2PresetTimeControl_->withinTolerance(presetTime)){
		qDebug() << "Changing SDD2 Preset Time to " << presetTime;
		AmptekSDD2PresetTimeControl_->move(presetTime);
	}
	if(AmptekSDD3IsAvailableControl_->withinTolerance(1) && !AmptekSDD3PresetTimeControl_->withinTolerance(presetTime)){
		qDebug() << "Changing SDD3 Preset Time to " << presetTime;
		AmptekSDD3PresetTimeControl_->move(presetTime);
	}
	if(AmptekSDD4IsAvailableControl_->withinTolerance(1) && !AmptekSDD4PresetTimeControl_->withinTolerance(presetTime)){
		qDebug() << "Changing SDD4 Preset Time to " << presetTime;
		AmptekSDD4PresetTimeControl_->move(presetTime);
	}
	if(AmptekSDD5IsAvailableControl_->withinTolerance(1) && !AmptekSDD5PresetTimeControl_->withinTolerance(presetTime)){
		qDebug() << "Changing SDD5 Preset Time to " << presetTime;
		AmptekSDD5PresetTimeControl_->move(presetTime);
	}
}

void SGMAmptekCoordinator::onAmptekSpectrumStateControlsChanged(double spectrumState){
	Q_UNUSED(spectrumState)
	if(!amptekConnectedOnce_)
		return;

	bool isActive = false;

	if(AmptekSDD1SpectrumStateControl_->withinTolerance(1) && AmptekSDD1IsAvailableControl_->withinTolerance(1) && AmptekSDD1IsRequestedControl_->withinTolerance(1))
		isActive = true;

	if(AmptekSDD2SpectrumStateControl_->withinTolerance(1) && AmptekSDD2IsAvailableControl_->withinTolerance(1) && AmptekSDD2IsRequestedControl_->withinTolerance(1))
		isActive = true;

	if(AmptekSDD3SpectrumStateControl_->withinTolerance(1) && AmptekSDD3IsAvailableControl_->withinTolerance(1) && AmptekSDD3IsRequestedControl_->withinTolerance(1))
		isActive = true;

	if(AmptekSDD4SpectrumStateControl_->withinTolerance(1) && AmptekSDD4IsAvailableControl_->withinTolerance(1) && AmptekSDD4IsRequestedControl_->withinTolerance(1))
		isActive = true;

	if(AmptekSDD5SpectrumStateControl_->withinTolerance(1) && AmptekSDD5IsAvailableControl_->withinTolerance(1) && AmptekSDD5IsRequestedControl_->withinTolerance(1))
		isActive = true;

	if(isActive)
		AmptekAllSpectrumStateControl_->move(1);
	else
		AmptekAllSpectrumStateControl_->move(0);
}
