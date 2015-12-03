#include "BioXASFilterFlipper.h"

BioXASFilterFlipper::BioXASFilterFlipper(const QString &deviceName, QObject *parent) :
	BioXASBeamlineComponent(deviceName, parent)
{
	// Create class variables.

	currentSlide_ = new AMPVControl("BFF1607-6-01:Slide:Current", "BFF1607-6-01:Slide:Current", "BFF1607-6-01:Slide:Select", "", this);
	addChildControl(currentSlide_);

	currentFilter_ = new BioXASFilterFlipperFilters(name()+"Filters", this);
	currentFilter_->setSlides(currentSlide_);
	addChildControl(currentFilter_);

	slideChangeStatus_ = new AMReadOnlyPVControl("BFF1607-6-01:Status", "BFF1607-6-01:Status", this);
	addChildControl(slideChangeStatus_);

	runMode_ = new AMSinglePVControl("BFF1607-6-01:RunMode", "BFF1607-6-01:RunMode", this);
	addChildControl(runMode_);

	nextStepTrigger_ = new AMSinglePVControl("BFF1607-6-01:goToNextStep", "BFF1607-6-01:goToNextStep", this);
	addChildControl(nextStepTrigger_);

	slideReceiverStatus_ = new AMReadOnlyPVControl("BFF1607-6-01:Slide:InReceiver", "BFF1607-6-01:Slide:InReceiver", this);
	addChildControl(slideReceiverStatus_);

	receiverStageCWLimit_ = new AMReadOnlyPVControl("SMTR1607-6-I22-20:cw", "SMTR1607-6-I22-20:cw", this);
	addChildControl(receiverStageCWLimit_);

	receiverStageCCWLimit_ = new AMReadOnlyPVControl("SMTR1607-6-I22-20:ccw", "SMTR1607-6-I22-20:ccw", this);
	addChildControl(receiverStageCCWLimit_);

	cartridgeStageCWLimit_ = new AMReadOnlyPVControl("SMTR1607-6-I22-21:cw", "SMTR1607-6-I22-21:cw", this);
	addChildControl(cartridgeStageCWLimit_);

	cartridgeStageCCWLimit_ = new AMReadOnlyPVControl("SMTR1607-6-I22-21:ccw", "SMTR1607-6-I22-21:ccw", this);
	addChildControl(cartridgeStageCCWLimit_);

	// Current settings.

	updateConnected();
}

BioXASFilterFlipper::~BioXASFilterFlipper()
{

}

bool BioXASFilterFlipper::isConnected() const
{
	bool connected = (
				currentSlide_ && currentSlide_->isConnected() &&
				currentFilter_ && currentFilter_->isConnected() &&
				slideChangeStatus_ && slideChangeStatus_->isConnected() &&
				runMode_ && runMode_->isConnected() &&
				nextStepTrigger_ && nextStepTrigger_->isConnected() &&
				slideReceiverStatus_ && slideReceiverStatus_->isConnected() &&
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

QString BioXASFilterFlipper::statusToString(int status) const
{
	QString result = "Unknown";

	switch (status) {
	case Done:
		result = "Done";
		break;
	case Changing:
		result = "Changing";
		break;
	case Error:
		result = "Error";
		break;
	default:
		break;
	}

	return result;
}

QString BioXASFilterFlipper::modeToString(int mode) const
{
	QString result = "Unknown";

	switch (mode) {
	case Auto:
		result = "Auto";
		break;
	case User:
		result = "User";
		break;
	default:
		break;
	}

	return result;
}

void BioXASFilterFlipper::setSlideCartridgeStatus(int index, AMReadOnlyPVControl *status)
{
	if (slideCartridgeStatusMap_.keys().contains(index))
		removeSlideCartridgeStatus(index);

	slideCartridgeStatusMap_.insert(index, status);

	if (status)
		addChildControl(status);
}

void BioXASFilterFlipper::removeSlideCartridgeStatus(int index)
{
	if (slideCartridgeStatusMap_.keys().contains(index)) {

		AMReadOnlyPVControl *oldStatus = slideCartridgeStatusMap_.value(index);
		slideCartridgeStatusMap_.remove(index);

		if (oldStatus)
			removeChildControl(oldStatus);
	}
}
