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


#include "SXRMBAddOnsCoordinator.h"

#include "beamline/AMPVControl.h"

SXRMBAddOnsCoordinator::SXRMBAddOnsCoordinator(QObject *parent) :
	QObject(parent)
{
	connectedOnce_ = false;

	oldEnergy_ =  new AMSinglePVControl("OldEnergy", "BL1606-B1-1:Energy", this, 0.01);
	oldEnergyFeedback_ = new AMReadOnlyPVControl("OldEnergyFeedback", "BL1606-B1-1:Energy:fbk", this);
	oldEnergyStatus_ = new AMReadOnlyPVControl("OldEnergyStatus", "BL1606-B1-1:Energy:status", this);

	monoZ2StopControl_ = new AMSinglePVControl("MonoZ2Stop", "SMTR1606-4-B10-11:stop", this, 0.5);
	monoY2StopControl_ = new AMSinglePVControl("MonoY2Stop", "SMTR1606-4-B10-12:stop", this, 0.5);
	monoThetaStopControl_ = new AMSinglePVControl("MonoThetaStop", "SMTR1606-4-B10-16:stop", this, 0.5);

	addOnsEnergy_ = new AMSinglePVControl("AddOnsEnergy", "BL1606-B1-1:AddOns:Energy", this, 0.001);
	addOnsEnergyFeedback_ = new AMSinglePVControl("AddOnsEnergyFeedback", "BL1606-B1-1:AddOns:Energy:fbk", this, 0.001);
	addOnsEnergyStatus_ = new AMSinglePVControl("AddOnsEnergyStatus", "BL1606-B1-1:AddOns:Energy:status", this, 0.5);
	addOnsEnergyStop_ = new AMSinglePVControl("AddOnsEnergyStop", "BL1606-B1-1:AddOns:Energy:stop", this, 0.5);

	sxrmbMicroprobeSampleStageX_ = new AMSinglePVControl("SXRMBMicroprobleSampleStageX", "SVM1606-5-B10-07:mm", this, 0.0001);
	sxrmbMicroprobeSampleStageY_ = new AMSinglePVControl("SXRMBMicroprobleSampleStageY", "SVM1606-5-B10-08:mm", this, 0.0001);
	sxrmbMicroprobeSampleStageZ_ = new AMSinglePVControl("SXRMBMicroprobleSampleStageZ", "SVM1606-5-B10-09:mm", this, 0.0001);

	addOnsMicroprobeSampleStageX_ = new AMSinglePVControl("SXRMBMicroprobleSampleStageFeedbackX", "SVM1606-5-B10-07:mm:sp", this, 0.0001);
	addOnsMicroprobeSampleStageY_ = new AMSinglePVControl("SXRMBMicroprobleSampleStageFeedbackY", "SVM1606-5-B10-08:mm:sp", this, 0.0001);
	addOnsMicroprobeSampleStageZ_ = new AMSinglePVControl("SXRMBMicroprobleSampleStageFeedbackZ", "SVM1606-5-B10-09:mm:sp", this, 0.0001);

	sxrmbMicroprobeSampleStageFeedbackX_ = new AMSinglePVControl("SXRMBMicroprobleSampleStageStatusX", "SVM1606-5-B10-07:mm:status", this, 0.5);
	sxrmbMicroprobeSampleStageFeedbackY_ = new AMSinglePVControl("SXRMBMicroprobleSampleStageStatusY", "SVM1606-5-B10-08:mm:status", this, 0.5);
	sxrmbMicroprobeSampleStageFeedbackZ_ = new AMSinglePVControl("SXRMBMicroprobleSampleStageStatusZ", "SVM1606-5-B10-09:mm:status", this, 0.5);

	addOnsMicroprobeSampleStageFeedbackX_ = new AMSinglePVControl("AddOnsMicroprobleSampleStageX", "BL1606-B1-1:AddOns:uProbe:SampleStage:X:mm", this, 0.0001);
	addOnsMicroprobeSampleStageFeedbackY_ = new AMSinglePVControl("AddOnsMicroprobleSampleStageY", "BL1606-B1-1:AddOns:uProbe:SampleStage:Y:mm", this, 0.0001);
	addOnsMicroprobeSampleStageFeedbackZ_ = new AMSinglePVControl("AddOnsMicroprobleSampleStageZ", "BL1606-B1-1:AddOns:uProbe:SampleStage:Z:mm", this, 0.0001);

	sxrmbMicroprobeSampleStageStatusX_ = new AMSinglePVControl("AddOnsMicroprobleSampleStageFeedbackX", "BL1606-B1-1:AddOns:uProbe:SampleStage:X:mm:fbk", this, 0.0001);
	sxrmbMicroprobeSampleStageStatusY_ = new AMSinglePVControl("AddOnsMicroprobleSampleStageFeedbackY", "BL1606-B1-1:AddOns:uProbe:SampleStage:Y:mm:fbk", this, 0.0001);
	sxrmbMicroprobeSampleStageStatusZ_ = new AMSinglePVControl("AddOnsMicroprobleSampleStageFeedbackZ", "BL1606-B1-1:AddOns:uProbe:SampleStage:Z:mm:fbk", this, 0.0001);

	addOnsMicroprobeSampleStageStatusX_ = new AMSinglePVControl("AddOnsMicroprobleSampleStageStatusX", "BL1606-B1-1:AddOns:uProbe:SampleStage:X:mm:status", this, 0.5);
	addOnsMicroprobeSampleStageStatusY_ = new AMSinglePVControl("AddOnsMicroprobleSampleStageStatusY", "BL1606-B1-1:AddOns:uProbe:SampleStage:Y:mm:status", this, 0.5);
	addOnsMicroprobeSampleStageStatusZ_ = new AMSinglePVControl("AddOnsMicroprobleSampleStageStatusZ", "BL1606-B1-1:AddOns:uProbe:SampleStage:Z:mm:status", this, 0.5);

	allControls_ = new AMControlSet(this);
	allControls_->addControl(oldEnergy_);
	allControls_->addControl(oldEnergyFeedback_);
	allControls_->addControl(oldEnergyStatus_);
	allControls_->addControl(monoZ2StopControl_);
	allControls_->addControl(monoY2StopControl_);
	allControls_->addControl(monoThetaStopControl_);
	allControls_->addControl(addOnsEnergy_);
	allControls_->addControl(addOnsEnergyFeedback_);
	allControls_->addControl(addOnsEnergyStatus_);
	allControls_->addControl(addOnsEnergyStop_);
	allControls_->addControl(sxrmbMicroprobeSampleStageX_);
	allControls_->addControl(sxrmbMicroprobeSampleStageY_);
	allControls_->addControl(sxrmbMicroprobeSampleStageZ_);
	allControls_->addControl(sxrmbMicroprobeSampleStageFeedbackX_);
	allControls_->addControl(sxrmbMicroprobeSampleStageFeedbackY_);
	allControls_->addControl(sxrmbMicroprobeSampleStageFeedbackZ_);
	allControls_->addControl(sxrmbMicroprobeSampleStageStatusX_);
	allControls_->addControl(sxrmbMicroprobeSampleStageStatusY_);
	allControls_->addControl(sxrmbMicroprobeSampleStageStatusZ_);
	allControls_->addControl(addOnsMicroprobeSampleStageX_);
	allControls_->addControl(addOnsMicroprobeSampleStageY_);
	allControls_->addControl(addOnsMicroprobeSampleStageZ_);
	allControls_->addControl(addOnsMicroprobeSampleStageFeedbackX_);
	allControls_->addControl(addOnsMicroprobeSampleStageFeedbackY_);
	allControls_->addControl(addOnsMicroprobeSampleStageFeedbackZ_);
	allControls_->addControl(addOnsMicroprobeSampleStageStatusX_);
	allControls_->addControl(addOnsMicroprobeSampleStageStatusY_);
	allControls_->addControl(addOnsMicroprobeSampleStageStatusZ_);

	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onAllControlsConnected(bool)));

	connect(oldEnergy_, SIGNAL(valueChanged(double)), this, SLOT(onOldEnergyValueChanged(double)));
	connect(oldEnergyFeedback_, SIGNAL(valueChanged(double)), this, SLOT(onOldEnergyFeedbackValueChanged(double)));
	connect(oldEnergyStatus_, SIGNAL(valueChanged(double)), this, SLOT(onOldEnergyStatusValueChanged(double)));

	connect(addOnsEnergy_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsEnergyValueChanged(double)));
	connect(addOnsEnergyStop_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsEnergyStopValueChanged(double)));

	connect(sxrmbMicroprobeSampleStageX_, SIGNAL(valueChanged(double)), this, SLOT(onSXRMBMicroprobeSampleStageXValueChanged(double)));
	connect(sxrmbMicroprobeSampleStageY_, SIGNAL(valueChanged(double)), this, SLOT(onSXRMBMicroprobeSampleStageYValueChanged(double)));
	connect(sxrmbMicroprobeSampleStageZ_, SIGNAL(valueChanged(double)), this, SLOT(onSXRMBMicroprobeSampleStageZValueChanged(double)));

	connect(sxrmbMicroprobeSampleStageFeedbackX_, SIGNAL(valueChanged(double)), this, SLOT(onSXRMBMicroprobeSampleStageFeedbackXValueChanged(double)));
	connect(sxrmbMicroprobeSampleStageFeedbackY_, SIGNAL(valueChanged(double)), this, SLOT(onSXRMBMicroprobeSampleStageFeedbackYValueChanged(double)));
	connect(sxrmbMicroprobeSampleStageFeedbackZ_, SIGNAL(valueChanged(double)), this, SLOT(onSXRMBMicroprobeSampleStageFeedbackZValueChanged(double)));

	connect(sxrmbMicroprobeSampleStageStatusX_, SIGNAL(valueChanged(double)), this, SLOT(onSXRMBMicroprobeSampleStageStatusXValueChanged(double)));
	connect(sxrmbMicroprobeSampleStageStatusY_, SIGNAL(valueChanged(double)), this, SLOT(onSXRMBMicroprobeSampleStageStatusYValueChanged(double)));
	connect(sxrmbMicroprobeSampleStageStatusZ_, SIGNAL(valueChanged(double)), this, SLOT(onSXRMBMicroprobeSampleStageStatusZValueChanged(double)));

	connect(addOnsMicroprobeSampleStageX_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsMicroprobeSampleStageXValueChanged(double)));
	connect(addOnsMicroprobeSampleStageY_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsMicroprobeSampleStageYValueChanged(double)));
	connect(addOnsMicroprobeSampleStageZ_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsMicroprobeSampleStageZValueChanged(double)));
}

