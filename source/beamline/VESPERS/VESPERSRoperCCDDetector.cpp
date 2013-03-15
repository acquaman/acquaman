/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "VESPERSRoperCCDDetector.h"

#include "actions/AMBeamlineControlMoveAction.h"
#include "actions/VESPERS/VESPERSBeamlineSetStringAction.h"

VESPERSRoperCCDDetector::VESPERSRoperCCDDetector(const QString &name, const QString &description, QObject *parent)
	: VESPERSRoperCCDDetectorInfo(name, description, parent), AMDetector(name)
{
	temperatureControl_ = new AMPVControl("Temperature", "IOC1607-003:det1:Temperature_RBV", "IOC1607-003:det1:Temperature", QString(), this, 1);
	imageModeControl_ = new AMPVControl("Image Mode", "IOC1607-003:det1:ImageMode_RBV", "IOC1607-003:det1:ImageMode", QString(), this, 0.1);
	triggerModeControl_ = new AMPVControl("Trigger Mode", "IOC1607-003:det1:TriggerMode_RBV", "IOC1607-003:det1:TriggerMode", QString(), this, 0.1);
	operationControl_ = new AMSinglePVControl("Operation", "IOC1607-003:det1:Acquire", this, 0.1);
	stateControl_ = new AMReadOnlyPVControl("State", "IOC1607-003:det1:DetectorState_RBV", this);
	acquireTimeControl_ = new AMSinglePVControl("Acquire Time", "IOC1607-003:det1:AcquireTime", this, 0.1);
	autoSaveControl_ = new AMPVControl("AutoSave", "IOC1607-003:det1:AutoSave_RBV", "IOC1607-003:det1:AutoSave", QString(), this, 0.1);
	saveFileControl_ = new AMPVwStatusControl("Save File", "IOC1607-003:det1:WriteFile", "IOC1607-003:det1:WriteFile", "IOC1607-003:det1:WriteFile_RBV", QString(), this, 0.1);

	// Various CCD file path PVs.
	ccdPath_ = new AMProcessVariable("IOC1607-003:det1:FilePath", true, this);
	ccdFile_ = new AMProcessVariable("IOC1607-003:det1:FileName", true, this);
	ccdNumber_ = new AMSinglePVControl("File Number", "IOC1607-003:det1:FileNumber", this);

	connect(signalSource(), SIGNAL(connected(bool)), this, SIGNAL(connected(bool)));
	connect(temperatureControl_, SIGNAL(valueChanged(double)), this, SIGNAL(temperatureChanged(double)));
	connect(temperatureControl_, SIGNAL(setpointChanged(double)), this, SLOT(onTemperatureSetpointChanged(double)));
	connect(imageModeControl_, SIGNAL(valueChanged(double)), this, SLOT(onImageModeChanged()));
	connect(triggerModeControl_, SIGNAL(valueChanged(double)), this, SLOT(onTriggerModeChanged()));
	connect(operationControl_, SIGNAL(valueChanged(double)), this, SLOT(onIsAcquiringChanged()));
	connect(stateControl_, SIGNAL(valueChanged(double)), this, SLOT(onStateChanged()));
	connect(acquireTimeControl_, SIGNAL(valueChanged(double)), this, SLOT(onAcquireTimeChanged(double)));
	connect(autoSaveControl_, SIGNAL(valueChanged(double)), this, SLOT(onAutoSaveEnabledChanged()));
	connect(saveFileControl_, SIGNAL(valueChanged(double)), this, SLOT(onSaveFileStateChanged()));

	connect(ccdPath_, SIGNAL(valueChanged()), this, SLOT(onCCDPathChanged()));
	connect(ccdFile_, SIGNAL(valueChanged()), this, SLOT(onCCDNameChanged()));
	connect(ccdNumber_, SIGNAL(valueChanged(double)), this, SLOT(onCCDNumberChanged()));
}

VESPERSRoperCCDDetector::ImageMode VESPERSRoperCCDDetector::imageMode() const
{
	ImageMode mode = Normal;

	switch((int)imageModeControl_->value()){

	case 0:
		mode = Normal;
		break;

	case 1:
		mode = Continuous;
		break;

	case 2:
		mode = Focus;
		break;
	}

	return mode;
}

