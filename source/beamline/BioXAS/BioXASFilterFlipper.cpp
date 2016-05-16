#include "BioXASFilterFlipper.h"

BioXASFilterFlipper::BioXASFilterFlipper(const QString &deviceName, QObject *parent) :
	BioXASBeamlineComponent(deviceName, parent)
{
	// Create class variables.

	slides_ = 0;
	filters_ = 0;
	slideChangeStatus_ = 0;
	runMode_ = 0;
	nextStepTrigger_ = 0;
	receiverStatus_ = 0;
	receiverStageCWLimit_ = 0;
	receiverStageCCWLimit_ = 0;
	cartridgeStageCWLimit_ = 0;
	cartridgeStageCCWLimit_ = 0;
}

BioXASFilterFlipper::~BioXASFilterFlipper()
{

}

bool BioXASFilterFlipper::isConnected() const
{
	bool connected = (
				slides_ && slides_->isConnected() &&
				filters_ && filters_->isConnected() &&
				slideChangeStatus_ && slideChangeStatus_->isConnected() &&
				runMode_ && runMode_->isConnected() &&
				nextStepTrigger_ && nextStepTrigger_->isConnected() &&
				receiverStatus_ && receiverStatus_->isConnected() &&
				receiverStageCWLimit_ && receiverStageCWLimit_->isConnected() &&
				receiverStageCCWLimit_ && receiverStageCCWLimit_->isConnected() &&
				cartridgeStageCWLimit_ && cartridgeStageCWLimit_->isConnected() &&
				cartridgeStageCCWLimit_ && cartridgeStageCCWLimit_->isConnected()
				);

	return connected;
}

AMReadOnlyPVControl* BioXASFilterFlipper::cartridgeStatusForSlide(int slideIndex)
{
	AMReadOnlyPVControl *result = 0;

	if (slideCartridgeStatusMap_.keys().contains(slideIndex))
		result = slideCartridgeStatusMap_.value(slideIndex);

	return result;
}

void BioXASFilterFlipper::setSlides(AMPVControl *newControl)
{
	if (slides_ != newControl) {

                if (removeChildControl(slides_)){
                    slides_ = newControl;
                    addChildControl(slides_);
                }

		emit slidesChanged(slides_);
	}
}

void BioXASFilterFlipper::setFilters(BioXASFilterFlipperFilters *newControl)
{
	if (filters_ != newControl) {

		if (filters_)
			removeChildControl(filters_);

		filters_ = newControl;

		if (filters_)
			addChildControl(filters_);

		updateFilters(); // We want the filters control to be updated with our slides control.

		emit filtersChanged(filters_);
	}
}

void BioXASFilterFlipper::setSlideChangeStatus(AMReadOnlyPVControl *newControl)
{
	if (slideChangeStatus_ != newControl) {

		if (slideChangeStatus_)
			removeChildControl(slideChangeStatus_);

		slideChangeStatus_ = newControl;

		if (slideChangeStatus_)
			addChildControl(slideChangeStatus_);

		emit slideChangeStatusChanged(slideChangeStatus_);
	}
}

void BioXASFilterFlipper::setRunMode(AMPVControl *newControl)
{
	if (runMode_ != newControl) {

		if (runMode_)
			removeChildControl(runMode_);

		runMode_ = newControl;

		if (runMode_)
			addChildControl(runMode_);

		emit runModeChanged(runMode_);
	}
}

void BioXASFilterFlipper::setNextStepTrigger(AMSinglePVControl *newControl)
{
	if (nextStepTrigger_ != newControl) {

		if (nextStepTrigger_)
			removeChildControl(nextStepTrigger_);

		nextStepTrigger_ = newControl;

		if (nextStepTrigger_)
			addChildControl(nextStepTrigger_);

		emit nextStepTriggerChanged(nextStepTrigger_);
	}
}

void BioXASFilterFlipper::setReceiverStatus(AMReadOnlyPVControl *newControl)
{
	if (receiverStatus_ != newControl) {

		if (receiverStatus_)
			removeChildControl(receiverStatus_);

		receiverStatus_ = newControl;

		if (receiverStatus_)
			addChildControl(receiverStatus_);

		emit receiverStatusChanged(receiverStatus_);
	}
}

void BioXASFilterFlipper::setReceiverStageCWLimit(AMReadOnlyPVControl *newControl)
{
	if (receiverStageCWLimit_ != newControl) {

		if (receiverStageCWLimit_)
			removeChildControl(receiverStageCWLimit_);

		receiverStageCWLimit_ = newControl;

		if (receiverStageCWLimit_)
			addChildControl(receiverStageCWLimit_);

		emit receiverStageCWLimitChanged(receiverStageCWLimit_);
	}
}

void BioXASFilterFlipper::setReceiverStageCCWLimit(AMReadOnlyPVControl *newControl)
{
	if (receiverStageCCWLimit_ != newControl) {

		if (receiverStageCCWLimit_)
			removeChildControl(receiverStageCCWLimit_);

		receiverStageCCWLimit_ = newControl;

		if (receiverStageCCWLimit_)
			addChildControl(receiverStageCCWLimit_);

		emit receiverStageCCWLimitChanged(receiverStageCCWLimit_);
	}
}

void BioXASFilterFlipper::setCartridgeStageCWLimit(AMReadOnlyPVControl *newControl)
{
	if (cartridgeStageCWLimit_ != newControl) {

		if (cartridgeStageCWLimit_)
			removeChildControl(cartridgeStageCWLimit_);

		cartridgeStageCWLimit_ = newControl;

		if (cartridgeStageCWLimit_)
			addChildControl(cartridgeStageCWLimit_);

		emit cartridgeStageCWLimitChanged(cartridgeStageCWLimit_);
	}
}

void BioXASFilterFlipper::setCartridgeStageCCWLimit(AMReadOnlyPVControl *newControl)
{
	if (cartridgeStageCCWLimit_ != newControl) {

		if (cartridgeStageCCWLimit_)
			removeChildControl(cartridgeStageCCWLimit_);

		cartridgeStageCCWLimit_ = newControl;

		if (cartridgeStageCCWLimit_)
			addChildControl(cartridgeStageCCWLimit_);

		emit cartridgeStageCCWLimitChanged(cartridgeStageCCWLimit_);
	}
}

void BioXASFilterFlipper::setSlideCartridgeStatus(int index, AMReadOnlyPVControl *status)
{
	if (slideCartridgeStatusMap_.keys().contains(index))
		removeSlideCartridgeStatus(index);

	slideCartridgeStatusMap_.insert(index, status);

	if (status)
		addChildControl(status);

	emit slideCartridgeStatusChanged(index);
}

void BioXASFilterFlipper::removeSlideCartridgeStatus(int index)
{
	if (slideCartridgeStatusMap_.keys().contains(index)) {

		AMReadOnlyPVControl *oldStatus = slideCartridgeStatusMap_.value(index);
		slideCartridgeStatusMap_.remove(index);

		if (oldStatus)
			removeChildControl(oldStatus);

		emit slideCartridgeStatusChanged(index);
	}
}

void BioXASFilterFlipper::updateSlides()
{
	AMPVControl *slidesControl = 0;

	if (filters_)
		slidesControl = filters_->slides();

	setSlides(slidesControl);
}

void BioXASFilterFlipper::updateFilters()
{
	if (filters_)
		filters_->setSlides(slides_);
}
