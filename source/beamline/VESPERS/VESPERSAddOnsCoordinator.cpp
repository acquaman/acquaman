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


#include "VESPERSAddOnsCoordinator.h"

#include "beamline/AMPVControl.h"

VESPERSAddOnsCoordinator::VESPERSAddOnsCoordinator(QObject *parent) :
	QObject(parent)
{
	connectedOnce_ = false;

	// Sample H, V, N

	oldSampleHSetpointControl_ = new AMSinglePVControl("oldSampleHSetpoint", "TS1607-2-B21-02:H:user:mm", this, 0.001);
	oldSampleHFeedbackControl_ = new AMReadOnlyPVControl("oldSampleHFeedback", "TS1607-2-B21-02:H:user:mm:fbk", this);

	oldSampleVSetpointControl_ = new AMSinglePVControl("oldSampleVSetpoint", "TS1607-2-B21-02:V:user:mm", this, 0.001);
	oldSampleVFeedbackControl_ = new AMReadOnlyPVControl("oldSampleVFeedback", "TS1607-2-B21-02:V:user:mm:fbk", this);

	oldSampleNSetpointControl_ = new AMSinglePVControl("oldSampleNSetpoint", "TS1607-2-B21-02:N:user:mm", this, 0.001);
	oldSampleNFeedbackControl_ = new AMReadOnlyPVControl("oldSampleNFeedback", "TS1607-2-B21-02:N:user:mm:fbk", this);

	oldRealSampleXStatusControl_ = new AMReadOnlyPVControl("oldRealSampleXFeedback", "SVM1607-2-B21-05:status", this);
	oldRealSampleYStatusControl_ = new AMReadOnlyPVControl("oldRealSampleYFeedback", "SVM1607-2-B21-06:status", this);
	oldRealSampleZStatusControl_ = new AMReadOnlyPVControl("oldRealSampleZFeedback", "SVM1607-2-B21-04:status", this);

	addOnsSampleHSetpointControl_ = new AMSinglePVControl("AddOnsSampleHSetpoint", "BL1607-B2-1:AddOns:SampleStage:H:mm", this, 0.001);
	addOnsSampleHFeedbackControl_ = new AMSinglePVControl("AddOnsSampleHFeedback", "BL1607-B2-1:AddOns:SampleStage:H:mm:fbk", this, 0.001);
	addOnsSampleHStatusControl_ = new AMSinglePVControl("AddOnsSampleHStatus", "BL1607-B2-1:AddOns:SampleStage:H:status", this, 0.5);

	addOnsSampleVSetpointControl_ = new AMSinglePVControl("AddOnsSampleVSetpoint", "BL1607-B2-1:AddOns:SampleStage:V:mm", this, 0.001);
	addOnsSampleVFeedbackControl_ = new AMSinglePVControl("AddOnsSampleVFeedback", "BL1607-B2-1:AddOns:SampleStage:V:mm:fbk", this, 0.001);
	addOnsSampleVStatusControl_ = new AMSinglePVControl("AddOnsSampleVStatus", "BL1607-B2-1:AddOns:SampleStage:V:status", this, 0.5);

	addOnsSampleNSetpointControl_ = new AMSinglePVControl("AddOnsSampleNSetpoint", "BL1607-B2-1:AddOns:SampleStage:N:mm", this, 0.001);
	addOnsSampleNFeedbackControl_ = new AMSinglePVControl("AddOnsSampleNFeedback", "BL1607-B2-1:AddOns:SampleStage:N:mm:fbk", this, 0.001);
	addOnsSampleNStatusControl_ = new AMSinglePVControl("AddOnsSampleHStatus", "BL1607-B2-1:AddOns:SampleStage:N:status", this, 0.5);

	// Sample X, Z, Y

	// Atto H, V, N

	oldAttoHSetpointControl_ = new AMSinglePVControl("oldAttoHSetpoint", "TS1607-2-B21-02:H:user:mm", this, 0.001);
	oldAttoHFeedbackControl_ = new AMReadOnlyPVControl("oldAttoHFeedback", "TS1607-2-B21-02:H:user:mm:fbk", this);

	oldAttoVSetpointControl_ = new AMSinglePVControl("oldAttoVSetpoint", "TS1607-2-B21-02:V:user:mm", this, 0.001);
	oldAttoVFeedbackControl_ = new AMReadOnlyPVControl("oldAttoVFeedback", "TS1607-2-B21-02:V:user:mm:fbk", this);

	oldAttoNSetpointControl_ = new AMSinglePVControl("oldAttoNSetpoint", "TS1607-2-B21-02:N:user:mm", this, 0.001);
	oldAttoNFeedbackControl_ = new AMReadOnlyPVControl("oldAttoNFeedback", "TS1607-2-B21-02:N:user:mm:fbk", this);

	oldRealAttoXStatusControl_ = new AMReadOnlyPVControl("oldRealAttoXFeedback", "SVM1607-2-B21-05:status", this);
	oldRealAttoYStatusControl_ = new AMReadOnlyPVControl("oldRealAttoYFeedback", "SVM1607-2-B21-06:status", this);
	oldRealAttoZStatusControl_ = new AMReadOnlyPVControl("oldRealAttoZFeedback", "SVM1607-2-B21-04:status", this);

	addOnsAttoHSetpointControl_ = new AMSinglePVControl("AddOnsAttoHSetpoint", "BL1607-B2-1:AddOns:AttoStage:H:mm", this, 0.001);
	addOnsAttoHFeedbackControl_ = new AMSinglePVControl("AddOnsAttoHFeedback", "BL1607-B2-1:AddOns:AttoStage:H:mm:fbk", this, 0.001);
	addOnsAttoHStatusControl_ = new AMSinglePVControl("AddOnsAttoHStatus", "BL1607-B2-1:AddOns:AttoStage:H:status", this, 0.5);

	addOnsAttoVSetpointControl_ = new AMSinglePVControl("AddOnsAttoVSetpoint", "BL1607-B2-1:AddOns:AttoStage:V:mm", this, 0.001);
	addOnsAttoVFeedbackControl_ = new AMSinglePVControl("AddOnsAttoVFeedback", "BL1607-B2-1:AddOns:AttoStage:V:mm:fbk", this, 0.001);
	addOnsAttoVStatusControl_ = new AMSinglePVControl("AddOnsAttoVStatus", "BL1607-B2-1:AddOns:AttoStage:V:status", this, 0.5);

	addOnsAttoNSetpointControl_ = new AMSinglePVControl("AddOnsAttoNSetpoint", "BL1607-B2-1:AddOns:AttoStage:N:mm", this, 0.001);
	addOnsAttoNFeedbackControl_ = new AMSinglePVControl("AddOnsAttoNFeedback", "BL1607-B2-1:AddOns:AttoStage:N:mm:fbk", this, 0.001);
	addOnsAttoNStatusControl_ = new AMSinglePVControl("AddOnsAttoHStatus", "BL1607-B2-1:AddOns:AttoStage:N:status", this, 0.5);


	// Atto X, Z, Y

	// Wire H, V, N

	oldWireHSetpointControl_ = new AMSinglePVControl("oldWireHSetpoint", "TS1607-2-B21-02:H:user:mm", this, 0.001);
	oldWireHFeedbackControl_ = new AMReadOnlyPVControl("oldWireHFeedback", "TS1607-2-B21-02:H:user:mm:fbk", this);

	oldWireVSetpointControl_ = new AMSinglePVControl("oldWireVSetpoint", "TS1607-2-B21-02:V:user:mm", this, 0.001);
	oldWireVFeedbackControl_ = new AMReadOnlyPVControl("oldWireVFeedback", "TS1607-2-B21-02:V:user:mm:fbk", this);

	oldWireNSetpointControl_ = new AMSinglePVControl("oldWireNSetpoint", "TS1607-2-B21-02:N:user:mm", this, 0.001);
	oldWireNFeedbackControl_ = new AMReadOnlyPVControl("oldWireNFeedback", "TS1607-2-B21-02:N:user:mm:fbk", this);

	oldRealWireXStatusControl_ = new AMReadOnlyPVControl("oldRealWireXFeedback", "SVM1607-2-B21-05:status", this);
	oldRealWireYStatusControl_ = new AMReadOnlyPVControl("oldRealWireYFeedback", "SVM1607-2-B21-06:status", this);
	oldRealWireZStatusControl_ = new AMReadOnlyPVControl("oldRealWireZFeedback", "SVM1607-2-B21-04:status", this);

	addOnsWireHSetpointControl_ = new AMSinglePVControl("AddOnsWireHSetpoint", "BL1607-B2-1:AddOns:WireStage:H:mm", this, 0.001);
	addOnsWireHFeedbackControl_ = new AMSinglePVControl("AddOnsWireHFeedback", "BL1607-B2-1:AddOns:WireStage:H:mm:fbk", this, 0.001);
	addOnsWireHStatusControl_ = new AMSinglePVControl("AddOnsWireHStatus", "BL1607-B2-1:AddOns:WireStage:H:status", this, 0.5);

	addOnsWireVSetpointControl_ = new AMSinglePVControl("AddOnsWireVSetpoint", "BL1607-B2-1:AddOns:WireStage:V:mm", this, 0.001);
	addOnsWireVFeedbackControl_ = new AMSinglePVControl("AddOnsWireVFeedback", "BL1607-B2-1:AddOns:WireStage:V:mm:fbk", this, 0.001);
	addOnsWireVStatusControl_ = new AMSinglePVControl("AddOnsWireVStatus", "BL1607-B2-1:AddOns:WireStage:V:status", this, 0.5);

	addOnsWireNSetpointControl_ = new AMSinglePVControl("AddOnsWireNSetpoint", "BL1607-B2-1:AddOns:WireStage:N:mm", this, 0.001);
	addOnsWireNFeedbackControl_ = new AMSinglePVControl("AddOnsWireNFeedback", "BL1607-B2-1:AddOns:WireStage:N:mm:fbk", this, 0.001);
	addOnsWireNStatusControl_ = new AMSinglePVControl("AddOnsWireHStatus", "BL1607-B2-1:AddOns:WireStage:N:status", this, 0.5);

	/////////////////////////////////////////////

	allControls_ = new AMControlSet(this);

	// Sample H, V, N

	allControls_->addControl(oldSampleHSetpointControl_);
	allControls_->addControl(oldSampleHFeedbackControl_);

	allControls_->addControl(oldSampleVSetpointControl_);
	allControls_->addControl(oldSampleVFeedbackControl_);

	allControls_->addControl(oldSampleNSetpointControl_);
	allControls_->addControl(oldSampleNFeedbackControl_);

	allControls_->addControl(oldRealSampleXStatusControl_);
	allControls_->addControl(oldRealSampleYStatusControl_);
	allControls_->addControl(oldRealSampleZStatusControl_);

	allControls_->addControl(addOnsSampleHSetpointControl_);
	allControls_->addControl(addOnsSampleHFeedbackControl_);
	allControls_->addControl(addOnsSampleHStatusControl_);

	allControls_->addControl(addOnsSampleVSetpointControl_);
	allControls_->addControl(addOnsSampleVFeedbackControl_);
	allControls_->addControl(addOnsSampleVStatusControl_);

	allControls_->addControl(addOnsSampleNSetpointControl_);
	allControls_->addControl(addOnsSampleNFeedbackControl_);
	allControls_->addControl(addOnsSampleNStatusControl_);

	// Sample X, Z, Y

	// Atto H, V, N

	allControls_->addControl(oldAttoHSetpointControl_);
	allControls_->addControl(oldAttoHFeedbackControl_);

	allControls_->addControl(oldAttoVSetpointControl_);
	allControls_->addControl(oldAttoVFeedbackControl_);

	allControls_->addControl(oldAttoNSetpointControl_);
	allControls_->addControl(oldAttoNFeedbackControl_);

	allControls_->addControl(oldRealAttoXStatusControl_);
	allControls_->addControl(oldRealAttoYStatusControl_);
	allControls_->addControl(oldRealAttoZStatusControl_);

	allControls_->addControl(addOnsAttoHSetpointControl_);
	allControls_->addControl(addOnsAttoHFeedbackControl_);
	allControls_->addControl(addOnsAttoHStatusControl_);

	allControls_->addControl(addOnsAttoVSetpointControl_);
	allControls_->addControl(addOnsAttoVFeedbackControl_);
	allControls_->addControl(addOnsAttoVStatusControl_);

	allControls_->addControl(addOnsAttoNSetpointControl_);
	allControls_->addControl(addOnsAttoNFeedbackControl_);
	allControls_->addControl(addOnsAttoNStatusControl_);

	// Atto X, Z, Y

	// Wire H, V, N

	allControls_->addControl(oldWireHSetpointControl_);
	allControls_->addControl(oldWireHFeedbackControl_);

	allControls_->addControl(oldWireVSetpointControl_);
	allControls_->addControl(oldWireVFeedbackControl_);

	allControls_->addControl(oldWireNSetpointControl_);
	allControls_->addControl(oldWireNFeedbackControl_);

	allControls_->addControl(oldRealWireXStatusControl_);
	allControls_->addControl(oldRealWireYStatusControl_);
	allControls_->addControl(oldRealWireZStatusControl_);

	allControls_->addControl(addOnsWireHSetpointControl_);
	allControls_->addControl(addOnsWireHFeedbackControl_);
	allControls_->addControl(addOnsWireHStatusControl_);

	allControls_->addControl(addOnsWireVSetpointControl_);
	allControls_->addControl(addOnsWireVFeedbackControl_);
	allControls_->addControl(addOnsWireVStatusControl_);

	allControls_->addControl(addOnsWireNSetpointControl_);
	allControls_->addControl(addOnsWireNFeedbackControl_);
	allControls_->addControl(addOnsWireNStatusControl_);

	/////////////////////////////////////////////////////

	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onAllControlsConnected(bool)));

	// Sample H, V, N

	connect(oldSampleHSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldSampleHSetpointControlChanged()));
	connect(oldSampleHFeedbackControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldSampleHFeedbackControlChanged()));

	connect(oldSampleVSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldSampleVSetpointControlChanged()));
	connect(oldSampleVFeedbackControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldSampleVFeedbackControlChanged()));

	connect(oldSampleNSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldSampleNSetpointControlChanged()));
	connect(oldSampleNFeedbackControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldSampleNFeedbackControlChanged()));

	connect(oldRealSampleXStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldRealSampleXStatusControlChanged()));
	connect(oldRealSampleYStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldRealSampleYAndZStatusControlChanged()));
	connect(oldRealSampleZStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldRealSampleYAndZStatusControlChanged()));

	connect(addOnsSampleHSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsSampleHSetpointControlChanged()));
	connect(addOnsSampleVSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsSampleVSetpointControlChanged()));
	connect(addOnsSampleNSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsSampleNSetpointControlChanged()));

	// Sample X, Z, Y

	// Atto H, V, N

	connect(oldAttoHSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldAttoHSetpointControlChanged()));
	connect(oldAttoHFeedbackControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldAttoHFeedbackControlChanged()));

	connect(oldAttoVSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldAttoVSetpointControlChanged()));
	connect(oldAttoVFeedbackControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldAttoVFeedbackControlChanged()));

	connect(oldAttoNSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldAttoNSetpointControlChanged()));
	connect(oldAttoNFeedbackControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldAttoNFeedbackControlChanged()));

	connect(oldRealAttoXStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldRealAttoXStatusControlChanged()));
	connect(oldRealAttoYStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldRealAttoYAndZStatusControlChanged()));
	connect(oldRealAttoZStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldRealAttoYAndZStatusControlChanged()));

	connect(addOnsAttoHSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsAttoHSetpointControlChanged()));
	connect(addOnsAttoVSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsAttoVSetpointControlChanged()));
	connect(addOnsAttoNSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsAttoNSetpointControlChanged()));

	// Atto X, Z, Y

	// Wire H, V, N

	connect(oldWireHSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldWireHSetpointControlChanged()));
	connect(oldWireHFeedbackControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldWireHFeedbackControlChanged()));

	connect(oldWireVSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldWireVSetpointControlChanged()));
	connect(oldWireVFeedbackControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldWireVFeedbackControlChanged()));

	connect(oldWireNSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldWireNSetpointControlChanged()));
	connect(oldWireNFeedbackControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldWireNFeedbackControlChanged()));

	connect(oldRealWireXStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldRealWireXStatusControlChanged()));
	connect(oldRealWireYStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldRealWireYAndZStatusControlChanged()));
	connect(oldRealWireZStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldRealWireYAndZStatusControlChanged()));

	connect(addOnsWireHSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsWireHSetpointControlChanged()));
	connect(addOnsWireVSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsWireVSetpointControlChanged()));
	connect(addOnsWireNSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsWireNSetpointControlChanged()));
}

