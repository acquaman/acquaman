#include "VESPERSCCDDetector.h"

#include "actions/AMBeamlineControlMoveAction.h"
#include "actions/VESPERS/VESPERSBeamlineSetStringAction.h"

#include <QStringBuilder>

VESPERSCCDDetector::VESPERSCCDDetector(const QString &name, const QString &description, const QString &pvBase, const AMnDIndex &detectorSize, QObject *parent)
	: VESPERSCCDDetectorInfo(name, description, detectorSize, parent), AMOldDetector(name)
{
	imageModeControl_ = new AMPVControl("Image Mode", pvBase % ":ImageMode_RBV", "ccd1607-002:cam1:ImageMode", QString(), this, 0.1);
	triggerModeControl_ = new AMPVControl("Trigger Mode", pvBase % ":TriggerMode_RBV", "ccd1607-002:cam1:TriggerMode", QString(), this, 0.1);
	operationControl_ = new AMSinglePVControl("Operation", pvBase % ":Acquire", this, 0.1);
	stateControl_ = new AMReadOnlyPVControl("State", pvBase % ":DetectorState_RBV", this);
	acquireTimeControl_ = new AMSinglePVControl("Acquire Time", pvBase % ":AcquireTime", this, 0.1);
	autoSaveControl_ = new AMPVControl("AutoSave", pvBase % ":AutoSave_RBV", "ccd1607-002:cam1:AutoSave", QString(), this, 0.1);
	saveFileControl_ = new AMPVwStatusControl("Save File", pvBase % ":WriteFile", "ccd1607-002:cam1:WriteFile", "ccd1607-002:cam1:WriteFile_RBV", QString(), this, 0.1);

	// Various CCD file path PVs.
	ccdPath_ = new AMProcessVariable(pvBase % ":FilePath", true, this);
	ccdFile_ = new AMProcessVariable(pvBase % ":FileName", true, this);
	ccdNumber_ = new AMSinglePVControl("File Number",pvBase % ":FileNumber", this);

	connect(signalSource(), SIGNAL(connected(bool)), this, SIGNAL(connected(bool)));
	connect(imageModeControl_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()));
	connect(triggerModeControl_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()));
	connect(operationControl_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()));
	connect(stateControl_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()));
	connect(acquireTimeControl_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()));
	connect(autoSaveControl_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()));
	connect(saveFileControl_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()));
	connect(ccdPath_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()));
	connect(ccdFile_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()));
	connect(ccdNumber_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()));

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

void VESPERSCCDDetector::onConnectedChanged()
{
	if (isConnected() && !allControlsConnected())
		setConnected(false);

	else if (!isConnected() && allControlsConnected())
		setConnected(true);
}

bool VESPERSCCDDetector::allControlsConnected() const
{
	return operationControl_->isConnected()
			&& stateControl_->isConnected()
			&& acquireTimeControl_->isConnected()
			&& ccdPath_->isConnected()
			&& ccdFile_->isConnected()
			&& ccdNumber_->isConnected();
}

VESPERSCCDDetector::ImageMode VESPERSCCDDetector::imageMode() const
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

VESPERSCCDDetector::TriggerMode VESPERSCCDDetector::triggerMode() const
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

VESPERSCCDDetector::State VESPERSCCDDetector::state() const
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

bool VESPERSCCDDetector::setFromInfo(const AMOldDetectorInfo *info)
{
	const VESPERSCCDDetectorInfo *detectorInfo = qobject_cast<const VESPERSCCDDetectorInfo *>(info);

	// Check to make sure the detector info was valid.  If it isn't, then don't do anything to the detector.
	if (!detectorInfo)
		return false;

	setAcquireTime(detectorInfo->acquireTime());

	return true;
}

void VESPERSCCDDetector::setFromVESPERSInfo(const VESPERSCCDDetectorInfo &info)
{
	setFromInfo(&info);
}

AMBeamlineActionItem *VESPERSCCDDetector::createAcquireTimeAction(double time)
{
	if (!acquireTimeControl_->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(acquireTimeControl_);
	action->setSetpoint(time);

	return action;
}

AMBeamlineActionItem *VESPERSCCDDetector::createImageModeAction(VESPERSCCDDetector::ImageMode mode)
{
	if (!imageModeControl_->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(imageModeControl_);
	action->setSetpoint(int(mode));

	return action;
}

AMBeamlineActionItem *VESPERSCCDDetector::createTriggerModeAction(VESPERSCCDDetector::TriggerMode mode)
{
	if (!triggerModeControl_->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(triggerModeControl_);
	action->setSetpoint(int(mode));

	return action;
}

AMBeamlineActionItem *VESPERSCCDDetector::createStartAction()
{
	if (!operationControl_->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(operationControl_);
	action->setSetpoint(1);

	return action;
}

AMBeamlineActionItem *VESPERSCCDDetector::createStopAction()
{
	if (!operationControl_->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(operationControl_);
	action->setSetpoint(0);

	return action;
}

AMBeamlineActionItem *VESPERSCCDDetector::createAutoSaveAction(bool autoSave)
{
	if (!autoSaveControl_->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(autoSaveControl_);
	action->setSetpoint(autoSave ? 1 : 0);

	return action;
}

AMBeamlineActionItem *VESPERSCCDDetector::createSaveFileAction()
{
	if (!saveFileControl_->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(saveFileControl_);
	action->setSetpoint(1);

	return action;
}

AMBeamlineActionItem *VESPERSCCDDetector::createFilePathAction(const QString &path)
{
	if (!ccdPath_->isConnected())
		return 0;

	return new VESPERSBeamlineSetStringAction(ccdPath_, path);
}

AMBeamlineActionItem *VESPERSCCDDetector::createFileNameAction(const QString &name)
{
	if (!ccdFile_->isConnected())
		return 0;

	return new VESPERSBeamlineSetStringAction(ccdFile_, name);
}

AMBeamlineActionItem *VESPERSCCDDetector::createFileNumberAction(int number)
{
	if (!ccdNumber_->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(ccdNumber_);
	action->setSetpoint(number);

	return action;
}
