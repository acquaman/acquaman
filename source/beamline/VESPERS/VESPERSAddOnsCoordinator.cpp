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

VESPERSAddOnsCoordinator::VESPERSAddOnsCoordinator(QObject *parent)
	: QObject(parent)
{
	connectedOnce_ = false;

	// Sample H, V, N

	oldSampleHSetpointControl_ = new AMSinglePVControl("oldSampleHSetpoint", "TS1607-2-B21-02:H:user:mm", this, 0.001);
	oldSampleHFeedbackControl_ = new AMReadOnlyPVControl("oldSampleHFeedback", "TS1607-2-B21-02:H:user:mm:fbk", this);

	oldSampleVSetpointControl_ = new AMSinglePVControl("oldSampleVSetpoint", "TS1607-2-B21-02:V:user:mm", this, 0.001);
	oldSampleVFeedbackControl_ = new AMReadOnlyPVControl("oldSampleVFeedback", "TS1607-2-B21-02:V:user:mm:fbk", this);

	oldSampleNSetpointControl_ = new AMSinglePVControl("oldSampleNSetpoint", "TS1607-2-B21-02:N:user:mm", this, 0.001);
	oldSampleNFeedbackControl_ = new AMReadOnlyPVControl("oldSampleNFeedback", "TS1607-2-B21-02:N:user:mm:fbk", this);

	addOnsSampleHSetpointControl_ = new AMSinglePVControl("AddOnsSampleHSetpoint", "BL1607-B2-1:AddOns:SampleStage:H:mm", this, 0.001);
	addOnsSampleHFeedbackControl_ = new AMSinglePVControl("AddOnsSampleHFeedback", "BL1607-B2-1:AddOns:SampleStage:H:mm:fbk", this, 0.001);
	addOnsSampleHStatusControl_ = new AMSinglePVControl("AddOnsSampleHStatus", "BL1607-B2-1:AddOns:SampleStage:H:status", this, 0.5);

	addOnsSampleVSetpointControl_ = new AMSinglePVControl("AddOnsSampleVSetpoint", "BL1607-B2-1:AddOns:SampleStage:V:mm", this, 0.001);
	addOnsSampleVFeedbackControl_ = new AMSinglePVControl("AddOnsSampleVFeedback", "BL1607-B2-1:AddOns:SampleStage:V:mm:fbk", this, 0.001);
	addOnsSampleVStatusControl_ = new AMSinglePVControl("AddOnsSampleVStatus", "BL1607-B2-1:AddOns:SampleStage:V:status", this, 0.5);

	addOnsSampleNSetpointControl_ = new AMSinglePVControl("AddOnsSampleNSetpoint", "BL1607-B2-1:AddOns:SampleStage:N:mm", this, 0.001);
	addOnsSampleNFeedbackControl_ = new AMSinglePVControl("AddOnsSampleNFeedback", "BL1607-B2-1:AddOns:SampleStage:N:mm:fbk", this, 0.001);
	addOnsSampleNStatusControl_ = new AMSinglePVControl("AddOnsSampleNStatus", "BL1607-B2-1:AddOns:SampleStage:N:status", this, 0.5);

	// Sample X, Z, Y

	oldSampleXSetpointControl_ = new AMSinglePVControl("oldSampleXSetpoint", "TS1607-2-B21-02:X:user:mm", this, 0.001);
	oldSampleXFeedbackControl_ = new AMReadOnlyPVControl("oldSampleXFeedback", "TS1607-2-B21-02:X:user:mm:fbk", this);

	oldSampleZSetpointControl_ = new AMSinglePVControl("oldSampleZSetpoint", "TS1607-2-B21-02:Z:user:mm", this, 0.001);
	oldSampleZFeedbackControl_ = new AMReadOnlyPVControl("oldSampleZFeedback", "TS1607-2-B21-02:Z:user:mm:fbk", this);

	oldSampleYSetpointControl_ = new AMSinglePVControl("oldSampleYSetpoint", "TS1607-2-B21-02:Y:user:mm", this, 0.001);
	oldSampleYFeedbackControl_ = new AMReadOnlyPVControl("oldSampleYFeedback", "TS1607-2-B21-02:Y:user:mm:fbk", this);

	addOnsSampleXSetpointControl_ = new AMSinglePVControl("AddOnsSampleXSetpoint", "BL1607-B2-1:AddOns:SampleStage:X:mm", this, 0.001);
	addOnsSampleXFeedbackControl_ = new AMSinglePVControl("AddOnsSampleXFeedback", "BL1607-B2-1:AddOns:SampleStage:X:mm:fbk", this, 0.001);
	addOnsSampleXStatusControl_ = new AMSinglePVControl("AddOnsSampleXStatus", "BL1607-B2-1:AddOns:SampleStage:X:status", this, 0.5);

	addOnsSampleZSetpointControl_ = new AMSinglePVControl("AddOnsSampleZSetpoint", "BL1607-B2-1:AddOns:SampleStage:Z:mm", this, 0.001);
	addOnsSampleZFeedbackControl_ = new AMSinglePVControl("AddOnsSampleZFeedback", "BL1607-B2-1:AddOns:SampleStage:Z:mm:fbk", this, 0.001);
	addOnsSampleZStatusControl_ = new AMSinglePVControl("AddOnsSampleZStatus", "BL1607-B2-1:AddOns:SampleStage:Z:status", this, 0.5);

	addOnsSampleYSetpointControl_ = new AMSinglePVControl("AddOnsSampleYSetpoint", "BL1607-B2-1:AddOns:SampleStage:Y:mm", this, 0.001);
	addOnsSampleYFeedbackControl_ = new AMSinglePVControl("AddOnsSampleYFeedback", "BL1607-B2-1:AddOns:SampleStage:Y:mm:fbk", this, 0.001);
	addOnsSampleYStatusControl_ = new AMSinglePVControl("AddOnsSampleYStatus", "BL1607-B2-1:AddOns:SampleStage:Y:status", this, 0.5);

	// Sample status

	oldRealSampleXStatusControl_ = new AMReadOnlyPVControl("oldRealSampleXFeedback", "SVM1607-2-B21-05:status", this);
	oldRealSampleYStatusControl_ = new AMReadOnlyPVControl("oldRealSampleYFeedback", "SVM1607-2-B21-06:status", this);
	oldRealSampleZStatusControl_ = new AMReadOnlyPVControl("oldRealSampleZFeedback", "SVM1607-2-B21-04:status", this);

	// Atto H, V, N

	oldAttoHSetpointControl_ = new AMSinglePVControl("oldAttoHSetpoint", "TS1607-2-B21-07:H:user:mm", this, 0.001);
	oldAttoHFeedbackControl_ = new AMReadOnlyPVControl("oldAttoHFeedback", "TS1607-2-B21-07:H:user:mm:fbk", this);

	oldAttoVSetpointControl_ = new AMSinglePVControl("oldAttoVSetpoint", "TS1607-2-B21-07:V:user:mm", this, 0.001);
	oldAttoVFeedbackControl_ = new AMReadOnlyPVControl("oldAttoVFeedback", "TS1607-2-B21-07:V:user:mm:fbk", this);

	oldAttoNSetpointControl_ = new AMSinglePVControl("oldAttoNSetpoint", "TS1607-2-B21-07:N:user:mm", this, 0.001);
	oldAttoNFeedbackControl_ = new AMReadOnlyPVControl("oldAttoNFeedback", "TS1607-2-B21-07:N:user:mm:fbk", this);

	addOnsAttoHSetpointControl_ = new AMSinglePVControl("AddOnsAttoHSetpoint", "BL1607-B2-1:AddOns:AttoStage:H:mm", this, 0.001);
	addOnsAttoHFeedbackControl_ = new AMSinglePVControl("AddOnsAttoHFeedback", "BL1607-B2-1:AddOns:AttoStage:H:mm:fbk", this, 0.001);
	addOnsAttoHStatusControl_ = new AMSinglePVControl("AddOnsAttoHStatus", "BL1607-B2-1:AddOns:AttoStage:H:status", this, 0.5);

	addOnsAttoVSetpointControl_ = new AMSinglePVControl("AddOnsAttoVSetpoint", "BL1607-B2-1:AddOns:AttoStage:V:mm", this, 0.001);
	addOnsAttoVFeedbackControl_ = new AMSinglePVControl("AddOnsAttoVFeedback", "BL1607-B2-1:AddOns:AttoStage:V:mm:fbk", this, 0.001);
	addOnsAttoVStatusControl_ = new AMSinglePVControl("AddOnsAttoVStatus", "BL1607-B2-1:AddOns:AttoStage:V:status", this, 0.5);

	addOnsAttoNSetpointControl_ = new AMSinglePVControl("AddOnsAttoNSetpoint", "BL1607-B2-1:AddOns:AttoStage:N:mm", this, 0.001);
	addOnsAttoNFeedbackControl_ = new AMSinglePVControl("AddOnsAttoNFeedback", "BL1607-B2-1:AddOns:AttoStage:N:mm:fbk", this, 0.001);
	addOnsAttoNStatusControl_ = new AMSinglePVControl("AddOnsAttoNStatus", "BL1607-B2-1:AddOns:AttoStage:N:status", this, 0.5);

	// Atto X, Z, Y

	oldAttoXSetpointControl_ = new AMSinglePVControl("oldAttoXSetpoint", "TS1607-2-B21-07:X:user:mm", this, 0.001);
	oldAttoXFeedbackControl_ = new AMReadOnlyPVControl("oldAttoXFeedback", "TS1607-2-B21-07:X:user:mm:fbk", this);

	oldAttoZSetpointControl_ = new AMSinglePVControl("oldAttoZSetpoint", "TS1607-2-B21-07:Z:user:mm", this, 0.001);
	oldAttoZFeedbackControl_ = new AMReadOnlyPVControl("oldAttoZFeedback", "TS1607-2-B21-07:Z:user:mm:fbk", this);

	oldAttoYSetpointControl_ = new AMSinglePVControl("oldAttoYSetpoint", "TS1607-2-B21-07:Y:user:mm", this, 0.001);
	oldAttoYFeedbackControl_ = new AMReadOnlyPVControl("oldAttoYFeedback", "TS1607-2-B21-07:Y:user:mm:fbk", this);

	addOnsAttoXSetpointControl_ = new AMSinglePVControl("AddOnsAttoXSetpoint", "BL1607-B2-1:AddOns:AttoStage:X:mm", this, 0.001);
	addOnsAttoXFeedbackControl_ = new AMSinglePVControl("AddOnsAttoXFeedback", "BL1607-B2-1:AddOns:AttoStage:X:mm:fbk", this, 0.001);
	addOnsAttoXStatusControl_ = new AMSinglePVControl("AddOnsAttoXStatus", "BL1607-B2-1:AddOns:AttoStage:X:status", this, 0.5);

	addOnsAttoZSetpointControl_ = new AMSinglePVControl("AddOnsAttoZSetpoint", "BL1607-B2-1:AddOns:AttoStage:Z:mm", this, 0.001);
	addOnsAttoZFeedbackControl_ = new AMSinglePVControl("AddOnsAttoZFeedback", "BL1607-B2-1:AddOns:AttoStage:Z:mm:fbk", this, 0.001);
	addOnsAttoZStatusControl_ = new AMSinglePVControl("AddOnsAttoZStatus", "BL1607-B2-1:AddOns:AttoStage:Z:status", this, 0.5);

	addOnsAttoYSetpointControl_ = new AMSinglePVControl("AddOnsAttoYSetpoint", "BL1607-B2-1:AddOns:AttoStage:Y:mm", this, 0.001);
	addOnsAttoYFeedbackControl_ = new AMSinglePVControl("AddOnsAttoYFeedback", "BL1607-B2-1:AddOns:AttoStage:Y:mm:fbk", this, 0.001);
	addOnsAttoYStatusControl_ = new AMSinglePVControl("AddOnsAttoYStatus", "BL1607-B2-1:AddOns:AttoStage:Y:status", this, 0.5);

	// Atto status

	oldRealAttoXStatusControl_ = new AMReadOnlyPVControl("oldRealAttoXFeedback", "SVM1607-2-B21-11:status", this);
	oldRealAttoYStatusControl_ = new AMReadOnlyPVControl("oldRealAttoYFeedback", "SVM1607-2-B21-10:status", this);
	oldRealAttoZStatusControl_ = new AMReadOnlyPVControl("oldRealAttoZFeedback", "SVM1607-2-B21-12:status", this);

	// Atto rotation Rx, Ry, Rz

	oldAttoRxSetpointControl_ = new AMSinglePVControl("oldAttoRxSetpoint", "SVM1607-2-B21-08:deg", this, 0.0001);
	oldAttoRxFeedbackControl_ = new AMReadOnlyPVControl("oldAttoRxFeedback", "SVM1607-2-B21-08:deg:fbk", this);

	oldAttoRzSetpointControl_ = new AMSinglePVControl("oldAttoRzSetpoint", "SVM1607-2-B21-09:deg", this, 0.001);
	oldAttoRzFeedbackControl_ = new AMReadOnlyPVControl("oldAttoRzFeedback", "SVM1607-2-B21-09:deg:fbk", this);

	oldAttoRySetpointControl_ = new AMSinglePVControl("oldAttoRySetpoint", "SVM1607-2-B21-07:deg", this, 0.001);
	oldAttoRyFeedbackControl_ = new AMReadOnlyPVControl("oldAttoRyFeedback", "SVM1607-2-B21-07:deg:fbk", this);

	addOnsAttoRxSetpointControl_ = new AMSinglePVControl("AddOnsAttoRxSetpoint", "BL1607-B2-1:AddOns:AttoStage:Rx:deg", this, 0.0001);
	addOnsAttoRxFeedbackControl_ = new AMSinglePVControl("AddOnsAttoRxFeedback", "BL1607-B2-1:AddOns:AttoStage:Rx:deg:fbk", this, 0.0001);
	addOnsAttoRxStatusControl_ = new AMSinglePVControl("AddOnsAttoRxStatus", "BL1607-B2-1:AddOns:AttoStage:Rx:status", this, 0.5);

	addOnsAttoRzSetpointControl_ = new AMSinglePVControl("AddOnsAttoRzSetpoint", "BL1607-B2-1:AddOns:AttoStage:Rz:deg", this, 0.001);
	addOnsAttoRzFeedbackControl_ = new AMSinglePVControl("AddOnsAttoRzFeedback", "BL1607-B2-1:AddOns:AttoStage:Rz:deg:fbk", this, 0.001);
	addOnsAttoRzStatusControl_ = new AMSinglePVControl("AddOnsAttoRzStatus", "BL1607-B2-1:AddOns:AttoStage:Rz:status", this, 0.5);

	addOnsAttoRySetpointControl_ = new AMSinglePVControl("AddOnsAttoRySetpoint", "BL1607-B2-1:AddOns:AttoStage:Ry:deg", this, 0.001);
	addOnsAttoRyFeedbackControl_ = new AMSinglePVControl("AddOnsAttoRyFeedback", "BL1607-B2-1:AddOns:AttoStage:Ry:deg:fbk", this, 0.001);
	addOnsAttoRyStatusControl_ = new AMSinglePVControl("AddOnsAttoRyStatus", "BL1607-B2-1:AddOns:AttoStage:Ry:status", this, 0.5);

	// Atto rotation status

	oldRealAttoRxStatusControl_ = new AMReadOnlyPVControl("oldRealAttoRxFeedback", "SVM1607-2-B21-08:status", this);
	oldRealAttoRyStatusControl_ = new AMReadOnlyPVControl("oldRealAttoRyFeedback", "SVM1607-2-B21-07:status", this);
	oldRealAttoRzStatusControl_ = new AMReadOnlyPVControl("oldRealAttoRzFeedback", "SVM1607-2-B21-09:status", this);

	// Wire H, V, N

	oldWireHSetpointControl_ = new AMSinglePVControl("oldWireHSetpoint", "TS1607-2-B21-01:H:user:mm", this, 0.001);
	oldWireHFeedbackControl_ = new AMReadOnlyPVControl("oldWireHFeedback", "TS1607-2-B21-01:H:user:mm:fbk", this);

	oldWireVSetpointControl_ = new AMSinglePVControl("oldWireVSetpoint", "TS1607-2-B21-01:V:user:mm", this, 0.001);
	oldWireVFeedbackControl_ = new AMReadOnlyPVControl("oldWireVFeedback", "TS1607-2-B21-01:V:user:mm:fbk", this);

	oldWireNSetpointControl_ = new AMSinglePVControl("oldWireNSetpoint", "TS1607-2-B21-01:N:user:mm", this, 0.001);
	oldWireNFeedbackControl_ = new AMReadOnlyPVControl("oldWireNFeedback", "TS1607-2-B21-01:N:user:mm:fbk", this);

	addOnsWireHSetpointControl_ = new AMSinglePVControl("AddOnsWireHSetpoint", "BL1607-B2-1:AddOns:WireStage:H:mm", this, 0.001);
	addOnsWireHFeedbackControl_ = new AMSinglePVControl("AddOnsWireHFeedback", "BL1607-B2-1:AddOns:WireStage:H:mm:fbk", this, 0.001);
	addOnsWireHStatusControl_ = new AMSinglePVControl("AddOnsWireHStatus", "BL1607-B2-1:AddOns:WireStage:H:status", this, 0.5);

	addOnsWireVSetpointControl_ = new AMSinglePVControl("AddOnsWireVSetpoint", "BL1607-B2-1:AddOns:WireStage:V:mm", this, 0.001);
	addOnsWireVFeedbackControl_ = new AMSinglePVControl("AddOnsWireVFeedback", "BL1607-B2-1:AddOns:WireStage:V:mm:fbk", this, 0.001);
	addOnsWireVStatusControl_ = new AMSinglePVControl("AddOnsWireVStatus", "BL1607-B2-1:AddOns:WireStage:V:status", this, 0.5);

	addOnsWireNSetpointControl_ = new AMSinglePVControl("AddOnsWireNSetpoint", "BL1607-B2-1:AddOns:WireStage:N:mm", this, 0.001);
	addOnsWireNFeedbackControl_ = new AMSinglePVControl("AddOnsWireNFeedback", "BL1607-B2-1:AddOns:WireStage:N:mm:fbk", this, 0.001);
	addOnsWireNStatusControl_ = new AMSinglePVControl("AddOnsWireNStatus", "BL1607-B2-1:AddOns:WireStage:N:status", this, 0.5);

	// Wire status

	oldRealWireXStatusControl_ = new AMReadOnlyPVControl("oldRealWireXFeedback", "SVM1607-2-B21-02:status", this);
	oldRealWireYStatusControl_ = new AMReadOnlyPVControl("oldRealWireYFeedback", "SVM1607-2-B21-03:status", this);
	oldRealWireZStatusControl_ = new AMReadOnlyPVControl("oldRealWireZFeedback", "SVM1607-2-B21-01:status", this);

	/////////////////////////////////////////////

	allControls_ = new AMControlSet(this);

	// Sample H, V, N

	allControls_->addControl(oldSampleHSetpointControl_);
	allControls_->addControl(oldSampleHFeedbackControl_);

	allControls_->addControl(oldSampleVSetpointControl_);
	allControls_->addControl(oldSampleVFeedbackControl_);

	allControls_->addControl(oldSampleNSetpointControl_);
	allControls_->addControl(oldSampleNFeedbackControl_);

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

	allControls_->addControl(oldSampleXSetpointControl_);
	allControls_->addControl(oldSampleXFeedbackControl_);

	allControls_->addControl(oldSampleZSetpointControl_);
	allControls_->addControl(oldSampleZFeedbackControl_);

	allControls_->addControl(oldSampleYSetpointControl_);
	allControls_->addControl(oldSampleYFeedbackControl_);

	allControls_->addControl(addOnsSampleXSetpointControl_);
	allControls_->addControl(addOnsSampleXFeedbackControl_);
	allControls_->addControl(addOnsSampleXStatusControl_);

	allControls_->addControl(addOnsSampleZSetpointControl_);
	allControls_->addControl(addOnsSampleZFeedbackControl_);
	allControls_->addControl(addOnsSampleZStatusControl_);

	allControls_->addControl(addOnsSampleYSetpointControl_);
	allControls_->addControl(addOnsSampleYFeedbackControl_);
	allControls_->addControl(addOnsSampleYStatusControl_);

	// Sample low level status

	allControls_->addControl(oldRealSampleXStatusControl_);
	allControls_->addControl(oldRealSampleYStatusControl_);
	allControls_->addControl(oldRealSampleZStatusControl_);

	// Atto H, V, N

/*
	allControls_->addControl(oldAttoHSetpointControl_);
	allControls_->addControl(oldAttoHFeedbackControl_);

	allControls_->addControl(oldAttoVSetpointControl_);
	allControls_->addControl(oldAttoVFeedbackControl_);

	allControls_->addControl(oldAttoNSetpointControl_);
	allControls_->addControl(oldAttoNFeedbackControl_);

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

	allControls_->addControl(oldAttoXSetpointControl_);
	allControls_->addControl(oldAttoXFeedbackControl_);

	allControls_->addControl(oldAttoZSetpointControl_);
	allControls_->addControl(oldAttoZFeedbackControl_);

	allControls_->addControl(oldAttoYSetpointControl_);
	allControls_->addControl(oldAttoYFeedbackControl_);

	allControls_->addControl(addOnsAttoXSetpointControl_);
	allControls_->addControl(addOnsAttoXFeedbackControl_);
	allControls_->addControl(addOnsAttoXStatusControl_);

	allControls_->addControl(addOnsAttoZSetpointControl_);
	allControls_->addControl(addOnsAttoZFeedbackControl_);
	allControls_->addControl(addOnsAttoZStatusControl_);

	allControls_->addControl(addOnsAttoYSetpointControl_);
	allControls_->addControl(addOnsAttoYFeedbackControl_);
	allControls_->addControl(addOnsAttoYStatusControl_);

	// Atto low level status

	allControls_->addControl(oldRealAttoXStatusControl_);
	allControls_->addControl(oldRealAttoYStatusControl_);
	allControls_->addControl(oldRealAttoZStatusControl_);

	// Atto rotation Rx, Rz, Ry

	allControls_->addControl(oldAttoRxSetpointControl_);
	allControls_->addControl(oldAttoRxFeedbackControl_);

	allControls_->addControl(oldAttoRzSetpointControl_);
	allControls_->addControl(oldAttoRzFeedbackControl_);

	allControls_->addControl(oldAttoRySetpointControl_);
	allControls_->addControl(oldAttoRyFeedbackControl_);

	allControls_->addControl(addOnsAttoRxSetpointControl_);
	allControls_->addControl(addOnsAttoRxFeedbackControl_);
	allControls_->addControl(addOnsAttoRxStatusControl_);

	allControls_->addControl(addOnsAttoRzSetpointControl_);
	allControls_->addControl(addOnsAttoRzFeedbackControl_);
	allControls_->addControl(addOnsAttoRzStatusControl_);

	allControls_->addControl(addOnsAttoRySetpointControl_);
	allControls_->addControl(addOnsAttoRyFeedbackControl_);
	allControls_->addControl(addOnsAttoRyStatusControl_);

	// Atto low level status

	allControls_->addControl(oldRealAttoRxStatusControl_);
	allControls_->addControl(oldRealAttoRyStatusControl_);
	allControls_->addControl(oldRealAttoRzStatusControl_);
*/

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

	connect(addOnsSampleHSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsSampleHSetpointControlChanged()));
	connect(addOnsSampleVSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsSampleVSetpointControlChanged()));
	connect(addOnsSampleNSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsSampleNSetpointControlChanged()));

	// Sample X, Z, Y

	connect(oldSampleXSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldSampleXSetpointControlChanged()));
	connect(oldSampleXFeedbackControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldSampleXFeedbackControlChanged()));

	connect(oldSampleZSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldSampleZSetpointControlChanged()));
	connect(oldSampleZFeedbackControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldSampleZFeedbackControlChanged()));

	connect(oldSampleYSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldSampleYSetpointControlChanged()));
	connect(oldSampleYFeedbackControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldSampleYFeedbackControlChanged()));

	connect(addOnsSampleXSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsSampleXSetpointControlChanged()));
	connect(addOnsSampleZSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsSampleZSetpointControlChanged()));
	connect(addOnsSampleYSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsSampleYSetpointControlChanged()));

	// Sample low level status

	connect(oldRealSampleXStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldRealSampleXStatusControlChanged()));
	connect(oldRealSampleYStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldRealSampleYStatusControlChanged()));
	connect(oldRealSampleZStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldRealSampleZStatusControlChanged()));

	// Atto H, V, N

	connect(oldAttoHSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldAttoHSetpointControlChanged()));
	connect(oldAttoHFeedbackControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldAttoHFeedbackControlChanged()));

	connect(oldAttoVSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldAttoVSetpointControlChanged()));
	connect(oldAttoVFeedbackControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldAttoVFeedbackControlChanged()));

	connect(oldAttoNSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldAttoNSetpointControlChanged()));
	connect(oldAttoNFeedbackControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldAttoNFeedbackControlChanged()));

	connect(addOnsAttoHSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsAttoHSetpointControlChanged()));
	connect(addOnsAttoVSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsAttoVSetpointControlChanged()));
	connect(addOnsAttoNSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsAttoNSetpointControlChanged()));

	// Atto X, Z, Y

	connect(oldAttoXSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldAttoXSetpointControlChanged()));
	connect(oldAttoXFeedbackControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldAttoXFeedbackControlChanged()));

	connect(oldAttoZSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldAttoZSetpointControlChanged()));
	connect(oldAttoZFeedbackControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldAttoZFeedbackControlChanged()));

	connect(oldAttoYSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldAttoYSetpointControlChanged()));
	connect(oldAttoYFeedbackControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldAttoYFeedbackControlChanged()));

	connect(addOnsAttoXSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsAttoXSetpointControlChanged()));
	connect(addOnsAttoZSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsAttoZSetpointControlChanged()));
	connect(addOnsAttoYSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsAttoYSetpointControlChanged()));

	// Atto low level status

	connect(oldRealAttoXStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldRealAttoXStatusControlChanged()));
	connect(oldRealAttoYStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldRealAttoYStatusControlChanged()));
	connect(oldRealAttoZStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldRealAttoZStatusControlChanged()));

	// Atto rotation Rx, Rz, Ry

	connect(oldAttoRxSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldAttoRxSetpointControlChanged()));
	connect(oldAttoRxFeedbackControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldAttoRxFeedbackControlChanged()));

	connect(oldAttoRzSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldAttoRzSetpointControlChanged()));
	connect(oldAttoRzFeedbackControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldAttoRzFeedbackControlChanged()));

	connect(oldAttoRySetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldAttoRySetpointControlChanged()));
	connect(oldAttoRyFeedbackControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldAttoRyFeedbackControlChanged()));

	connect(addOnsAttoRxSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsAttoRxSetpointControlChanged()));
	connect(addOnsAttoRzSetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsAttoRzSetpointControlChanged()));
	connect(addOnsAttoRySetpointControl_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsAttoRySetpointControlChanged()));

	// Atto rotation low level status

	connect(oldRealAttoRxStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldRealAttoRxStatusControlChanged()));
	connect(oldRealAttoRyStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldRealAttoRyStatusControlChanged()));
	connect(oldRealAttoRzStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onOldRealAttoRzStatusControlChanged()));

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

		// Sample stage
		/////////////////////////////////////

		qDebug() << "Checking start up value from old sample H setpoint control" << oldSampleHSetpointControl_->value();
		onOldSampleHSetpointControlChanged();
		qDebug() << "Checking start up value from old sample V setpoint control" << oldSampleVSetpointControl_->value();
		onOldSampleVSetpointControlChanged();
		qDebug() << "Checking start up value from old sample N setpoint control" << oldSampleNSetpointControl_->value();
		onOldSampleNSetpointControlChanged();

		qDebug() << "Checking start up value from old sample H feedback control" << oldSampleHFeedbackControl_->value();
		onOldSampleHFeedbackControlChanged();
		qDebug() << "Checking start up value from old sample V feedback control" << oldSampleVFeedbackControl_->value();
		onOldSampleVFeedbackControlChanged();
		qDebug() << "Checking start up value from old sample N feedback control" << oldSampleNFeedbackControl_->value();
		onOldSampleNFeedbackControlChanged();

		qDebug() << "Checking start up value from old sample X setpoint control" << oldSampleXSetpointControl_->value();
		onOldSampleXSetpointControlChanged();
		qDebug() << "Checking start up value from old sample Z setpoint control" << oldSampleZSetpointControl_->value();
		onOldSampleZSetpointControlChanged();
		qDebug() << "Checking start up value from old sample Y setpoint control" << oldSampleYSetpointControl_->value();
		onOldSampleYSetpointControlChanged();

		qDebug() << "Checking start up value from old sample X feedback control" << oldSampleXFeedbackControl_->value();
		onOldSampleXFeedbackControlChanged();
		qDebug() << "Checking start up value from old sample Z feedback control" << oldSampleZFeedbackControl_->value();
		onOldSampleZFeedbackControlChanged();
		qDebug() << "Checking start up value from old sample Y feedback control" << oldSampleYFeedbackControl_->value();
		onOldSampleYFeedbackControlChanged();

		qDebug() << "Checking start up value from old sample X status control" << oldRealSampleXStatusControl_->value();
		onOldRealSampleXStatusControlChanged();
		qDebug() << "Checking start up value from old sample Y status control" << oldRealSampleYStatusControl_->value();
		onOldRealSampleYStatusControlChanged();
		qDebug() << "Checking start up value from old sample Z status control" << oldRealSampleZStatusControl_->value();
		onOldRealSampleZStatusControlChanged();

		// Atto stage
		//////////////////////////////////////////

		qDebug() << "Checking start up value from old atto H setpoint control" << oldAttoHSetpointControl_->value();
		onOldAttoHSetpointControlChanged();
		qDebug() << "Checking start up value from old atto V setpoint control" << oldAttoVSetpointControl_->value();
		onOldAttoVSetpointControlChanged();
		qDebug() << "Checking start up value from old atto N setpoint control" << oldAttoNSetpointControl_->value();
		onOldAttoNSetpointControlChanged();

		qDebug() << "Checking start up value from old atto H feedback control" << oldAttoHFeedbackControl_->value();
		onOldAttoHFeedbackControlChanged();
		qDebug() << "Checking start up value from old atto V feedback control" << oldAttoVFeedbackControl_->value();
		onOldAttoVFeedbackControlChanged();
		qDebug() << "Checking start up value from old atto N feedback control" << oldAttoNFeedbackControl_->value();
		onOldAttoNFeedbackControlChanged();

		qDebug() << "Checking start up value from old atto X setpoint control" << oldAttoXSetpointControl_->value();
		onOldAttoXSetpointControlChanged();
		qDebug() << "Checking start up value from old atto Z setpoint control" << oldAttoZSetpointControl_->value();
		onOldAttoZSetpointControlChanged();
		qDebug() << "Checking start up value from old atto Y setpoint control" << oldAttoYSetpointControl_->value();
		onOldAttoYSetpointControlChanged();

		qDebug() << "Checking start up value from old atto X feedback control" << oldAttoXFeedbackControl_->value();
		onOldAttoXFeedbackControlChanged();
		qDebug() << "Checking start up value from old atto Z feedback control" << oldAttoZFeedbackControl_->value();
		onOldAttoZFeedbackControlChanged();
		qDebug() << "Checking start up value from old atto Y feedback control" << oldAttoYFeedbackControl_->value();
		onOldAttoYFeedbackControlChanged();

		qDebug() << "Checking start up value from old atto X status control" << oldRealAttoXStatusControl_->value();
		onOldRealAttoXStatusControlChanged();
		qDebug() << "Checking start up value from old atto Y status control" << oldRealAttoYStatusControl_->value();
		onOldRealAttoYStatusControlChanged();
		qDebug() << "Checking start up value from old atto Z status control" << oldRealAttoZStatusControl_->value();
		onOldRealAttoZStatusControlChanged();

		// Atto Rotation

		qDebug() << "Checking start up value from old atto Rx setpoint control" << oldAttoRxSetpointControl_->value();
		onOldAttoRxSetpointControlChanged();
		qDebug() << "Checking start up value from old atto Rz setpoint control" << oldAttoRzSetpointControl_->value();
		onOldAttoRzSetpointControlChanged();
		qDebug() << "Checking start up value from old atto Ry setpoint control" << oldAttoRySetpointControl_->value();
		onOldAttoRySetpointControlChanged();

		qDebug() << "Checking start up value from old atto Rx feedback control" << oldAttoRxFeedbackControl_->value();
		onOldAttoRxFeedbackControlChanged();
		qDebug() << "Checking start up value from old atto Rz feedback control" << oldAttoRzFeedbackControl_->value();
		onOldAttoRzFeedbackControlChanged();
		qDebug() << "Checking start up value from old atto Ry feedback control" << oldAttoRyFeedbackControl_->value();
		onOldAttoRyFeedbackControlChanged();

		qDebug() << "Checking start up value from old atto Rx status control" << oldRealAttoRxStatusControl_->value();
		onOldRealAttoRxStatusControlChanged();
		qDebug() << "Checking start up value from old atto Ry status control" << oldRealAttoRyStatusControl_->value();
		onOldRealAttoRyStatusControlChanged();
		qDebug() << "Checking start up value from old atto Rz status control" << oldRealAttoRzStatusControl_->value();
		onOldRealAttoRzStatusControlChanged();

		// Wire stage
		//////////////////////////////////////////

		qDebug() << "Checking start up value from old wire H setpoint control" << oldWireHSetpointControl_->value();
		onOldWireHSetpointControlChanged();
		qDebug() << "Checking start up value from old wire V setpoint control" << oldWireVSetpointControl_->value();
		onOldWireVSetpointControlChanged();
		qDebug() << "Checking start up value from old wire N setpoint control" << oldWireNSetpointControl_->value();
		onOldWireNSetpointControlChanged();

		qDebug() << "Checking start up value from old wire H feedback control" << oldWireHFeedbackControl_->value();
		onOldWireHFeedbackControlChanged();
		qDebug() << "Checking start up value from old wire V feedback control" << oldWireVFeedbackControl_->value();
		onOldWireVFeedbackControlChanged();
		qDebug() << "Checking start up value from old wire N feedback control" << oldWireNFeedbackControl_->value();
		onOldWireNFeedbackControlChanged();

		qDebug() << "Checking start up value from old wire X status control" << oldRealWireXStatusControl_->value();
		onOldRealWireXStatusControlChanged();
		qDebug() << "Checking start up value from old wire Y status control" << oldRealWireYStatusControl_->value();
		qDebug() << "Checking start up value from old wire Z status control" << oldRealWireZStatusControl_->value();
		onOldRealWireYAndZStatusControlChanged();
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