VESPERSAddOnsCoordinator::~VESPERSAddOnsCoordinator(){}

void VESPERSAddOnsCoordinator::onAllControlsConnected(bool connected)
{
	if(connected){

		connectedOnce_ = true;

		qDebug() << "Checking start up value from old H setpoint control" << oldSampleHSetpointControl_->value();
		onOldSampleHSetpointControlChanged();
		qDebug() << "Checking start up value from old V setpoint control" << oldSampleVSetpointControl_->value();
		onOldSampleVSetpointControlChanged();
		qDebug() << "Checking start up value from old N setpoint control" << oldSampleNSetpointControl_->value();
		onOldSampleNSetpointControlChanged();

		qDebug() << "Checking start up value from old H feedback control" << oldSampleHFeedbackControl_->value();
		onOldSampleHFeedbackControlChanged();
		qDebug() << "Checking start up value from old V feedback control" << oldSampleVFeedbackControl_->value();
		onOldSampleVFeedbackControlChanged();
		qDebug() << "Checking start up value from old N feedback control" << oldSampleNFeedbackControl_->value();
		onOldSampleNFeedbackControlChanged();

		qDebug() << "Checking start up value from old read X status control" << oldRealSampleXStatusControl_->value();
		onOldRealSampleXStatusControlChanged();
		qDebug() << "Checking start up value from old read Y status control" << oldRealSampleYStatusControl_->value();
		qDebug() << "Checking start up value from old read Z status control" << oldRealSampleZStatusControl_->value();
		onOldRealSampleYAndZStatusControlChanged();


	}
}

