#include "BioXASSidePOEBeamStatusControl.h"

BioXASSidePOEBeamStatusControl::BioXASSidePOEBeamStatusControl(const QString &name, QObject *parent) :
	BioXASBeamStatusControl(name, parent)
{
	// Initialize inherited settings.

	setContextKnownDescription("SideBeamStatus");

	// Initialize class variables.

	frontEndBeamStatus_ = 0;
	mirrorMask_ = 0;
	monoMask_ = 0;

	// Current settings.

	updateStates();
}

BioXASSidePOEBeamStatusControl::~BioXASSidePOEBeamStatusControl()
{

}

bool BioXASSidePOEBeamStatusControl::shouldStop() const
{
	bool result = false;

	if (isConnected())
		result = (frontEndBeamStatus_->shouldStop() && mirrorMask_->shouldStop());

	return result;
}

bool BioXASSidePOEBeamStatusControl::canMeasure() const
{
	bool result = false;

	if (isConnected())
		result = (frontEndBeamStatus_->canMeasure() && mirrorMask_->canMeasure() && monoMask_->canMeasure());

	return result;
}

bool BioXASSidePOEBeamStatusControl::canMove() const
{
	bool result = false;

	if (isConnected())
		result = (frontEndBeamStatus_->canMove() && mirrorMask_->canMove());

	return result;
}

bool BioXASSidePOEBeamStatusControl::canStop() const
{
	bool result = false;

	if (isConnected())
		result = (frontEndBeamStatus_->canStop() && mirrorMask_->canStop());

	return result;
}

bool BioXASSidePOEBeamStatusControl::isOn() const
{
	bool result = false;

	if (isConnected())
		result = (frontEndBeamStatus_->isOn() && (mirrorMask_->value() > 0) && (monoMask_->isOpen()));

	return result;
}

void BioXASSidePOEBeamStatusControl::setFrontEndBeamStatus(BioXASFrontEndBeamStatusControl *newControl)
{
	if (frontEndBeamStatus_ != newControl) {

		if (frontEndBeamStatus_)
			removeChildControl(frontEndBeamStatus_);

		frontEndBeamStatus_ = newControl;

		if (frontEndBeamStatus_)
			addChildControl(frontEndBeamStatus_);

		emit frontEndBeamStatusChanged(frontEndBeamStatus_);
	}
}

void BioXASSidePOEBeamStatusControl::setMirrorMask(AMControl *newControl)
{
	if (mirrorMask_ != newControl) {

		if (mirrorMask_)
			removeChildControl(mirrorMask_);

		mirrorMask_ = newControl;

		if (mirrorMask_)
			addChildControl(mirrorMask_);

		emit mirrorMaskChanged(mirrorMask_);
	}
}

void BioXASSidePOEBeamStatusControl::setMonoMask(AMControl *newControl)
{
	if (monoMask_ != newControl) {

		if (monoMask_)
			removeChildControl(monoMask_);

		monoMask_ = newControl;

		if (monoMask_)
			addChildControl(monoMask_);

		emit monoMaskChanged(monoMask_);
	}
}

void BioXASSidePOEBeamStatusControl::updateConnected()
{
	bool isConnected = (
				frontEndBeamStatus_ && frontEndBeamStatus_->isConnected() &&
				mirrorMask_ && mirrorMask_->isConnected() &&
				monoMask_ && monoMask_->isConnected()
				);

	setConnected(isConnected);
}

void BioXASSidePOEBeamStatusControl::updateMoving()
{
	bool isMoving = false;

	if (isConnected()) {
		isMoving = (
					frontEndBeamStatus_->isMoving() ||
					mirrorMask_->isMoving() ||
					monoMask_->isMoving()
					);
	}

	setIsMoving(isMoving);
}