void VESPERSAddOnsCoordinator::onAddOnsSampleHSetpointControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected new AddOns sample H move of " << addOnsSampleHSetpointControl_->value() << " versus " << oldSampleHSetpointControl_->value();

	// If we're requesting the same stage X with AddOns as was in the old stage X AND the old stage X feedback matches the old stage X setpoint AND the status is MOVE DONE
	// THEN issuing a move will not do anything SO we'll trick the status to say "MOVE ACTIVE" then "MOVE DONE"
	if(oldSampleHSetpointControl_->withinTolerance(addOnsSampleHSetpointControl_->value()) && !oldRealSampleXStatusControl_->withinTolerance(1.0)){
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
			&& !oldRealSampleYStatusControl_->withinTolerance(1.0)
			 && !oldRealSampleZStatusControl_->withinTolerance(1.0)){
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
			&& !oldRealSampleYStatusControl_->withinTolerance(1.0)
			 && !oldRealSampleZStatusControl_->withinTolerance(1.0)){
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

void VESPERSAddOnsCoordinator::onOldSampleXSetpointControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD sample X setpoint move of " << oldSampleXSetpointControl_->value() << " versus " << addOnsSampleXSetpointControl_->value();

	if(!addOnsSampleXSetpointControl_->withinTolerance(oldSampleXSetpointControl_->value()))
		addOnsSampleXSetpointControl_->move(oldSampleXSetpointControl_->value());
}