SXRMBAddOnsCoordinator::~SXRMBAddOnsCoordinator(){}

void SXRMBAddOnsCoordinator::onAllControlsConnected(bool connected){
	if(connected){
		connectedOnce_ = true;

		qDebug() << "Checking start up value from the OLD energy as " << oldEnergy_->value();
		onOldEnergyValueChanged(oldEnergy_->value());
		qDebug() << "Checking start up value from the OLD energy feedback as " << oldEnergyFeedback_->value();
		onOldEnergyFeedbackValueChanged(oldEnergyFeedback_->value());
		qDebug() << "Checking start up value from the OLD energy status as " << oldEnergyStatus_->value();
		onOldEnergyStatusValueChanged(oldEnergyStatus_->value());
	}
}

void SXRMBAddOnsCoordinator::onOldEnergyValueChanged(double value){
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD energy move requested for " << oldEnergy_->value() << " versus " << addOnsEnergy_->value();

	if(!addOnsEnergy_->withinTolerance(oldEnergy_->value()))
		addOnsEnergy_->move(oldEnergy_->value());
}

void SXRMBAddOnsCoordinator::onOldEnergyFeedbackValueChanged(double value){
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD energy feedback move of " << oldEnergyFeedback_->value() << " versus " << addOnsEnergyFeedback_->value();

	if(!addOnsEnergyFeedback_->withinTolerance(oldEnergyFeedback_->value()))
		addOnsEnergyFeedback_->move(oldEnergyFeedback_->value());
}