int VESPERSAddOnsCoordinator::computeFinalStatus(int status1, int status2) const
{
	int yStatus = status1;
	int zStatus = status2;

	int finalStatus = -1;

	if (yStatus == 0 && zStatus == 0)
		finalStatus = 0;

	else if (yStatus == 1 || zStatus == 1)
		finalStatus = 1;

	else if (yStatus == 2 || zStatus == 2)
		finalStatus = 2;

	else if (yStatus == 3 || zStatus == 3)
		finalStatus = 3;

	else if (yStatus == 4 || zStatus == 4)
		finalStatus = 4;

	return finalStatus;
}

// Sample H, V, N
//////////////////////////////////////////////

void VESPERSAddOnsCoordinator::onOldSampleHSetpointControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD sample H setpoint move of " << oldSampleHSetpointControl_->value() << " versus " << addOnsSampleHSetpointControl_->value();

	if(!addOnsSampleHSetpointControl_->withinTolerance(oldSampleHSetpointControl_->value()))
		addOnsSampleHSetpointControl_->move(oldSampleHSetpointControl_->value());
}

void VESPERSAddOnsCoordinator::onOldSampleHFeedbackControlChanged()
{
	if(!connectedOnce_)
		return;


	if(!addOnsSampleHFeedbackControl_->withinTolerance(oldSampleHFeedbackControl_->value())){
		qDebug() << "Detected OLD sample H feedback move of " << oldSampleHFeedbackControl_->value() << " versus " << addOnsSampleHFeedbackControl_->value();
		addOnsSampleHFeedbackControl_->move(oldSampleHFeedbackControl_->value());
	}
}