void VESPERSAddOnsCoordinator::onOldSampleXFeedbackControlChanged()
{
	if(!connectedOnce_)
		return;


	if(!addOnsSampleXFeedbackControl_->withinTolerance(oldSampleXFeedbackControl_->value())){
		qDebug() << "Detected OLD sample X feedback move of " << oldSampleXFeedbackControl_->value() << " versus " << addOnsSampleXFeedbackControl_->value();
		addOnsSampleXFeedbackControl_->move(oldSampleXFeedbackControl_->value());
	}
}

void VESPERSAddOnsCoordinator::onOldSampleZSetpointControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD sample Z setpoint move of " << oldSampleZSetpointControl_->value() << " versus " << addOnsSampleZSetpointControl_->value();

	if(!addOnsSampleZSetpointControl_->withinTolerance(oldSampleZSetpointControl_->value()))
		addOnsSampleZSetpointControl_->move(oldSampleZSetpointControl_->value());
}

void VESPERSAddOnsCoordinator::onOldSampleZFeedbackControlChanged()
{
	if(!connectedOnce_)
		return;


	if(!addOnsSampleZFeedbackControl_->withinTolerance(oldSampleZFeedbackControl_->value())){
		addOnsSampleZFeedbackControl_->move(oldSampleZFeedbackControl_->value());
		qDebug() << "Detected OLD sample Z feedback move of " << oldSampleZFeedbackControl_->value() << " versus " << addOnsSampleZFeedbackControl_->value();
	}
}