void SXRMBAddOnsCoordinator::onOldEnergyStatusValueChanged(double value){
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD energy status change to " << oldEnergyStatus_->value() << " versus " << addOnsEnergyStatus_->value();
	if(!addOnsEnergyStatus_->withinTolerance(oldEnergyStatus_->value()))
		addOnsEnergyStatus_->move(oldEnergyStatus_->value());
}

void SXRMBAddOnsCoordinator::onAddOnsEnergyValueChanged(double value){
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	qDebug() << "Detected AddOns energy move requested for " << addOnsEnergy_->value() << " versus " << oldEnergy_->value() << " with OLD feedback " << oldEnergyFeedback_->value() << " and AddOns feedback " << addOnsEnergyFeedback_->value() << " with old status" << oldEnergyStatus_->value();

	// If we're requesting the same energy with AddOns as was in the old energy AND the old energy feedback matches the old energy setpoint AND the status is MOVE DONE
	// THEN issuing a move will not do anything SO we'll trick the status to say "MOVE ACTIVE" then "MOVE DONE"
	if(oldEnergy_->withinTolerance(addOnsEnergy_->value()) && oldEnergy_->withinTolerance(oldEnergyFeedback_->value()) && oldEnergyStatus_->withinTolerance(0.0)){
		qDebug() << "Faking an energy move";
		addOnsEnergyStatus_->move(1.0);
		QTimer::singleShot(100, this, SLOT(restoreEnergyStatus()));
		return;
	}

	// If we're requesting the same energy with AddOns as was in the old energy AND the old energy doesn't agree with the old energy feedback AND the status isn't moving
	// THEN we got genked somehow, and we're not going to move even though we really should SO we'll trick the old energy by moving to a value that's a fraction away from it's last value
	if(oldEnergy_->withinTolerance(addOnsEnergy_->value()) && !oldEnergy_->withinTolerance(oldEnergyFeedback_->value()) && !oldEnergyStatus_->withinTolerance(1.0)){
		qDebug() << "Having to do fractional delta move to get out of bad situation";
		oldEnergy_->move(oldEnergy_->value()+0.0000001);
		return;
	}

	if(!oldEnergy_->withinTolerance(addOnsEnergy_->value())){
		qDebug() << "Actually requesting an energy move in the regular way";
		oldEnergy_->move(addOnsEnergy_->value());
	}
}

