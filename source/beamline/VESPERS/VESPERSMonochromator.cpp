/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "VESPERSMonochromator.h"

VESPERSMonochromator::VESPERSMonochromator(QObject *parent) :
	QObject(parent)
{
	Eo_ = new AMPVwStatusControl("Energy Setpoint", "07B2_Mono_SineB_Egec:eV", "07B2_Mono_SineB_Eo", "SMTR1607-1-B20-20:status", QString(), this);
	energy_ = new AMPVwStatusControl("Acutal Energy", "07B2_Mono_SineB_Egec:eV", "07B2_Mono_SineB_Ea", "SMTR1607-1-B20-20:status", QString(), this, 0.5);
	delE_ = new AMPVwStatusControl("Relative Energy", "07B2_Mono_SineB_deltaE:fbk", "07B2_Mono_SineB_delE", "SMTR1607-1-B20-20:status", QString(), this, 0.5);
	K_ = new AMPVwStatusControl("K-space", "07B2_Mono_SineB_K:fbk", "07B2_Mono_SineB_K", "SMTR1607-1-B20-20:status", QString(), this, 0.01);
	offsetAngle_ = new AMSinglePVControl("Offset Angle", "07B2_Mono_SineB_ThOS", this, 0.01);
	allowScan_ = new AMSinglePVControl("Scan Allow Control", "07B2_Mono_ScanSineB", this, 0.1);
	encoder_ = new AMSinglePVControl("Energy Encoder Precision", "07B2_Mono_SineB_Use_eV", this,  0.1);

	connect(Eo_, SIGNAL(setpointChanged(double)), this, SIGNAL(EoChanged(double)));
	connect(energy_, SIGNAL(setpointChanged(double)), this, SIGNAL(EaChanged(double)));
	connect(energy_, SIGNAL(valueChanged(double)), this, SIGNAL(energyChanged(double)));
	connect(delE_, SIGNAL(valueChanged(double)), this, SIGNAL(delEChanged(double)));
	connect(K_, SIGNAL(setpointChanged(double)), this, SIGNAL(KSetpointChanged(double)));
	connect(K_, SIGNAL(valueChanged(double)), this, SIGNAL(KFeedbackChanged(double)));
	connect(offsetAngle_, SIGNAL(valueChanged(double)), this, SIGNAL(offsetAngleChanged(double)));
	connect(allowScan_, SIGNAL(valueChanged(double)), this, SLOT(onAllowScanChanged(double)));
	connect(encoder_, SIGNAL(valueChanged(double)), this, SLOT(onEncoderChanged(double)));
}

AMBeamlineActionItem *VESPERSMonochromator::createEoAction(double energy)
{
	if (!Eo_->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(Eo_);
	action->setSetpoint(energy);

	return action;
}

AMBeamlineActionItem *VESPERSMonochromator::createEaAction(double energy)
{
	if (!energy_->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(energy_);
	action->setSetpoint(energy);

	return action;
}

AMBeamlineActionItem *VESPERSMonochromator::createDelEAction(double energy)
{
	if (!delE_->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(delE_);
	action->setSetpoint(energy);

	return action;
}

AMBeamlineActionItem *VESPERSMonochromator::createKAction(double k)
{
	if (!K_->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(K_);
	action->setSetpoint(k);

	return action;
}

AMBeamlineActionItem *VESPERSMonochromator::createOffsetAngleAction(double angle)
{
	if (!offsetAngle_->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(offsetAngle_);
	action->setSetpoint(angle);

	return action;
}

AMBeamlineActionItem *VESPERSMonochromator::createAllowScanningAction(bool allow)
{
	if (!allowScan_->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(allowScan_);
	action->setSetpoint((allow == true) ? 1.0 : 0.0);

	return action;
}

AMBeamlineActionItem *VESPERSMonochromator::createUsingeVAction(bool useeV)
{
	if (!encoder_->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(encoder_);
	action->setSetpoint((useeV == true) ? 1.0 : 0.0);

	return action;
}