void VESPERSAddOnsCoordinator::onOldSampleVSetpointControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD sample V setpoint move of " << oldSampleVSetpointControl_->value() << " versus " << addOnsSampleVSetpointControl_->value();

	if(!addOnsSampleVSetpointControl_->withinTolerance(oldSampleVSetpointControl_->value()))
		addOnsSampleVSetpointControl_->move(oldSampleVSetpointControl_->value());
}

void VESPERSAddOnsCoordinator::onOldSampleVFeedbackControlChanged()
{
	if(!connectedOnce_)
		return;


	if(!addOnsSampleVFeedbackControl_->withinTolerance(oldSampleVFeedbackControl_->value())){
		addOnsSampleVFeedbackControl_->move(oldSampleVFeedbackControl_->value());
		qDebug() << "Detected OLD sample V feedback move of " << oldSampleVFeedbackControl_->value() << " versus " << addOnsSampleVFeedbackControl_->value();
	}
}

void VESPERSAddOnsCoordinator::onOldSampleNSetpointControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD sample N setpoint move of " << oldSampleNSetpointControl_->value() << " versus " << addOnsSampleNSetpointControl_->value();

	if(!addOnsSampleNSetpointControl_->withinTolerance(oldSampleNSetpointControl_->value()))
		addOnsSampleNSetpointControl_->move(oldSampleNSetpointControl_->value());
}

void VESPERSAddOnsCoordinator::onOldSampleNFeedbackControlChanged()
{
	if(!connectedOnce_)
		return;


	if(!addOnsSampleNFeedbackControl_->withinTolerance(oldSampleNFeedbackControl_->value())){
		qDebug() << "Detected OLD sample N feedback move of " << oldSampleNFeedbackControl_->value() << " versus " << addOnsSampleNFeedbackControl_->value();
		addOnsSampleNFeedbackControl_->move(oldSampleNFeedbackControl_->value());
	}
}

void VESPERSAddOnsCoordinator::onOldRealSampleXStatusControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD sample X status move of " << oldRealSampleXStatusControl_->value() << " versus " << addOnsSampleHStatusControl_->value();

	if(!addOnsSampleHStatusControl_->withinTolerance(oldRealSampleXStatusControl_->value()))
		addOnsSampleHStatusControl_->move(oldRealSampleXStatusControl_->value());
}

void VESPERSAddOnsCoordinator::onOldRealSampleYAndZStatusControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD sample Y status move of " << oldRealSampleYStatusControl_->value() << " versus " << addOnsSampleVStatusControl_->value() << " and " << addOnsSampleNStatusControl_->value();

	int finalStatus = computeFinalStatus(int(oldRealSampleYStatusControl_->value()), int(oldRealSampleZStatusControl_->value()));

	if(!addOnsSampleVStatusControl_->withinTolerance(finalStatus))
		addOnsSampleVStatusControl_->move(finalStatus);

	if(!addOnsSampleNStatusControl_->withinTolerance(finalStatus))
		addOnsSampleNStatusControl_->move(finalStatus);
}