void SXRMBAddOnsCoordinator::onAddOnsEnergyStopValueChanged(double value){
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	qDebug() << "A request to stop energy was heard from the addOnsEnergyStop";

	if(addOnsEnergyStop_->withinTolerance(1.0)){
		monoZ2StopControl_->move(1.0);
		monoY2StopControl_->move(1.0);
		monoThetaStopControl_->move(1.0);
	}
}

void SXRMBAddOnsCoordinator::restoreEnergyStatus(){
	qDebug() << "Restoring AddOns energy status to old energy status with AddOns " << addOnsEnergyStatus_->value() << " and old energy status " << oldEnergyStatus_->value();

	if(!addOnsEnergyStatus_->withinTolerance(oldEnergyStatus_->value()))
		addOnsEnergyStatus_->move(oldEnergyStatus_->value());
}


void SXRMBAddOnsCoordinator::onSXRMBMicroprobeSampleStageXValueChanged(double value){
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD microprobe stage X move of " << sxrmbMicroprobeSampleStageX_->value() << " versus " << addOnsMicroprobeSampleStageX_->value();

	// If we're requesting the same stage X with AddOns as was in the old stage X AND the old stage X feedback matches the old stage X setpoint AND the status is MOVE DONE
	// THEN issuing a move will not do anything SO we'll trick the status to say "MOVE ACTIVE" then "MOVE DONE"
	if(sxrmbMicroprobeSampleStageX_->withinTolerance(addOnsMicroprobeSampleStageX_->value()) && sxrmbMicroprobeSampleStageX_->withinTolerance(sxrmbMicroprobeSampleStageFeedbackX_->value()) && sxrmbMicroprobeSampleStageStatusX_->withinTolerance(0.0)){
		qDebug() << "Faking a sample stage X move";
		addOnsMicroprobeSampleStageStatusX_->move(1.0);
		QTimer::singleShot(100, this, SLOT(restoreAddOnsMicroprobeSampleStageStatusX()));
		return;
	}

	if(!addOnsMicroprobeSampleStageX_->withinTolerance(sxrmbMicroprobeSampleStageX_->value())){
		qDebug() << "Doing sample stage X move in the regular way";
		addOnsMicroprobeSampleStageX_->move(sxrmbMicroprobeSampleStageX_->value());
	}
}

