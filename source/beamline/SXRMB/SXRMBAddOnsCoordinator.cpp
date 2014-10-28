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

	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onAllControlsConnected(bool)));

	connect(oldEnergy_, SIGNAL(valueChanged(double)), this, SLOT(onOldEnergyValueChanged(double)));
	connect(oldEnergyFeedback_, SIGNAL(valueChanged(double)), this, SLOT(onOldEnergyFeedbackValueChanged(double)));
	connect(oldEnergyStatus_, SIGNAL(valueChanged(double)), this, SLOT(onOldEnergyStatusValueChanged(double)));

	connect(addOnsEnergy_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsEnergyValueChanged(double)));
	connect(addOnsEnergyStop_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsEnergyStopValueChanged(double)));
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