void VESPERSAddOnsCoordinator::onAddOnsSampleHSetpointControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected new AddOns sample H move of " << addOnsSampleHSetpointControl_->value() << " versus " << oldSampleHSetpointControl_->value();

	// If we're requesting the same stage X with AddOns as was in the old stage X AND the old stage X feedback matches the old stage X setpoint AND the status is MOVE DONE
	// THEN issuing a move will not do anything SO we'll trick the status to say "MOVE ACTIVE" then "MOVE DONE"
	if(oldSampleHSetpointControl_->withinTolerance(addOnsSampleHSetpointControl_->value()) && oldRealSampleXStatusControl_->withinTolerance(0.0)){
		qDebug() << "Faking a sample stage X move";
		addOnsSampleHStatusControl_->move(1.0);
		QTimer::singleShot(50, this, SLOT(restoreAddOnsSampleHStatus()));
		return;
	}

	if(!oldSampleHSetpointControl_->withinTolerance(addOnsSampleHSetpointControl_->value())){
		qDebug() << "Doing addOns Sample H move in the regular way";
		oldSampleHSetpointControl_->move(addOnsSampleHSetpointControl_->value());
	}
}

void VESPERSAddOnsCoordinator::onAddOnsSampleVSetpointControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected new AddOns sample V move of " << addOnsSampleVSetpointControl_->value() << " versus " << oldSampleVSetpointControl_->value();

	// If we're requesting the same stage X with AddOns as was in the old stage X AND the old stage X feedback matches the old stage X setpoint AND the status is MOVE DONE
	// THEN issuing a move will not do anything SO we'll trick the status to say "MOVE ACTIVE" then "MOVE DONE"
	if(oldSampleVSetpointControl_->withinTolerance(addOnsSampleVSetpointControl_->value())
			&& oldRealSampleYStatusControl_->withinTolerance(0.0)
			 && oldRealSampleZStatusControl_->withinTolerance(0.0)){
		qDebug() << "Faking a sample stage V move";
		addOnsSampleVStatusControl_->move(1.0);
		addOnsSampleNStatusControl_->move(1.0);
		QTimer::singleShot(50, this, SLOT(restoreAddOnsSampleVAndNStatus()));
		return;
	}

	if(!oldSampleVSetpointControl_->withinTolerance(addOnsSampleVSetpointControl_->value())){
		qDebug() << "Doing addOns Sample V move in the regular way";
		oldSampleVSetpointControl_->move(addOnsSampleVSetpointControl_->value());
	}
}

void VESPERSAddOnsCoordinator::onAddOnsSampleNSetpointControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected new AddOns sample N move of " << addOnsSampleNSetpointControl_->value() << " versus " << oldSampleNSetpointControl_->value();

	// If we're requesting the same stage X with AddOns as was in the old stage X AND the old stage X feedback matches the old stage X setpoint AND the status is MOVE DONE
	// THEN issuing a move will not do anything SO we'll trick the status to say "MOVE ACTIVE" then "MOVE DONE"
	if(oldSampleNSetpointControl_->withinTolerance(addOnsSampleNSetpointControl_->value())
			&& oldRealSampleYStatusControl_->withinTolerance(0.0)
			 && oldRealSampleZStatusControl_->withinTolerance(0.0)){
		qDebug() << "Faking a sample stage N move";
		addOnsSampleVStatusControl_->move(1.0);
		addOnsSampleNStatusControl_->move(1.0);
		QTimer::singleShot(50, this, SLOT(restoreAddOnsSampleVAndNStatus()));
		return;
	}

	if(!oldSampleNSetpointControl_->withinTolerance(addOnsSampleNSetpointControl_->value())){
		qDebug() << "Doing addOns Sample N move in the regular way";
		oldSampleNSetpointControl_->move(addOnsSampleNSetpointControl_->value());
	}
}

void VESPERSAddOnsCoordinator::restoreAddOnsSampleHStatus()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Restoring AddOns sample stage H status to old VESPERS sample stage H status with AddOns " << addOnsSampleHStatusControl_->value() << " and old X status " << oldRealSampleXStatusControl_->value();

	if(!addOnsSampleHStatusControl_->withinTolerance(oldRealSampleXStatusControl_->value()))
		addOnsSampleHStatusControl_->move(oldRealSampleXStatusControl_->value());
}

void VESPERSAddOnsCoordinator::restoreAddOnsSampleVAndNStatus()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Restoring AddOns sample stage H status to old VESPERS sample stage V and N status with AddOns " << addOnsSampleVStatusControl_->value() << " and old X status "  << oldRealSampleYStatusControl_->value() << " and " << oldRealSampleZStatusControl_->value();

	int finalStatus = computeFinalStatus(int(oldRealSampleYStatusControl_->value()), int(oldRealSampleZStatusControl_->value()));

	if(!addOnsSampleVStatusControl_->withinTolerance(finalStatus))
		addOnsSampleVStatusControl_->move(finalStatus);

	if(!addOnsSampleNStatusControl_->withinTolerance(finalStatus))
		addOnsSampleNStatusControl_->move(finalStatus);
}

// Sample X, Z, Y
//////////////////////////////////////////////

// Atto H, V, N
//////////////////////////////////////////////

void VESPERSAddOnsCoordinator::onOldAttoHSetpointControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD atto H setpoint move of " << oldAttoHSetpointControl_->value() << " versus " << addOnsAttoHSetpointControl_->value();

	if(!addOnsAttoHSetpointControl_->withinTolerance(oldAttoHSetpointControl_->value()))
		addOnsAttoHSetpointControl_->move(oldAttoHSetpointControl_->value());
}