void VESPERSAddOnsCoordinator::onOldSampleYSetpointControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD sample Y setpoint move of " << oldSampleYSetpointControl_->value() << " versus " << addOnsSampleYSetpointControl_->value();

	if(!addOnsSampleYSetpointControl_->withinTolerance(oldSampleYSetpointControl_->value()))
		addOnsSampleYSetpointControl_->move(oldSampleYSetpointControl_->value());
}

void VESPERSAddOnsCoordinator::onOldSampleYFeedbackControlChanged()
{
	if(!connectedOnce_)
		return;


	if(!addOnsSampleYFeedbackControl_->withinTolerance(oldSampleYFeedbackControl_->value())){
		qDebug() << "Detected OLD sample Y feedback move of " << oldSampleYFeedbackControl_->value() << " versus " << addOnsSampleYFeedbackControl_->value();
		addOnsSampleYFeedbackControl_->move(oldSampleYFeedbackControl_->value());
	}
}

void VESPERSAddOnsCoordinator::onAddOnsSampleXSetpointControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected new AddOns sample X move of " << addOnsSampleXSetpointControl_->value() << " versus " << oldSampleXSetpointControl_->value();

	// If we're requesting the same stage X with AddOns as was in the old stage X AND the old stage X feedback matches the old stage X setpoint AND the status is MOVE DONE
	// THEN issuing a move will not do anything SO we'll trick the status to say "MOVE ACTIVE" then "MOVE DONE"
	if(oldSampleXSetpointControl_->withinTolerance(addOnsSampleXSetpointControl_->value()) && !oldRealSampleXStatusControl_->withinTolerance(1.0)){
		qDebug() << "Faking a sample stage X move";
		addOnsSampleXStatusControl_->move(1.0);
		QTimer::singleShot(50, this, SLOT(restoreAddOnsSampleXStatus()));
		return;
	}

	if(!oldSampleXSetpointControl_->withinTolerance(addOnsSampleXSetpointControl_->value())){
		qDebug() << "Doing addOns Sample X move in the regular way";
		oldSampleXSetpointControl_->move(addOnsSampleXSetpointControl_->value());
	}
}

