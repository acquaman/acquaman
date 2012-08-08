#include "SGMAmptekCoordinator.h"

#include "beamline/AMPVControl.h"

SGMAmptekCoordinator::SGMAmptekCoordinator(QObject *parent) :
	QObject(parent)
{
	amptekConnectedOnce_ = false;

	AmptekAllInitializeControl_ = new AMPVControl("AmptekAllInitialize", "amptek:sdd:all:initialize", "amptek:sdd:all:initialize", QString(), this, 0.5);
	AmptekAllStartAcquisitionControl_ = new AMPVControl("AmptekAllStartAcquisition", "amptek:sdd:all:spectrum:startAcquisition", "amptek:sdd:all:spectrum:startAcquisition", QString(), this, 0.5);
	AmptekAllSpectrumClearControl_ = new AMPVControl("AmptekAllSpectrumClear", "amptek:sdd:all:spectrum:clear", "amptek:sdd:all:spectrum:clear", QString(), this, 0.5);
	AmptekAllPresetTimeControl_ = new AMPVControl("AmptekAllPresetTime", "amptek:sdd:all:parameters:PresetTime", "amptek:sdd:all:parameters:PresetTime", QString(), this, 0.1);

	AmptekSDD1IsAvailableControl_ = new AMReadOnlyPVControl("AmptekSDD1IsAvailable", "amptek:sdd1:isAvailable", this);
	AmptekSDD2IsAvailableControl_ = new AMReadOnlyPVControl("AmptekSDD2IsAvailable", "amptek:sdd2:isAvailable", this);
	AmptekSDD3IsAvailableControl_ = new AMReadOnlyPVControl("AmptekSDD3IsAvailable", "amptek:sdd3:isAvailable", this);
	AmptekSDD4IsAvailableControl_ = new AMReadOnlyPVControl("AmptekSDD4IsAvailable", "amptek:sdd4:isAvailable", this);
	AmptekSDD1IsAvailableControl_->setTolerance(0.5);
	AmptekSDD2IsAvailableControl_->setTolerance(0.5);
	AmptekSDD3IsAvailableControl_->setTolerance(0.5);
	AmptekSDD4IsAvailableControl_->setTolerance(0.5);

	AmptekSDD1InitializeControl_ = new AMPVControl("AmptekSDD1Initialize", "amptek:sdd1:initialize", "amptek:sdd1:initialize", QString(), this, 0.5);
	AmptekSDD2InitializeControl_ = new AMPVControl("AmptekSDD2Initialize", "amptek:sdd2:initialize", "amptek:sdd2:initialize", QString(), this, 0.5);
	AmptekSDD3InitializeControl_ = new AMPVControl("AmptekSDD3Initialize", "amptek:sdd3:initialize", "amptek:sdd3:initialize", QString(), this, 0.5);
	AmptekSDD4InitializeControl_ = new AMPVControl("AmptekSDD4Initialize", "amptek:sdd4:initialize", "amptek:sdd4:initialize", QString(), this, 0.5);

	AmptekSDD1StartAcquisitionControl_ = new AMPVControl("AmptekSDD1StartAcquisition", "amptek:sdd1:spectrum:startAcquisition", "amptek:sdd1:spectrum:startAcquisition", QString(), this, 0.5);
	AmptekSDD2StartAcquisitionControl_ = new AMPVControl("AmptekSDD2StartAcquisition", "amptek:sdd2:spectrum:startAcquisition", "amptek:sdd2:spectrum:startAcquisition", QString(), this, 0.5);
	AmptekSDD3StartAcquisitionControl_ = new AMPVControl("AmptekSDD3StartAcquisition", "amptek:sdd3:spectrum:startAcquisition", "amptek:sdd3:spectrum:startAcquisition", QString(), this, 0.5);
	AmptekSDD4StartAcquisitionControl_ = new AMPVControl("AmptekSDD4StartAcquisition", "amptek:sdd4:spectrum:startAcquisition", "amptek:sdd4:spectrum:startAcquisition", QString(), this, 0.5);

	AmptekSDD1SpectrumClearControl_ = new AMPVControl("AmptekSDD1SpectrumClear", "amptek:sdd1:spectrum:clear", "amptek:sdd1:spectrum:clear", QString(), this, 0.5);
	AmptekSDD2SpectrumClearControl_ = new AMPVControl("AmptekSDD2SpectrumClear", "amptek:sdd2:spectrum:clear", "amptek:sdd2:spectrum:clear", QString(), this, 0.5);
	AmptekSDD3SpectrumClearControl_ = new AMPVControl("AmptekSDD3SpectrumClear", "amptek:sdd3:spectrum:clear", "amptek:sdd3:spectrum:clear", QString(), this, 0.5);
	AmptekSDD4SpectrumClearControl_ = new AMPVControl("AmptekSDD4SpectrumClear", "amptek:sdd4:spectrum:clear", "amptek:sdd4:spectrum:clear", QString(), this, 0.5);

	AmptekSDD1PresetTimeControl_ = new AMPVControl("AmptekSDD1PresetTime", "amptek:sdd1:parameters:PresetTime", "amptek:sdd1:parameters:PresetTime", QString(), this, 0.1);
	AmptekSDD2PresetTimeControl_ = new AMPVControl("AmptekSDD2PresetTime", "amptek:sdd2:parameters:PresetTime", "amptek:sdd2:parameters:PresetTime", QString(), this, 0.1);
	AmptekSDD3PresetTimeControl_ = new AMPVControl("AmptekSDD3PresetTime", "amptek:sdd3:parameters:PresetTime", "amptek:sdd3:parameters:PresetTime", QString(), this, 0.1);
	AmptekSDD4PresetTimeControl_ = new AMPVControl("AmptekSDD4PresetTime", "amptek:sdd4:parameters:PresetTime", "amptek:sdd4:parameters:PresetTime", QString(), this, 0.1);


	allAmptekControls_ = new AMControlSet(this);
	allAmptekControls_->addControl(AmptekAllInitializeControl_);
	allAmptekControls_->addControl(AmptekAllStartAcquisitionControl_);
	allAmptekControls_->addControl(AmptekAllSpectrumClearControl_);
	allAmptekControls_->addControl(AmptekAllPresetTimeControl_);
	allAmptekControls_->addControl(AmptekSDD1IsAvailableControl_);
	allAmptekControls_->addControl(AmptekSDD2IsAvailableControl_);
	allAmptekControls_->addControl(AmptekSDD3IsAvailableControl_);
	allAmptekControls_->addControl(AmptekSDD4IsAvailableControl_);
	allAmptekControls_->addControl(AmptekSDD1InitializeControl_);
	allAmptekControls_->addControl(AmptekSDD2InitializeControl_);
	allAmptekControls_->addControl(AmptekSDD3InitializeControl_);
	allAmptekControls_->addControl(AmptekSDD4InitializeControl_);
	allAmptekControls_->addControl(AmptekSDD1StartAcquisitionControl_);
	allAmptekControls_->addControl(AmptekSDD2StartAcquisitionControl_);
	allAmptekControls_->addControl(AmptekSDD3StartAcquisitionControl_);
	allAmptekControls_->addControl(AmptekSDD4StartAcquisitionControl_);
	allAmptekControls_->addControl(AmptekSDD1SpectrumClearControl_);
	allAmptekControls_->addControl(AmptekSDD2SpectrumClearControl_);
	allAmptekControls_->addControl(AmptekSDD3SpectrumClearControl_);
	allAmptekControls_->addControl(AmptekSDD4SpectrumClearControl_);
	allAmptekControls_->addControl(AmptekSDD1PresetTimeControl_);
	allAmptekControls_->addControl(AmptekSDD2PresetTimeControl_);
	allAmptekControls_->addControl(AmptekSDD3PresetTimeControl_);
	allAmptekControls_->addControl(AmptekSDD4PresetTimeControl_);

	connect(AmptekAllInitializeControl_, SIGNAL(valueChanged(double)), this, SLOT(onAmptekAllInitializeControlChanged(double)));
	connect(AmptekAllStartAcquisitionControl_, SIGNAL(valueChanged(double)), this, SLOT(onAmptekAllStartAcquisitionControlChanged(double)));
	connect(AmptekAllSpectrumClearControl_, SIGNAL(valueChanged(double)), this, SLOT(onAmptekAllSpectrumClearControlChanged(double)));
	connect(AmptekAllPresetTimeControl_, SIGNAL(valueChanged(double)), this, SLOT(onAmptekAllPresetTimeControlChanged(double)));

	connect(allAmptekControls_, SIGNAL(connected(bool)), this, SLOT(onAllAmptekControlsConnected(bool)));
}

