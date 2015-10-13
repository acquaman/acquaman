#include "BioXASSideBeamStatusControl.h"

BioXASSideBeamStatusControl::BioXASSideBeamStatusControl(QObject *parent) :
	BioXASBeamStatusControl("BioXASSideBeamStatus", parent)
{
	// Initialize class variables.

	frontEndBeamStatus_ = 0;
	preMirrorMask_ = 0;
	preMonoMask_ = 0;

	// Set inherited settings.

	setContextKnownDescription("SideBeamStatus");

	// Current settings.

	updateStates();
}

BioXASSideBeamStatusControl::~BioXASSideBeamStatusControl()
{

}

bool BioXASSideBeamStatusControl::shouldStop() const
{
	bool result = false;

	if (isConnected())
		result = (frontEndBeamStatus_->shouldStop() && preMirrorMask_->shouldStop() && preMonoMask_->shouldStop());

	return result;
}

bool BioXASSideBeamStatusControl::canMeasure() const
{
	bool result = false;

	if (isConnected())
		result = (frontEndBeamStatus_->canMeasure() && preMirrorMask_->canMeasure() && preMonoMask_->canMeasure());

	return result;
}

//bool BioXASSideBeamStatusControl::canMove() const
//{
//	bool result = false;

//	if (isConnected())
//		result = (frontEndBeamStatus_->canMove() && preMirrorMask_->canMove() && preMonoMask_->canMove());

//	return result;
//}

bool BioXASSideBeamStatusControl::canStop() const
{
	bool result = false;

	if (isConnected())
		result = (frontEndBeamStatus_->canStop() && preMirrorMask_->canStop() && preMonoMask_->canStop());

	return result;
}

bool BioXASSideBeamStatusControl::isOn() const
{
	bool result = false;

	if (isConnected())
		result = (frontEndBeamStatus_->isOn() && (preMirrorMask_->value() > 0) && (preMonoMask_->value() > 0));

	return result;
}

void BioXASSideBeamStatusControl::setFrontEndBeamStatusControl(BioXASFrontEndBeamStatusControl *newControl)
{
	if (frontEndBeamStatus_ != newControl) {

		if (frontEndBeamStatus_)
			removeChildControl(frontEndBeamStatus_);

		frontEndBeamStatus_ = newControl;

		if (frontEndBeamStatus_)
			addChildControl(frontEndBeamStatus_);

		emit frontEndBeamStatusControlChanged(frontEndBeamStatus_);
	}
}

void BioXASSideBeamStatusControl::setPreMirrorMaskControl(AMControl *newControl)
{
	if (preMirrorMask_ != newControl) {

		if (preMirrorMask_)
			removeChildControl(preMirrorMask_);

		preMirrorMask_ = newControl;

		if (preMirrorMask_)
			addChildControl(preMirrorMask_);

		emit preMirrorMaskControlChanged(preMirrorMask_);
	}
}

void BioXASSideBeamStatusControl::setPreMonoMaskControl(AMControl *newControl)
{
	if (preMonoMask_ != newControl) {

		if (preMonoMask_)
			removeChildControl(preMirrorMask_);

		preMirrorMask_ = newControl;

		if (preMirrorMask_)
			addChildControl(preMirrorMask_);

		emit preMirrorMaskControlChanged(preMirrorMask_);
	}
}

void BioXASSideBeamStatusControl::updateConnected()
{
	bool isConnected = (
				frontEndBeamStatus_ && frontEndBeamStatus_->isConnected() &&
				preMirrorMask_ && preMirrorMask_->isConnected() &&
				preMonoMask_ && preMonoMask_->isConnected()
				);

	setConnected(isConnected);
}

void BioXASSideBeamStatusControl::updateMoving()
{
	if (isConnected()) {
		bool isMoving = (
					frontEndBeamStatus_->isMoving() ||
					preMirrorMask_->isMoving() ||
					preMonoMask_->isMoving()
					);

		setIsMoving(isMoving);
	}
}