void VESPERSAddOnsCoordinator::onAddOnsSampleZSetpointControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected new AddOns sample Z move of " << addOnsSampleZSetpointControl_->value() << " versus " << oldSampleZSetpointControl_->value();

	// If we're requesting the same stage X with AddOns as was in the old stage X AND the old stage X feedback matches the old stage X setpoint AND the status is MOZE DONE
	// THEN issuing a move will not do anything SO we'll trick the status to say "MOZE ACTIZE" then "MOZE DONE"
	if(oldSampleZSetpointControl_->withinTolerance(addOnsSampleZSetpointControl_->value()) && !oldRealSampleZStatusControl_->withinTolerance(1.0)){
		qDebug() << "Faking a sample stage Z move";
		addOnsSampleZStatusControl_->move(1.0);
		QTimer::singleShot(50, this, SLOT(restoreAddOnsSampleZStatus()));
		return;
	}

	if(!oldSampleZSetpointControl_->withinTolerance(addOnsSampleZSetpointControl_->value())){
		qDebug() << "Doing addOns Sample Z move in the regular way";
		oldSampleZSetpointControl_->move(addOnsSampleZSetpointControl_->value());
	}
}

void VESPERSAddOnsCoordinator::onAddOnsSampleYSetpointControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected new AddOns sample Y move of " << addOnsSampleYSetpointControl_->value() << " versus " << oldSampleYSetpointControl_->value();

	// If we're requesting the same stage X with AddOns as was in the old stage X AND the old stage X feedback matches the old stage X setpoint AND the status is MOVE DONE
	// THEN issuing a move will not do anything SO we'll trick the status to say "MOVE ACTIVE" then "MOVE DONE"
	if(oldSampleYSetpointControl_->withinTolerance(addOnsSampleYSetpointControl_->value()) && !oldRealSampleYStatusControl_->withinTolerance(1.0)){
		qDebug() << "Faking a sample stage Y move";
		addOnsSampleYStatusControl_->move(1.0);
		QTimer::singleShot(50, this, SLOT(restoreAddOnsSampleYStatus()));
		return;
	}

	if(!oldSampleYSetpointControl_->withinTolerance(addOnsSampleYSetpointControl_->value())){
		qDebug() << "Doing addOns Sample Y move in the regular way";
		oldSampleYSetpointControl_->move(addOnsSampleYSetpointControl_->value());
	}
}

void VESPERSAddOnsCoordinator::restoreAddOnsSampleXStatus()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Restoring AddOns sample stage X status to old VESPERS sample stage X status with AddOns " << addOnsSampleXStatusControl_->value() << " and old X status " << oldRealSampleXStatusControl_->value();

	if(!addOnsSampleXStatusControl_->withinTolerance(oldRealSampleXStatusControl_->value()))
		addOnsSampleXStatusControl_->move(oldRealSampleXStatusControl_->value());
}