void SGMAmptekCoordinator::onAllAmptekControlsConnected(bool connected){
	if(connected){
		amptekConnectedOnce_ = true;
		qDebug() << "Have all amptek controls";
	}
}

void SGMAmptekCoordinator::onAmptekAllInitializeControlChanged(double initialize){
	Q_UNUSED(initialize)
	if(!amptekConnectedOnce_)
		return;

	if(AmptekAllInitializeControl_->withinTolerance(0.0))
		return;

	if(AmptekSDD1IsAvailableControl_->withinTolerance(1)){
		qDebug() << "Initializing SDD1";
		AmptekSDD1InitializeControl_->move(1);
	}
	if(AmptekSDD2IsAvailableControl_->withinTolerance(1)){
		qDebug() << "Initializing SDD2";
		AmptekSDD2InitializeControl_->move(1);
	}
	if(AmptekSDD3IsAvailableControl_->withinTolerance(1)){
		qDebug() << "Initializing SDD3";
		AmptekSDD3InitializeControl_->move(1);
	}
	if(AmptekSDD4IsAvailableControl_->withinTolerance(1)){
		qDebug() << "Initializing SDD4";
		AmptekSDD4InitializeControl_->move(1);
	}
}

void SGMAmptekCoordinator::onAmptekAllStartAcquisitionControlChanged(double startAcquisition){
	Q_UNUSED(startAcquisition)
	if(!amptekConnectedOnce_)
		return;

	if(AmptekAllStartAcquisitionControl_->withinTolerance(0.0))
		return;

	if(AmptekSDD1IsAvailableControl_->withinTolerance(1)){
		qDebug() << "Starting SDD1 acquisition";
		AmptekSDD1StartAcquisitionControl_->move(1);
	}
	if(AmptekSDD2IsAvailableControl_->withinTolerance(1)){
		qDebug() << "Starting SDD2 acquisition";
		AmptekSDD2StartAcquisitionControl_->move(1);
	}
	if(AmptekSDD3IsAvailableControl_->withinTolerance(1)){
		qDebug() << "Starting SDD3 acquisition";
		AmptekSDD3StartAcquisitionControl_->move(1);
	}
	if(AmptekSDD4IsAvailableControl_->withinTolerance(1)){
		qDebug() << "Starting SDD4 acquisition";
		AmptekSDD4StartAcquisitionControl_->move(1);
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
}