VESPERSRoperCCDDetector::TriggerMode VESPERSRoperCCDDetector::triggerMode() const
{
	TriggerMode mode = FreeRun;

	switch((int)triggerModeControl_->value()){

	case 0:
		mode = FreeRun;
		break;

	case 1:
		mode = ExtSync;
		break;

	case 2:
		mode = BulbTrigger;
		break;

	case 3:
		mode = SingleTrigger;
		break;
	}

	return mode;
}

VESPERSRoperCCDDetector::State VESPERSRoperCCDDetector::state() const
{
	State detectorState = Idle;

	switch((int)stateControl_->value()){

	case 0:
		detectorState = Idle;
		break;

	case 1:
		detectorState = Acquire;
		break;

	case 2:
		detectorState = Readout;
		break;

	case 3:
		detectorState = Correct;
		break;

	case 4:
		detectorState = Saving;
		break;

	case 5:
		detectorState = Aborting;
		break;

	case 6:
		detectorState = Error;
		break;

	case 7:
		detectorState = Waiting;
		break;
	}

	return detectorState;
}

bool VESPERSRoperCCDDetector::setFromInfo(const AMDetectorInfo *info)
{
	const VESPERSRoperCCDDetectorInfo *detectorInfo = qobject_cast<const VESPERSRoperCCDDetectorInfo *>(info);

	// Check to make sure the detector info was valid.  If it isn't, then don't do anything to the detector.
	if (!detectorInfo)
		return false;

	setTemperature(detectorInfo->temperature());
	setAcquireTime(detectorInfo->acquireTime());

	return true;
}

void VESPERSRoperCCDDetector::setFromRoperInfo(const VESPERSRoperCCDDetectorInfo &info)
{
	setTemperature(info.temperature());
	setAcquireTime(info.acquireTime());
}

AMBeamlineActionItem *VESPERSRoperCCDDetector::createAcquireTimeAction(double time)
{
	if (!acquireTimeControl_->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(acquireTimeControl_);
	action->setSetpoint(time);

	return action;
}

AMBeamlineActionItem *VESPERSRoperCCDDetector::createImageModeAction(VESPERSRoperCCDDetector::ImageMode mode)
{
	if (!imageModeControl_->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(imageModeControl_);
	action->setSetpoint(int(mode));

	return action;
}

AMBeamlineActionItem *VESPERSRoperCCDDetector::createTriggerModeAction(VESPERSRoperCCDDetector::TriggerMode mode)
{
	if (!triggerModeControl_->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(triggerModeControl_);
	action->setSetpoint(int(mode));

	return action;
}

AMBeamlineActionItem *VESPERSRoperCCDDetector::createTemperatureAction(double temperature)
{
	if (!temperatureControl_->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(temperatureControl_);
	action->setSetpoint(temperature);

	return action;
}

AMBeamlineActionItem *VESPERSRoperCCDDetector::createStartAction()
{
	if (!operationControl_->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(operationControl_);
	action->setSetpoint(1);

	return action;
}

AMBeamlineActionItem *VESPERSRoperCCDDetector::createStopAction()
{
	if (!operationControl_->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(operationControl_);
	action->setSetpoint(0);

	return action;
}

AMBeamlineActionItem *VESPERSRoperCCDDetector::createAutoSaveAction(bool autoSave)
{
	if (!autoSaveControl_->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(autoSaveControl_);
	action->setSetpoint(autoSave ? 1 : 0);

	return action;
}

AMBeamlineActionItem *VESPERSRoperCCDDetector::createSaveFileAction()
{
	if (!saveFileControl_->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(saveFileControl_);
	action->setSetpoint(1);

	return action;
}

AMBeamlineActionItem *VESPERSRoperCCDDetector::createFilePathAction(const QString &path)
{
	if (!ccdPath_->isConnected())
		return 0;

	return new VESPERSBeamlineSetStringAction(ccdPath_, path);
}

AMBeamlineActionItem *VESPERSRoperCCDDetector::createFileNameAction(const QString &name)
{
	if (!ccdFile_->isConnected())
		return 0;

	return new VESPERSBeamlineSetStringAction(ccdFile_, name);
}

AMBeamlineActionItem *VESPERSRoperCCDDetector::createFileNumberAction(int number)
{
	if (!ccdNumber_->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(ccdNumber_);
	action->setSetpoint(number);

	return action;
}