void VESPERSAddOnsCoordinator::restoreAddOnsSampleZStatus()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Restoring AddOns sample stage Z status to old VESPERS sample stage Z status with AddOns " << addOnsSampleZStatusControl_->value() << " and old Z status " << oldRealSampleZStatusControl_->value();

	if(!addOnsSampleZStatusControl_->withinTolerance(oldRealSampleZStatusControl_->value()))
		addOnsSampleZStatusControl_->move(oldRealSampleZStatusControl_->value());
}

void VESPERSAddOnsCoordinator::restoreAddOnsSampleYStatus()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Restoring AddOns sample stage Y status to old VESPERS sample stage Y status with AddOns " << addOnsSampleYStatusControl_->value() << " and old Y status " << oldRealSampleYStatusControl_->value();

	if(!addOnsSampleYStatusControl_->withinTolerance(oldRealSampleYStatusControl_->value()))
		addOnsSampleYStatusControl_->move(oldRealSampleYStatusControl_->value());
}

// Sample low level status
/////////////////////////////////////////////

void VESPERSAddOnsCoordinator::onOldRealSampleXStatusControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD sample X status move of " << oldRealSampleXStatusControl_->value() << " versus " << addOnsSampleHStatusControl_->value();

	if(!addOnsSampleHStatusControl_->withinTolerance(oldRealSampleXStatusControl_->value()))
		addOnsSampleHStatusControl_->move(oldRealSampleXStatusControl_->value());

	qDebug() << "Detected OLD sample X status move of " << oldRealSampleXStatusControl_->value() << " versus " << addOnsSampleXStatusControl_->value();

	if(!addOnsSampleXStatusControl_->withinTolerance(oldRealSampleXStatusControl_->value()))
		addOnsSampleXStatusControl_->move(oldRealSampleXStatusControl_->value());
}

void VESPERSAddOnsCoordinator::onOldRealSampleYStatusControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD sample Y status move of " << oldRealSampleYStatusControl_->value() << " versus " << addOnsSampleVStatusControl_->value() << " and " << addOnsSampleNStatusControl_->value();

	int finalStatus = computeFinalStatus(int(oldRealSampleYStatusControl_->value()), int(oldRealSampleZStatusControl_->value()));

	if(!addOnsSampleVStatusControl_->withinTolerance(finalStatus))
		addOnsSampleVStatusControl_->move(finalStatus);

	if(!addOnsSampleNStatusControl_->withinTolerance(finalStatus))
		addOnsSampleNStatusControl_->move(finalStatus);

	qDebug() << "Detected OLD sample Y status move of " << oldRealSampleYStatusControl_->value() << " versus " << addOnsSampleYStatusControl_->value();

	if(!addOnsSampleYStatusControl_->withinTolerance(oldRealSampleYStatusControl_->value()))
		addOnsSampleYStatusControl_->move(oldRealSampleYStatusControl_->value());
}

void VESPERSAddOnsCoordinator::onOldRealSampleZStatusControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD sample Z status move of " << oldRealSampleZStatusControl_->value() << " versus " << addOnsSampleVStatusControl_->value() << " and " << addOnsSampleNStatusControl_->value();

	int finalStatus = computeFinalStatus(int(oldRealSampleYStatusControl_->value()), int(oldRealSampleZStatusControl_->value()));

	if(!addOnsSampleVStatusControl_->withinTolerance(finalStatus))
		addOnsSampleVStatusControl_->move(finalStatus);

	if(!addOnsSampleNStatusControl_->withinTolerance(finalStatus))
		addOnsSampleNStatusControl_->move(finalStatus);

	qDebug() << "Detected OLD sample Z status move of " << oldRealSampleZStatusControl_->value() << " versus " << addOnsSampleZStatusControl_->value();

	if(!addOnsSampleZStatusControl_->withinTolerance(oldRealSampleZStatusControl_->value()))
		addOnsSampleZStatusControl_->move(oldRealSampleZStatusControl_->value());
}

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

void VESPERSAddOnsCoordinator::onAddOnsAttoHSetpointControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected new AddOns atto H move of " << addOnsAttoHSetpointControl_->value() << " versus " << oldAttoHSetpointControl_->value();

	// If we're requesting the same stage X with AddOns as was in the old stage X AND the old stage X feedback matches the old stage X setpoint AND the status is MOVE DONE
	// THEN issuing a move will not do anything SO we'll trick the status to say "MOVE ACTIVE" then "MOVE DONE"
	if(oldAttoHSetpointControl_->withinTolerance(addOnsAttoHSetpointControl_->value()) && !oldRealAttoXStatusControl_->withinTolerance(1.0)){
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
			&& !oldRealAttoYStatusControl_->withinTolerance(1.0)
			 && !oldRealAttoZStatusControl_->withinTolerance(1.0)){
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
			&& !oldRealAttoYStatusControl_->withinTolerance(1.0)
			 && !oldRealAttoZStatusControl_->withinTolerance(1.0)){
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

void VESPERSAddOnsCoordinator::onOldAttoXSetpointControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD sample X setpoint move of " << oldAttoXSetpointControl_->value() << " versus " << addOnsAttoXSetpointControl_->value();

	if(!addOnsAttoXSetpointControl_->withinTolerance(oldAttoXSetpointControl_->value()))
		addOnsAttoXSetpointControl_->move(oldAttoXSetpointControl_->value());
}

void VESPERSAddOnsCoordinator::onOldAttoXFeedbackControlChanged()
{
	if(!connectedOnce_)
		return;


	if(!addOnsAttoXFeedbackControl_->withinTolerance(oldAttoXFeedbackControl_->value())){
		qDebug() << "Detected OLD sample X feedback move of " << oldAttoXFeedbackControl_->value() << " versus " << addOnsAttoXFeedbackControl_->value();
		addOnsAttoXFeedbackControl_->move(oldAttoXFeedbackControl_->value());
	}
}

void VESPERSAddOnsCoordinator::onOldAttoZSetpointControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD sample Z setpoint move of " << oldAttoZSetpointControl_->value() << " versus " << addOnsAttoZSetpointControl_->value();

	if(!addOnsAttoZSetpointControl_->withinTolerance(oldAttoZSetpointControl_->value()))
		addOnsAttoZSetpointControl_->move(oldAttoZSetpointControl_->value());
}

void VESPERSAddOnsCoordinator::onOldAttoZFeedbackControlChanged()
{
	if(!connectedOnce_)
		return;


	if(!addOnsAttoZFeedbackControl_->withinTolerance(oldAttoZFeedbackControl_->value())){
		qDebug() << "Detected OLD sample Z feedback move of " << oldAttoZFeedbackControl_->value() << " versus " << addOnsAttoZFeedbackControl_->value();
		addOnsAttoZFeedbackControl_->move(oldAttoZFeedbackControl_->value());
	}
}

void VESPERSAddOnsCoordinator::onOldAttoYSetpointControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD sample Y setpoint move of " << oldAttoYSetpointControl_->value() << " versus " << addOnsAttoYSetpointControl_->value();

	if(!addOnsAttoYSetpointControl_->withinTolerance(oldAttoYSetpointControl_->value()))
		addOnsAttoYSetpointControl_->move(oldAttoYSetpointControl_->value());
}

void VESPERSAddOnsCoordinator::onOldAttoYFeedbackControlChanged()
{
	if(!connectedOnce_)
		return;


	if(!addOnsAttoYFeedbackControl_->withinTolerance(oldAttoYFeedbackControl_->value())){
		qDebug() << "Detected OLD sample Y feedback move of " << oldAttoYFeedbackControl_->value() << " versus " << addOnsAttoYFeedbackControl_->value();
		addOnsAttoYFeedbackControl_->move(oldAttoYFeedbackControl_->value());
	}
}

void VESPERSAddOnsCoordinator::onAddOnsAttoXSetpointControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected new AddOns sample X move of " << addOnsAttoXSetpointControl_->value() << " versus " << oldAttoXSetpointControl_->value();

	// If we're requesting the same stage X with AddOns as was in the old stage X AND the old stage X feedback matches the old stage X setpoint AND the status is MOVE DONE
	// THEN issuing a move will not do anything SO we'll trick the status to say "MOVE ACTIVE" then "MOVE DONE"
	if(oldAttoXSetpointControl_->withinTolerance(addOnsAttoXSetpointControl_->value()) && !oldRealAttoXStatusControl_->withinTolerance(1.0)){
		qDebug() << "Faking a sample stage X move";
		addOnsAttoXStatusControl_->move(1.0);
		QTimer::singleShot(50, this, SLOT(restoreAddOnsAttoXStatus()));
		return;
	}

	if(!oldAttoXSetpointControl_->withinTolerance(addOnsAttoXSetpointControl_->value())){
		qDebug() << "Doing addOns Atto X move in the regular way";
		oldAttoXSetpointControl_->move(addOnsAttoXSetpointControl_->value());
	}
}

