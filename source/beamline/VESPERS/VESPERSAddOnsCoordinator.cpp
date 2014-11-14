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

	allControls_ = new AMControlSet(this);
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

	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onAllControlsConnected(bool)));

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

	qDebug() << "Restoring AddOns sample stage H status to old VESPERS sample stage V and N status with AddOns " << addOnsSampleVStatusControl_->value() << " and old X status "  << addOnsSampleVStatusControl_->value() << " and " << addOnsSampleNStatusControl_->value();

	int finalStatus = computeFinalStatus(int(oldRealSampleYStatusControl_->value()), int(oldRealSampleZStatusControl_->value()));

	if(!addOnsSampleVStatusControl_->withinTolerance(finalStatus))
		addOnsSampleVStatusControl_->move(finalStatus);

	if(!addOnsSampleNStatusControl_->withinTolerance(finalStatus))
		addOnsSampleNStatusControl_->move(finalStatus);
}
