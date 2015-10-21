#include "BioXASSideBeamStatusControl.h"

BioXASSideBeamStatusControl::BioXASSideBeamStatusControl(QObject *parent) :
	BioXASBeamStatusControl("BioXASSideBeamStatus", parent)
{
	// Initialize inherited settings.

	setContextKnownDescription("SideBeamStatus");

	// Initialize class variables.

	frontEndBeamStatus_ = 0;
	preMirrorMask_ = 0;
	preMonoMaskUpperBlade_ = 0;
	preMonoMaskLowerBlade_ = 0;

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
		result = (frontEndBeamStatus_->shouldStop() && preMirrorMask_->shouldStop() && preMonoMaskUpperBlade_->shouldStop() && preMonoMaskLowerBlade_->shouldStop());

	return result;
}

bool BioXASSideBeamStatusControl::canMeasure() const
{
	bool result = false;

	if (isConnected())
		result = (frontEndBeamStatus_->canMeasure() && preMirrorMask_->canMeasure() && preMonoMaskUpperBlade_->canMeasure() && preMonoMaskLowerBlade_->canMeasure());

	return result;
}

bool BioXASSideBeamStatusControl::canMove() const
{
	bool result = false;

	if (isConnected())
		result = (frontEndBeamStatus_->canMove() && preMirrorMask_->canMove() && preMonoMask_->canMove());

	return result;
}

bool BioXASSideBeamStatusControl::canStop() const
{
	bool result = false;

	if (isConnected())
		result = (frontEndBeamStatus_->canStop() && preMirrorMask_->canStop() && preMonoMaskUpperBlade_->canStop() && preMonoMaskLowerBlade_->canStop());

	return result;
}

bool BioXASSideBeamStatusControl::isOn() const
{
	bool result = false;

	if (isConnected())
		result = (frontEndBeamStatus_->isOn() && (preMirrorMask_->value() > 0) && (preMonoMaskUpperBlade_->value() != preMonoMaskLowerBlade_->value()));

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

void BioXASSideBeamStatusControl::setPreMonoMaskUpperBladeControl(AMControl *newControl)
{
	if (preMonoMaskUpperBlade_ != newControl) {

		if (preMonoMaskUpperBlade_)
			removeChildControl(preMonoMaskUpperBlade_);

		preMonoMaskUpperBlade_ = newControl;

		if (preMonoMaskUpperBlade_)
			addChildControl(preMonoMaskUpperBlade_);

		emit preMonoMaskUpperBladeControlChanged(preMonoMaskUpperBlade_);
	}
}

void BioXASSideBeamStatusControl::setPreMonoMaskLowerBladeControl(AMControl *newControl)
{
	if (preMonoMaskLowerBlade_ != newControl) {

		if (preMonoMaskLowerBlade_)
			removeChildControl(preMonoMaskLowerBlade_);

		preMonoMaskLowerBlade_ = newControl;

		if (preMonoMaskLowerBlade_)
			addChildControl(preMonoMaskLowerBlade_);

		emit preMonoMaskLowerBladeControlChanged(preMonoMaskLowerBlade_);
	}
}

void BioXASSideBeamStatusControl::updateConnected()
{
	bool isConnected = (
				frontEndBeamStatus_ && frontEndBeamStatus_->isConnected() &&
				preMirrorMask_ && preMirrorMask_->isConnected() &&
				preMonoMaskUpperBlade_ && preMonoMaskUpperBlade_->isConnected() &&
				preMonoMaskLowerBlade_ && preMonoMaskLowerBlade_->isConnected()
				);

	setConnected(isConnected);
}

void BioXASSideBeamStatusControl::updateMoving()
{
	if (isConnected()) {
		bool isMoving = (
					frontEndBeamStatus_->isMoving() ||
					preMirrorMask_->isMoving() ||
					preMonoMaskUpperBlade_->isMoving() ||
					preMonoMaskLowerBlade_->isMoving()
					);

		setIsMoving(isMoving);
	}
}