void VESPERSAddOnsCoordinator::onAddOnsAttoZSetpointControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected new AddOns sample Z move of " << addOnsAttoZSetpointControl_->value() << " versus " << oldAttoZSetpointControl_->value();

	// If we're requesting the same stage X with AddOns as was in the old stage X AND the old stage X feedback matches the old stage X setpoint AND the status is MOZE DONE
	// THEN issuing a move will not do anything SO we'll trick the status to say "MOZE ACTIZE" then "MOZE DONE"
	if(oldAttoZSetpointControl_->withinTolerance(addOnsAttoZSetpointControl_->value()) && !oldRealAttoZStatusControl_->withinTolerance(1.0)){
		qDebug() << "Faking a sample stage Z move";
		addOnsAttoZStatusControl_->move(1.0);
		QTimer::singleShot(50, this, SLOT(restoreAddOnsAttoZStatus()));
		return;
	}

	if(!oldAttoZSetpointControl_->withinTolerance(addOnsAttoZSetpointControl_->value())){
		qDebug() << "Doing addOns Atto Z move in the regular way";
		oldAttoZSetpointControl_->move(addOnsAttoZSetpointControl_->value());
	}
}

void VESPERSAddOnsCoordinator::onAddOnsAttoYSetpointControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected new AddOns sample Y move of " << addOnsAttoYSetpointControl_->value() << " versus " << oldAttoYSetpointControl_->value();

	// If we're requesting the same stage X with AddOns as was in the old stage X AND the old stage X feedback matches the old stage X setpoint AND the status is MOVE DONE
	// THEN issuing a move will not do anything SO we'll trick the status to say "MOVE ACTIVE" then "MOVE DONE"
	if(oldAttoYSetpointControl_->withinTolerance(addOnsAttoYSetpointControl_->value()) && !oldRealAttoYStatusControl_->withinTolerance(1.0)){
		qDebug() << "Faking a sample stage Y move";
		addOnsAttoYStatusControl_->move(1.0);
		QTimer::singleShot(50, this, SLOT(restoreAddOnsAttoYStatus()));
		return;
	}

	if(!oldAttoYSetpointControl_->withinTolerance(addOnsAttoYSetpointControl_->value())){
		qDebug() << "Doing addOns Atto Y move in the regular way";
		oldAttoYSetpointControl_->move(addOnsAttoYSetpointControl_->value());
	}
}

void VESPERSAddOnsCoordinator::restoreAddOnsAttoXStatus()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Restoring AddOns sample stage X status to old VESPERS sample stage X status with AddOns " << addOnsAttoXStatusControl_->value() << " and old X status " << oldRealAttoXStatusControl_->value();

	if(!addOnsAttoXStatusControl_->withinTolerance(oldRealAttoXStatusControl_->value()))
		addOnsAttoXStatusControl_->move(oldRealAttoXStatusControl_->value());
}

void VESPERSAddOnsCoordinator::restoreAddOnsAttoZStatus()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Restoring AddOns sample stage Z status to old VESPERS sample stage Z status with AddOns " << addOnsAttoZStatusControl_->value() << " and old Z status " << oldRealAttoZStatusControl_->value();

	if(!addOnsAttoZStatusControl_->withinTolerance(oldRealAttoZStatusControl_->value()))
		addOnsAttoZStatusControl_->move(oldRealAttoZStatusControl_->value());
}

void VESPERSAddOnsCoordinator::restoreAddOnsAttoYStatus()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Restoring AddOns sample stage Y status to old VESPERS sample stage Y status with AddOns " << addOnsAttoYStatusControl_->value() << " and old Y status " << oldRealAttoYStatusControl_->value();

	if(!addOnsAttoYStatusControl_->withinTolerance(oldRealAttoYStatusControl_->value()))
		addOnsAttoYStatusControl_->move(oldRealAttoYStatusControl_->value());
}

// Atto low level status
/////////////////////////////////////////////

void VESPERSAddOnsCoordinator::onOldRealAttoXStatusControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD sample X status move of " << oldRealAttoXStatusControl_->value() << " versus " << addOnsAttoHStatusControl_->value();

	if(!addOnsAttoHStatusControl_->withinTolerance(oldRealAttoXStatusControl_->value()))
		addOnsAttoHStatusControl_->move(oldRealAttoXStatusControl_->value());

	qDebug() << "Detected OLD sample X status move of " << oldRealAttoXStatusControl_->value() << " versus " << addOnsAttoXStatusControl_->value();

	if(!addOnsAttoXStatusControl_->withinTolerance(oldRealAttoXStatusControl_->value()))
		addOnsAttoXStatusControl_->move(oldRealAttoXStatusControl_->value());
}

void VESPERSAddOnsCoordinator::onOldRealAttoYStatusControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD sample Y status move of " << oldRealAttoYStatusControl_->value() << " versus " << addOnsAttoVStatusControl_->value() << " and " << addOnsAttoNStatusControl_->value();

	int finalStatus = computeFinalStatus(int(oldRealAttoYStatusControl_->value()), int(oldRealAttoZStatusControl_->value()));

	if(!addOnsAttoVStatusControl_->withinTolerance(finalStatus))
		addOnsAttoVStatusControl_->move(finalStatus);

	if(!addOnsAttoNStatusControl_->withinTolerance(finalStatus))
		addOnsAttoNStatusControl_->move(finalStatus);

	qDebug() << "Detected OLD sample Y status move of " << oldRealAttoYStatusControl_->value() << " versus " << addOnsAttoYStatusControl_->value();

	if(!addOnsAttoYStatusControl_->withinTolerance(oldRealAttoYStatusControl_->value()))
		addOnsAttoYStatusControl_->move(oldRealAttoYStatusControl_->value());
}

void VESPERSAddOnsCoordinator::onOldRealAttoZStatusControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD sample Y status move of " << oldRealAttoYStatusControl_->value() << " versus " << addOnsAttoVStatusControl_->value() << " and " << addOnsAttoNStatusControl_->value();

	int finalStatus = computeFinalStatus(int(oldRealAttoYStatusControl_->value()), int(oldRealAttoZStatusControl_->value()));

	if(!addOnsAttoVStatusControl_->withinTolerance(finalStatus))
		addOnsAttoVStatusControl_->move(finalStatus);

	if(!addOnsAttoNStatusControl_->withinTolerance(finalStatus))
		addOnsAttoNStatusControl_->move(finalStatus);

	qDebug() << "Detected OLD sample Z status move of " << oldRealAttoZStatusControl_->value() << " versus " << addOnsAttoZStatusControl_->value();

	if(!addOnsAttoZStatusControl_->withinTolerance(oldRealAttoZStatusControl_->value()))
		addOnsAttoZStatusControl_->move(oldRealAttoZStatusControl_->value());
}

// Atto Rotation Rx, Rz, Ry
//////////////////////////////////////////////

void VESPERSAddOnsCoordinator::onOldAttoRxSetpointControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD atto Rx setpoint move of " << oldAttoRxSetpointControl_->value() << " versus " << addOnsAttoRxSetpointControl_->value();

	if(!addOnsAttoRxSetpointControl_->withinTolerance(oldAttoRxSetpointControl_->value()))
		addOnsAttoRxSetpointControl_->move(oldAttoRxSetpointControl_->value());
}

void VESPERSAddOnsCoordinator::onOldAttoRxFeedbackControlChanged()
{
	if(!connectedOnce_)
		return;


	if(!addOnsAttoRxFeedbackControl_->withinTolerance(oldAttoRxFeedbackControl_->value())){
		qDebug() << "Detected OLD atto Rx feedback move of " << oldAttoRxFeedbackControl_->value() << " versus " << addOnsAttoRxFeedbackControl_->value();
		addOnsAttoRxFeedbackControl_->move(oldAttoRxFeedbackControl_->value());
	}
}

void VESPERSAddOnsCoordinator::onOldAttoRzSetpointControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD atto Rz setpoint move of " << oldAttoRzSetpointControl_->value() << " versus " << addOnsAttoRzSetpointControl_->value();

	if(!addOnsAttoRzSetpointControl_->withinTolerance(oldAttoRzSetpointControl_->value()))
		addOnsAttoRzSetpointControl_->move(oldAttoRzSetpointControl_->value());
}

