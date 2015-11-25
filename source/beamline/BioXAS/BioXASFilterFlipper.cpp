#include "BioXASFilterFlipper.h"

BioXASFilterFlipper::BioXASFilterFlipper(const QString &deviceName, QObject *parent) :
	BioXASFilterFlipperControl(deviceName, parent)
{
	// Create class variables.

	currentSlide_ = new AMPVControl("BFF2023-01:Slide:Current", "BFF2023-01:Slide:Current", "BFF2023-01:Slide:Select", "", this);
	addChildControl(currentSlide_);

	slideChangeStatus_ = new AMReadOnlyPVControl("BFF2023-01:Status", "BFF2023-01:Status", this);
	addChildControl(slideChangeStatus_);

	runMode_ = new AMSinglePVControl("BFF2023-01:RunMode", "BFF2023-01:RunMode", this);
	addChildControl(runMode_);

	nextStepTrigger_ = new AMSinglePVControl("BFF2023-01:goToNextStep", "BFF2023-01:goToNextStep", this);
	addChildControl(nextStepTrigger_);

	slideReceiverStatus_ = new AMReadOnlyPVControl("BFF2023-01:Slide:InReceiver", "BFF2023-01:Slide:InReceiver", this);
	addChildControl(slideReceiverStatus_);

	receiverStageCWLimit_ = new AMReadOnlyPVControl("SMTR2023-01:cw", "SMTR2023-01:cw", this);
	addChildControl(receiverStageCWLimit_);

	receiverStageCCWLimit_ = new AMReadOnlyPVControl("SMTR2023-01:ccw", "SMTR2023-01:ccw", this);
	addChildControl(receiverStageCCWLimit_);

	cartridgeStageCWLimit_ = new AMReadOnlyPVControl("SMTR2023-02:cw", "SMTR2023-02:cw", this);
	addChildControl(cartridgeStageCWLimit_);

	cartridgeStageCCWLimit_ = new AMReadOnlyPVControl("SMTR2023-02:ccw", "SMTR2023-02:ccw", this);
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

BioXASFilterFlipperFilter* BioXASFilterFlipper::filterForSlide(int slideIndex)
{
	BioXASFilterFlipperFilter *result = 0;

	if (slideFilterMap_.keys().contains(slideIndex))
		result = slideFilterMap_.value(slideIndex);

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

void BioXASFilterFlipper::setSlideFilter(int index, const QString &elementSymbol, double thickness)
{
	BioXASFilterFlipperFilter *newFilter = createFilter(elementSymbol, thickness);

	if (BioXASFilterFlipperControl::setSlideFilter(index, newFilter))
		emit slideFilterChanged(index);
}

void BioXASFilterFlipper::removeSlideFilter(int index)
{
	if (BioXASFilterFlipperControl::removeSlideFilter(index))
		emit slideFilterChanged(index);
}

BioXASFilterFlipperFilter* BioXASFilterFlipper::createFilter(const QString &elementSymbol, double thickness)
{
	return new BioXASFilterFlipperFilter(elementSymbol, thickness, this);
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