void SXRMBAddOnsCoordinator::onSXRMBMicroprobeSampleStageYValueChanged(double value){
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD microprobe stage Y move of " << sxrmbMicroprobeSampleStageY_->value() << " versus " << addOnsMicroprobeSampleStageY_->value();

	// If we're requesting the same stage Y with AddOns as was in the old stage Y AND the old stage Y feedback matches the old stage Y setpoint AND the status is MOVE DONE
	// THEN issuing a move will not do anything SO we'll trick the status to say "MOVE ACTIVE" then "MOVE DONE"
	if(sxrmbMicroprobeSampleStageY_->withinTolerance(addOnsMicroprobeSampleStageY_->value()) && sxrmbMicroprobeSampleStageY_->withinTolerance(sxrmbMicroprobeSampleStageFeedbackY_->value()) && sxrmbMicroprobeSampleStageStatusY_->withinTolerance(0.0)){
		qDebug() << "Faking a sample stage Y move";
		addOnsMicroprobeSampleStageStatusY_->move(1.0);
		QTimer::singleShot(100, this, SLOT(restoreAddOnsMicroprobeSampleStageStatusY()));
		return;
	}

	if(!addOnsMicroprobeSampleStageY_->withinTolerance(sxrmbMicroprobeSampleStageY_->value())){
		qDebug() << "Doing sample stage Y move in the regular way";
		addOnsMicroprobeSampleStageY_->move(sxrmbMicroprobeSampleStageY_->value());
	}
}

void SXRMBAddOnsCoordinator::onSXRMBMicroprobeSampleStageZValueChanged(double value){
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD microprobe stage Z move of " << sxrmbMicroprobeSampleStageZ_->value() << " versus " << addOnsMicroprobeSampleStageZ_->value();

	// If we're requesting the same stage Z with AddOns as was in the old stage Z AND the old stage Z feedback matches the old stage Z setpoint AND the status is MOVE DONE
	// THEN issuing a move will not do anything SO we'll trick the status to say "MOVE ACTIVE" then "MOVE DONE"
	if(sxrmbMicroprobeSampleStageZ_->withinTolerance(addOnsMicroprobeSampleStageZ_->value()) && sxrmbMicroprobeSampleStageZ_->withinTolerance(sxrmbMicroprobeSampleStageFeedbackZ_->value()) && sxrmbMicroprobeSampleStageStatusZ_->withinTolerance(0.0)){
		qDebug() << "Faking a sample stage Z move";
		addOnsMicroprobeSampleStageStatusZ_->move(1.0);
		QTimer::singleShot(100, this, SLOT(restoreAddOnsMicroprobeSampleStageStatusZ()));
		return;
	}

	if(!addOnsMicroprobeSampleStageZ_->withinTolerance(sxrmbMicroprobeSampleStageZ_->value())){
		qDebug() << "Doing sample stage Z move in the regular way";
		addOnsMicroprobeSampleStageZ_->move(sxrmbMicroprobeSampleStageZ_->value());
	}
}