void VESPERSAddOnsCoordinator::onOldAttoRzFeedbackControlChanged()
{
	if(!connectedOnce_)
		return;

	if(!addOnsAttoRzFeedbackControl_->withinTolerance(oldAttoRzFeedbackControl_->value())){
		qDebug() << "Detected OLD atto Rz feedback move of " << oldAttoRzFeedbackControl_->value() << " versus " << addOnsAttoRzFeedbackControl_->value();
		addOnsAttoRzFeedbackControl_->move(oldAttoRzFeedbackControl_->value());
	}
}

void VESPERSAddOnsCoordinator::onOldAttoRySetpointControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD atto Ry setpoint move of " << oldAttoRySetpointControl_->value() << " versus " << addOnsAttoRySetpointControl_->value();

	if(!addOnsAttoRySetpointControl_->withinTolerance(oldAttoRySetpointControl_->value()))
		addOnsAttoRySetpointControl_->move(oldAttoRySetpointControl_->value());
}

void VESPERSAddOnsCoordinator::onOldAttoRyFeedbackControlChanged()
{
	if(!connectedOnce_)
		return;


	if(!addOnsAttoRyFeedbackControl_->withinTolerance(oldAttoRyFeedbackControl_->value())){
		qDebug() << "Detected OLD atto Ry feedback move of " << oldAttoRyFeedbackControl_->value() << " versus " << addOnsAttoRyFeedbackControl_->value();
		addOnsAttoRyFeedbackControl_->move(oldAttoRyFeedbackControl_->value());
	}
}

void VESPERSAddOnsCoordinator::onAddOnsAttoRxSetpointControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected new AddOns atto Rx move of " << addOnsAttoRxSetpointControl_->value() << " versus " << oldAttoRxSetpointControl_->value();

	// If we're requesting the same stage Rx with AddOns as was in the old stage Rx AND the old stage Rx feedback matches the old stage Rx setpoint AND the status is MOVE DONE
	// THEN issuing a move will not do anything SO we'll trick the status to say "MOVE ACTIVE" then "MOVE DONE"
	if(oldAttoRxSetpointControl_->withinTolerance(addOnsAttoRxSetpointControl_->value()) && !oldRealAttoRxStatusControl_->withinTolerance(1.0)){
		qDebug() << "Faking a atto stage Rx move";
		addOnsAttoRxStatusControl_->move(1.0);
		QTimer::singleShot(50, this, SLOT(restoreAddOnsAttoRxStatus()));
		return;
	}

	if(!oldAttoRxSetpointControl_->withinTolerance(addOnsAttoRxSetpointControl_->value())){
		qDebug() << "Doing addOns Atto Rx move in the regular way";
		oldAttoRxSetpointControl_->move(addOnsAttoRxSetpointControl_->value());
	}
}

void VESPERSAddOnsCoordinator::onAddOnsAttoRzSetpointControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected new AddOns atto Rz move of " << addOnsAttoRzSetpointControl_->value() << " versus " << oldAttoRzSetpointControl_->value();

	// If we're requesting the same stage Rz with AddOns as was in the old stage Rz AND the old stage Rz feedback matches the old stage Rz setpoint AND the status is MOZE DONE
	// THEN issuing a move will not do anything SO we'll trick the status to say "MOZE ACTIZE" then "MOZE DONE"
	if(oldAttoRzSetpointControl_->withinTolerance(addOnsAttoRzSetpointControl_->value()) && !oldRealAttoRzStatusControl_->withinTolerance(1.0)){
		qDebug() << "Faking a atto stage Rz move";
		addOnsAttoRzStatusControl_->move(1.0);
		QTimer::singleShot(50, this, SLOT(restoreAddOnsAttoRzStatus()));
		return;
	}

	if(!oldAttoRzSetpointControl_->withinTolerance(addOnsAttoRzSetpointControl_->value())){
		qDebug() << "Doing addOns Atto Rz move in the regular way";
		oldAttoRzSetpointControl_->move(addOnsAttoRzSetpointControl_->value());
	}
}

void VESPERSAddOnsCoordinator::onAddOnsAttoRySetpointControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected new AddOns atto Ry move of " << addOnsAttoRySetpointControl_->value() << " versus " << oldAttoRySetpointControl_->value();

	// If we're requesting the same stage Ry with AddOns as was in the old stage Ry AND the old stage Ry feedback matches the old stage Ry setpoint AND the status is MOVE DONE
	// THEN issuing a move will not do anything SO we'll trick the status to say "MOVE ACTIVE" then "MOVE DONE"
	if(oldAttoRySetpointControl_->withinTolerance(addOnsAttoRySetpointControl_->value()) && !oldRealAttoRyStatusControl_->withinTolerance(1.0)){
		qDebug() << "Faking a atto stage Ry move";
		addOnsAttoRyStatusControl_->move(1.0);
		QTimer::singleShot(50, this, SLOT(restoreAddOnsAttoRyStatus()));
		return;
	}

	if(!oldAttoRySetpointControl_->withinTolerance(addOnsAttoRySetpointControl_->value())){
		qDebug() << "Doing addOns Atto Ry move in the regular way";
		oldAttoRySetpointControl_->move(addOnsAttoRySetpointControl_->value());
	}
}

void VESPERSAddOnsCoordinator::restoreAddOnsAttoRxStatus()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Restoring AddOns atto stage Rx status to old VESPERS atto stage Rx status with AddOns " << addOnsAttoRxStatusControl_->value() << " and old Rx status " << oldRealAttoRxStatusControl_->value();

	if(!addOnsAttoRxStatusControl_->withinTolerance(oldRealAttoRxStatusControl_->value()))
		addOnsAttoRxStatusControl_->move(oldRealAttoRxStatusControl_->value());
}

void VESPERSAddOnsCoordinator::restoreAddOnsAttoRzStatus()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Restoring AddOns atto stage Rz status to old VESPERS atto stage Rz status with AddOns " << addOnsAttoRzStatusControl_->value() << " and old Rz status " << oldRealAttoRzStatusControl_->value();

	if(!addOnsAttoRzStatusControl_->withinTolerance(oldRealAttoRzStatusControl_->value()))
		addOnsAttoRzStatusControl_->move(oldRealAttoRzStatusControl_->value());
}

void VESPERSAddOnsCoordinator::restoreAddOnsAttoRyStatus()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Restoring AddOns atto stage Ry status to old VESPERS atto stage Ry status with AddOns " << addOnsAttoRyStatusControl_->value() << " and old Ry status " << oldRealAttoRyStatusControl_->value();

	if(!addOnsAttoRyStatusControl_->withinTolerance(oldRealAttoRyStatusControl_->value()))
		addOnsAttoRyStatusControl_->move(oldRealAttoRyStatusControl_->value());
}

// Atto low level status
/////////////////////////////////////////////

void VESPERSAddOnsCoordinator::onOldRealAttoRxStatusControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD atto Rx status move of " << oldRealAttoRxStatusControl_->value() << " versus " << addOnsAttoRxStatusControl_->value();

	if(!addOnsAttoRxStatusControl_->withinTolerance(oldRealAttoRxStatusControl_->value()))
		addOnsAttoRxStatusControl_->move(oldRealAttoRxStatusControl_->value());
}

void VESPERSAddOnsCoordinator::onOldRealAttoRyStatusControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD atto Ry status move of " << oldRealAttoRyStatusControl_->value() << " versus " << addOnsAttoRyStatusControl_->value();

	if(!addOnsAttoRyStatusControl_->withinTolerance(oldRealAttoRyStatusControl_->value()))
		addOnsAttoRyStatusControl_->move(oldRealAttoRyStatusControl_->value());
}

void VESPERSAddOnsCoordinator::onOldRealAttoRzStatusControlChanged()
{
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD atto Rz status move of " << oldRealAttoRzStatusControl_->value() << " versus " << addOnsAttoRzStatusControl_->value();

	if(!addOnsAttoRzStatusControl_->withinTolerance(oldRealAttoRzStatusControl_->value()))
		addOnsAttoRzStatusControl_->move(oldRealAttoRzStatusControl_->value());
}

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
	if(oldWireHSetpointControl_->withinTolerance(addOnsWireHSetpointControl_->value()) && !oldRealWireXStatusControl_->withinTolerance(1.0)){
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
			&& !oldRealWireYStatusControl_->withinTolerance(1.0)
			 && !oldRealWireZStatusControl_->withinTolerance(1.0)){
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
			&& !oldRealWireYStatusControl_->withinTolerance(1.0)
			 && !oldRealWireZStatusControl_->withinTolerance(1.0)){
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