void VESPERSAddOnsCoordinator::onOldAttoHFeedbackControlChanged()
{
	if(!connectedOnce_)
		return;


	if(!addOnsAttoHFeedbackControl_->withinTolerance(oldAttoHFeedbackControl_->value())){
		qDebug() << "Detected OLD atto H feedback move of " << oldAttoHFeedbackControl_->value() << " versus " << addOnsAttoHFeedbackControl_->value();
		addOnsAttoHFeedbackControl_->move(oldAttoHFeedbackControl_->value());
	}
}

void VESPERSAddOnsCoordinator::onOldAttoVSetpointControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD atto V setpoint move of " << oldAttoVSetpointControl_->value() << " versus " << addOnsAttoVSetpointControl_->value();

	if(!addOnsAttoVSetpointControl_->withinTolerance(oldAttoVSetpointControl_->value()))
		addOnsAttoVSetpointControl_->move(oldAttoVSetpointControl_->value());
}

void VESPERSAddOnsCoordinator::onOldAttoVFeedbackControlChanged()
{
	if(!connectedOnce_)
		return;


	if(!addOnsAttoVFeedbackControl_->withinTolerance(oldAttoVFeedbackControl_->value())){
		addOnsAttoVFeedbackControl_->move(oldAttoVFeedbackControl_->value());
		qDebug() << "Detected OLD atto V feedback move of " << oldAttoVFeedbackControl_->value() << " versus " << addOnsAttoVFeedbackControl_->value();
	}
}

void VESPERSAddOnsCoordinator::onOldAttoNSetpointControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD atto N setpoint move of " << oldAttoNSetpointControl_->value() << " versus " << addOnsAttoNSetpointControl_->value();

	if(!addOnsAttoNSetpointControl_->withinTolerance(oldAttoNSetpointControl_->value()))
		addOnsAttoNSetpointControl_->move(oldAttoNSetpointControl_->value());
}

void VESPERSAddOnsCoordinator::onOldAttoNFeedbackControlChanged()
{
	if(!connectedOnce_)
		return;


	if(!addOnsAttoNFeedbackControl_->withinTolerance(oldAttoNFeedbackControl_->value())){
		qDebug() << "Detected OLD atto N feedback move of " << oldAttoNFeedbackControl_->value() << " versus " << addOnsAttoNFeedbackControl_->value();
		addOnsAttoNFeedbackControl_->move(oldAttoNFeedbackControl_->value());
	}
}

void VESPERSAddOnsCoordinator::onOldRealAttoXStatusControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD atto X status move of " << oldRealAttoXStatusControl_->value() << " versus " << addOnsAttoHStatusControl_->value();

	if(!addOnsAttoHStatusControl_->withinTolerance(oldRealAttoXStatusControl_->value()))
		addOnsAttoHStatusControl_->move(oldRealAttoXStatusControl_->value());
}

void VESPERSAddOnsCoordinator::onOldRealAttoYAndZStatusControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD atto Y status move of " << oldRealAttoYStatusControl_->value() << " versus " << addOnsAttoVStatusControl_->value() << " and " << addOnsAttoNStatusControl_->value();

	int finalStatus = computeFinalStatus(int(oldRealAttoYStatusControl_->value()), int(oldRealAttoZStatusControl_->value()));

	if(!addOnsAttoVStatusControl_->withinTolerance(finalStatus))
		addOnsAttoVStatusControl_->move(finalStatus);

	if(!addOnsAttoNStatusControl_->withinTolerance(finalStatus))
		addOnsAttoNStatusControl_->move(finalStatus);
}

void VESPERSAddOnsCoordinator::onAddOnsAttoHSetpointControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected new AddOns atto H move of " << addOnsAttoHSetpointControl_->value() << " versus " << oldAttoHSetpointControl_->value();

	// If we're requesting the same stage X with AddOns as was in the old stage X AND the old stage X feedback matches the old stage X setpoint AND the status is MOVE DONE
	// THEN issuing a move will not do anything SO we'll trick the status to say "MOVE ACTIVE" then "MOVE DONE"
	if(oldAttoHSetpointControl_->withinTolerance(addOnsAttoHSetpointControl_->value()) && oldRealAttoXStatusControl_->withinTolerance(0.0)){
		qDebug() << "Faking a atto stage X move";
		addOnsAttoHStatusControl_->move(1.0);
		QTimer::singleShot(50, this, SLOT(restoreAddOnsAttoHStatus()));
		return;
	}

	if(!oldAttoHSetpointControl_->withinTolerance(addOnsAttoHSetpointControl_->value())){
		qDebug() << "Doing addOns Atto H move in the regular way";
		oldAttoHSetpointControl_->move(addOnsAttoHSetpointControl_->value());
	}
}

void VESPERSAddOnsCoordinator::onAddOnsAttoVSetpointControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected new AddOns atto V move of " << addOnsAttoVSetpointControl_->value() << " versus " << oldAttoVSetpointControl_->value();

	// If we're requesting the same stage X with AddOns as was in the old stage X AND the old stage X feedback matches the old stage X setpoint AND the status is MOVE DONE
	// THEN issuing a move will not do anything SO we'll trick the status to say "MOVE ACTIVE" then "MOVE DONE"
	if(oldAttoVSetpointControl_->withinTolerance(addOnsAttoVSetpointControl_->value())
			&& oldRealAttoYStatusControl_->withinTolerance(0.0)
			 && oldRealAttoZStatusControl_->withinTolerance(0.0)){
		qDebug() << "Faking a sample stage V move";
		addOnsAttoVStatusControl_->move(1.0);
		addOnsAttoNStatusControl_->move(1.0);
		QTimer::singleShot(50, this, SLOT(restoreAddOnsAttoVAndNStatus()));
		return;
	}

	if(!oldAttoVSetpointControl_->withinTolerance(addOnsAttoVSetpointControl_->value())){
		qDebug() << "Doing addOns Atto V move in the regular way";
		oldAttoVSetpointControl_->move(addOnsAttoVSetpointControl_->value());
	}
}