void SXRMBAddOnsCoordinator::onSXRMBMicroprobeSampleStageFeedbackXValueChanged(double value){
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD microprobe stage X feedback move of " << sxrmbMicroprobeSampleStageFeedbackX_->value() << " versus " << addOnsMicroprobeSampleStageFeedbackX_->value();

	if(!addOnsMicroprobeSampleStageFeedbackX_->withinTolerance(sxrmbMicroprobeSampleStageFeedbackX_->value()))
		addOnsMicroprobeSampleStageFeedbackX_->move(sxrmbMicroprobeSampleStageFeedbackX_->value());
}

void SXRMBAddOnsCoordinator::onSXRMBMicroprobeSampleStageFeedbackYValueChanged(double value){
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD microprobe stage Y feedback move of " << sxrmbMicroprobeSampleStageFeedbackY_->value() << " versus " << addOnsMicroprobeSampleStageFeedbackY_->value();

	if(!addOnsMicroprobeSampleStageFeedbackY_->withinTolerance(sxrmbMicroprobeSampleStageFeedbackY_->value()))
		addOnsMicroprobeSampleStageFeedbackY_->move(sxrmbMicroprobeSampleStageFeedbackY_->value());
}

void SXRMBAddOnsCoordinator::onSXRMBMicroprobeSampleStageFeedbackZValueChanged(double value){
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD microprobe stage Z feedback move of " << sxrmbMicroprobeSampleStageFeedbackZ_->value() << " versus " << addOnsMicroprobeSampleStageFeedbackZ_->value();

	if(!addOnsMicroprobeSampleStageFeedbackZ_->withinTolerance(sxrmbMicroprobeSampleStageFeedbackZ_->value()))
		addOnsMicroprobeSampleStageFeedbackZ_->move(sxrmbMicroprobeSampleStageFeedbackZ_->value());
}

void SXRMBAddOnsCoordinator::onSXRMBMicroprobeSampleStageStatusXValueChanged(double value){
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD microprobe stage X status move of " << sxrmbMicroprobeSampleStageStatusX_->value() << " versus " << addOnsMicroprobeSampleStageStatusX_->value();

	if(!addOnsMicroprobeSampleStageStatusX_->withinTolerance(sxrmbMicroprobeSampleStageStatusX_->value()))
		addOnsMicroprobeSampleStageStatusX_->move(sxrmbMicroprobeSampleStageStatusX_->value());
}

void SXRMBAddOnsCoordinator::onSXRMBMicroprobeSampleStageStatusYValueChanged(double value){
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD microprobe stage Y status move of " << sxrmbMicroprobeSampleStageStatusY_->value() << " versus " << addOnsMicroprobeSampleStageStatusY_->value();

	if(!addOnsMicroprobeSampleStageStatusY_->withinTolerance(sxrmbMicroprobeSampleStageStatusY_->value()))
		addOnsMicroprobeSampleStageStatusY_->move(sxrmbMicroprobeSampleStageStatusY_->value());
}

void SXRMBAddOnsCoordinator::onSXRMBMicroprobeSampleStageStatusZValueChanged(double value){
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD microprobe stage Z status move of " << sxrmbMicroprobeSampleStageStatusZ_->value() << " versus " << addOnsMicroprobeSampleStageStatusZ_->value();

	if(!addOnsMicroprobeSampleStageStatusZ_->withinTolerance(sxrmbMicroprobeSampleStageStatusZ_->value()))
		addOnsMicroprobeSampleStageStatusZ_->move(sxrmbMicroprobeSampleStageStatusZ_->value());
}

void SXRMBAddOnsCoordinator::onAddOnsMicroprobeSampleStageXValueChanged(double value){
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	qDebug() << "Detected new AddOns microprobe stage X move of " << addOnsMicroprobeSampleStageX_->value() << " versus " << sxrmbMicroprobeSampleStageX_->value();

	if(!sxrmbMicroprobeSampleStageX_->withinTolerance(addOnsMicroprobeSampleStageX_->value()))
		sxrmbMicroprobeSampleStageX_->move(addOnsMicroprobeSampleStageX_->value());
}

void SXRMBAddOnsCoordinator::onAddOnsMicroprobeSampleStageYValueChanged(double value){
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	qDebug() << "Detected new AddOns microprobe stage Y move of " << addOnsMicroprobeSampleStageY_->value() << " versus " << sxrmbMicroprobeSampleStageY_->value();

	if(!sxrmbMicroprobeSampleStageY_->withinTolerance(addOnsMicroprobeSampleStageY_->value()))
		sxrmbMicroprobeSampleStageY_->move(addOnsMicroprobeSampleStageY_->value());
}

void SXRMBAddOnsCoordinator::onAddOnsMicroprobeSampleStageZValueChanged(double value){
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	qDebug() << "Detected new AddOns microprobe stage Z move of " << addOnsMicroprobeSampleStageZ_->value() << " versus " << sxrmbMicroprobeSampleStageZ_->value();

	if(!sxrmbMicroprobeSampleStageZ_->withinTolerance(addOnsMicroprobeSampleStageZ_->value()))
		sxrmbMicroprobeSampleStageZ_->move(addOnsMicroprobeSampleStageZ_->value());
}

void SXRMBAddOnsCoordinator::restoreAddOnsMicroprobeSampleStageStatusX(){
	qDebug() << "Restoring AddOns sample stage X status to old SXRMB sample stage X status with AddOns " << addOnsMicroprobeSampleStageStatusX_->value() << " and old X status " << sxrmbMicroprobeSampleStageStatusX_->value();

	if(!addOnsMicroprobeSampleStageStatusX_->withinTolerance(sxrmbMicroprobeSampleStageStatusX_->value()))
		addOnsMicroprobeSampleStageStatusX_->move(sxrmbMicroprobeSampleStageStatusX_->value());
}

void SXRMBAddOnsCoordinator::restoreAddOnsMicroprobeSampleStageStatusY(){
	qDebug() << "Restoring AddOns sample stage Y status to old SXRMB sample stage Y status with AddOns " << addOnsMicroprobeSampleStageStatusY_->value() << " and old Y status " << sxrmbMicroprobeSampleStageStatusY_->value();

	if(!addOnsMicroprobeSampleStageStatusY_->withinTolerance(sxrmbMicroprobeSampleStageStatusY_->value()))
		addOnsMicroprobeSampleStageStatusY_->move(sxrmbMicroprobeSampleStageStatusY_->value());
}

void SXRMBAddOnsCoordinator::restoreAddOnsMicroprobeSampleStageStatusZ(){
	qDebug() << "Restoring AddOns sample stage Z status to old SXRMB sample stage Z status with AddOns " << addOnsMicroprobeSampleStageStatusZ_->value() << " and old Z status " << sxrmbMicroprobeSampleStageStatusZ_->value();

	if(!addOnsMicroprobeSampleStageStatusZ_->withinTolerance(sxrmbMicroprobeSampleStageStatusZ_->value()))
		addOnsMicroprobeSampleStageStatusZ_->move(sxrmbMicroprobeSampleStageStatusZ_->value());
}