void VESPERSAddOnsCoordinator::onAddOnsAttoNSetpointControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected new AddOns atto N move of " << addOnsAttoNSetpointControl_->value() << " versus " << oldAttoNSetpointControl_->value();

	// If we're requesting the same stage X with AddOns as was in the old stage X AND the old stage X feedback matches the old stage X setpoint AND the status is MOVE DONE
	// THEN issuing a move will not do anything SO we'll trick the status to say "MOVE ACTIVE" then "MOVE DONE"
	if(oldAttoNSetpointControl_->withinTolerance(addOnsAttoNSetpointControl_->value())
			&& oldRealAttoYStatusControl_->withinTolerance(0.0)
			 && oldRealAttoZStatusControl_->withinTolerance(0.0)){
		qDebug() << "Faking a sample stage N move";
		addOnsAttoVStatusControl_->move(1.0);
		addOnsAttoNStatusControl_->move(1.0);
		QTimer::singleShot(50, this, SLOT(restoreAddOnsAttoVAndNStatus()));
		return;
	}

	if(!oldAttoNSetpointControl_->withinTolerance(addOnsAttoNSetpointControl_->value())){
		qDebug() << "Doing addOns Atto N move in the regular way";
		oldAttoNSetpointControl_->move(addOnsAttoNSetpointControl_->value());
	}
}

void VESPERSAddOnsCoordinator::restoreAddOnsAttoHStatus()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Restoring AddOns atto stage H status to old VESPERS atto stage H status with AddOns " << addOnsAttoHStatusControl_->value() << " and old X status " << oldRealAttoXStatusControl_->value();

	if(!addOnsAttoHStatusControl_->withinTolerance(oldRealAttoXStatusControl_->value()))
		addOnsAttoHStatusControl_->move(oldRealAttoXStatusControl_->value());
}

void VESPERSAddOnsCoordinator::restoreAddOnsAttoVAndNStatus()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Restoring AddOns atto stage H status to old VESPERS atto stage V and N status with AddOns " << addOnsAttoVStatusControl_->value() << " and old X status "  << oldRealAttoYStatusControl_->value() << " and " << oldRealAttoZStatusControl_->value();

	int finalStatus = computeFinalStatus(int(oldRealAttoYStatusControl_->value()), int(oldRealAttoZStatusControl_->value()));

	if(!addOnsAttoVStatusControl_->withinTolerance(finalStatus))
		addOnsAttoVStatusControl_->move(finalStatus);

	if(!addOnsAttoNStatusControl_->withinTolerance(finalStatus))
		addOnsAttoNStatusControl_->move(finalStatus);
}

// Atto X, Z, Y
//////////////////////////////////////////////

// Wire H, V, N
//////////////////////////////////////////////

void VESPERSAddOnsCoordinator::onOldWireHSetpointControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD wire H setpoint move of " << oldWireHSetpointControl_->value() << " versus " << addOnsWireHSetpointControl_->value();

	if(!addOnsWireHSetpointControl_->withinTolerance(oldWireHSetpointControl_->value()))
		addOnsWireHSetpointControl_->move(oldWireHSetpointControl_->value());
}

void VESPERSAddOnsCoordinator::onOldWireHFeedbackControlChanged()
{
	if(!connectedOnce_)
		return;


	if(!addOnsWireHFeedbackControl_->withinTolerance(oldWireHFeedbackControl_->value())){
		qDebug() << "Detected OLD wire H feedback move of " << oldWireHFeedbackControl_->value() << " versus " << addOnsWireHFeedbackControl_->value();
		addOnsWireHFeedbackControl_->move(oldWireHFeedbackControl_->value());
	}
}

void VESPERSAddOnsCoordinator::onOldWireVSetpointControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD wire V setpoint move of " << oldWireVSetpointControl_->value() << " versus " << addOnsWireVSetpointControl_->value();

	if(!addOnsWireVSetpointControl_->withinTolerance(oldWireVSetpointControl_->value()))
		addOnsWireVSetpointControl_->move(oldWireVSetpointControl_->value());
}

void VESPERSAddOnsCoordinator::onOldWireVFeedbackControlChanged()
{
	if(!connectedOnce_)
		return;


	if(!addOnsWireVFeedbackControl_->withinTolerance(oldWireVFeedbackControl_->value())){
		addOnsWireVFeedbackControl_->move(oldWireVFeedbackControl_->value());
		qDebug() << "Detected OLD wire V feedback move of " << oldWireVFeedbackControl_->value() << " versus " << addOnsWireVFeedbackControl_->value();
	}
}

void VESPERSAddOnsCoordinator::onOldWireNSetpointControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD wire N setpoint move of " << oldWireNSetpointControl_->value() << " versus " << addOnsWireNSetpointControl_->value();

	if(!addOnsWireNSetpointControl_->withinTolerance(oldWireNSetpointControl_->value()))
		addOnsWireNSetpointControl_->move(oldWireNSetpointControl_->value());
}

void VESPERSAddOnsCoordinator::onOldWireNFeedbackControlChanged()
{
	if(!connectedOnce_)
		return;


	if(!addOnsWireNFeedbackControl_->withinTolerance(oldWireNFeedbackControl_->value())){
		qDebug() << "Detected OLD wire N feedback move of " << oldWireNFeedbackControl_->value() << " versus " << addOnsWireNFeedbackControl_->value();
		addOnsWireNFeedbackControl_->move(oldWireNFeedbackControl_->value());
	}
}

void VESPERSAddOnsCoordinator::onOldRealWireXStatusControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD wire X status move of " << oldRealWireXStatusControl_->value() << " versus " << addOnsWireHStatusControl_->value();

	if(!addOnsWireHStatusControl_->withinTolerance(oldRealWireXStatusControl_->value()))
		addOnsWireHStatusControl_->move(oldRealWireXStatusControl_->value());
}

void VESPERSAddOnsCoordinator::onOldRealWireYAndZStatusControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD wire Y status move of " << oldRealWireYStatusControl_->value() << " versus " << addOnsWireVStatusControl_->value() << " and " << addOnsWireNStatusControl_->value();

	int finalStatus = computeFinalStatus(int(oldRealWireYStatusControl_->value()), int(oldRealWireZStatusControl_->value()));

	if(!addOnsWireVStatusControl_->withinTolerance(finalStatus))
		addOnsWireVStatusControl_->move(finalStatus);

	if(!addOnsWireNStatusControl_->withinTolerance(finalStatus))
		addOnsWireNStatusControl_->move(finalStatus);
}

void VESPERSAddOnsCoordinator::onAddOnsWireHSetpointControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected new AddOns wire H move of " << addOnsWireHSetpointControl_->value() << " versus " << oldWireHSetpointControl_->value();

	// If we're requesting the same stage X with AddOns as was in the old stage X AND the old stage X feedback matches the old stage X setpoint AND the status is MOVE DONE
	// THEN issuing a move will not do anything SO we'll trick the status to say "MOVE ACTIVE" then "MOVE DONE"
	if(oldWireHSetpointControl_->withinTolerance(addOnsWireHSetpointControl_->value()) && oldRealWireXStatusControl_->withinTolerance(0.0)){
		qDebug() << "Faking a wire stage X move";
		addOnsWireHStatusControl_->move(1.0);
		QTimer::singleShot(50, this, SLOT(restoreAddOnsWireHStatus()));
		return;
	}

	if(!oldWireHSetpointControl_->withinTolerance(addOnsWireHSetpointControl_->value())){
		qDebug() << "Doing addOns Wire H move in the regular way";
		oldWireHSetpointControl_->move(addOnsWireHSetpointControl_->value());
	}
}

void VESPERSAddOnsCoordinator::onAddOnsWireVSetpointControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected new AddOns wire V move of " << addOnsWireVSetpointControl_->value() << " versus " << oldWireVSetpointControl_->value();

	// If we're requesting the same stage X with AddOns as was in the old stage X AND the old stage X feedback matches the old stage X setpoint AND the status is MOVE DONE
	// THEN issuing a move will not do anything SO we'll trick the status to say "MOVE ACTIVE" then "MOVE DONE"
	if(oldWireVSetpointControl_->withinTolerance(addOnsWireVSetpointControl_->value())
			&& oldRealWireYStatusControl_->withinTolerance(0.0)
			 && oldRealWireZStatusControl_->withinTolerance(0.0)){
		qDebug() << "Faking a wire stage V move";
		addOnsWireVStatusControl_->move(1.0);
		addOnsWireNStatusControl_->move(1.0);
		QTimer::singleShot(50, this, SLOT(restoreAddOnsWireVAndNStatus()));
		return;
	}

	if(!oldWireVSetpointControl_->withinTolerance(addOnsWireVSetpointControl_->value())){
		qDebug() << "Doing addOns Wire V move in the regular way";
		oldWireVSetpointControl_->move(addOnsWireVSetpointControl_->value());
	}
}

void VESPERSAddOnsCoordinator::onAddOnsWireNSetpointControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected new AddOns wire N move of " << addOnsWireNSetpointControl_->value() << " versus " << oldWireNSetpointControl_->value();

	// If we're requesting the same stage X with AddOns as was in the old stage X AND the old stage X feedback matches the old stage X setpoint AND the status is MOVE DONE
	// THEN issuing a move will not do anything SO we'll trick the status to say "MOVE ACTIVE" then "MOVE DONE"
	if(oldWireNSetpointControl_->withinTolerance(addOnsWireNSetpointControl_->value())
			&& oldRealWireYStatusControl_->withinTolerance(0.0)
			 && oldRealWireZStatusControl_->withinTolerance(0.0)){
		qDebug() << "Faking a sample stage N move";
		addOnsWireVStatusControl_->move(1.0);
		addOnsWireNStatusControl_->move(1.0);
		QTimer::singleShot(50, this, SLOT(restoreAddOnsWireVAndNStatus()));
		return;
	}

	if(!oldWireNSetpointControl_->withinTolerance(addOnsWireNSetpointControl_->value())){
		qDebug() << "Doing addOns Wire N move in the regular way";
		oldWireNSetpointControl_->move(addOnsWireNSetpointControl_->value());
	}
}

void VESPERSAddOnsCoordinator::restoreAddOnsWireHStatus()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Restoring AddOns wire stage H status to old VESPERS wire stage H status with AddOns " << addOnsWireHStatusControl_->value() << " and old X status " << oldRealWireXStatusControl_->value();

	if(!addOnsWireHStatusControl_->withinTolerance(oldRealWireXStatusControl_->value()))
		addOnsWireHStatusControl_->move(oldRealWireXStatusControl_->value());
}

void VESPERSAddOnsCoordinator::restoreAddOnsWireVAndNStatus()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Restoring AddOns wire stage H status to old VESPERS wire stage V and N status with AddOns " << addOnsWireVStatusControl_->value() << " and old X status "  << oldRealWireYStatusControl_->value() << " and " << oldRealWireZStatusControl_->value();

	int finalStatus = computeFinalStatus(int(oldRealWireYStatusControl_->value()), int(oldRealWireZStatusControl_->value()));

	if(!addOnsWireVStatusControl_->withinTolerance(finalStatus))
		addOnsWireVStatusControl_->move(finalStatus);

	if(!addOnsWireNStatusControl_->withinTolerance(finalStatus))
		addOnsWireNStatusControl_->move(finalStatus);
}
