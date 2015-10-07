#include "BioXASFilterFlipper.h"

BioXASFilterFlipper::BioXASFilterFlipper(QObject *parent) :
	BioXASBeamlineComponent("BioXASFilterFlipper", parent)
{
	// Create member variables.

	status_ = new AMReadOnlyPVControl(name()+"Status", "BFF2023-01:Status", this);
	runMode_ = new AMSinglePVControl(name()+"RunMode", "BFF2023-01:RunMode", this);
	goToNextStep_ = new AMSinglePVControl(name()+"NextStep", "BFF2023-01:goToNextStep", this);

	currentSlide_ = new AMPVControl(name()+"CurrentSlide", "BFF2023-01:Slide:Current", "BFF2023-01:Slide:Select", "", this);
	currentSlideStatus_ = new AMReadOnlyPVControl(name()+"CurrentSlideStatus", "BFF2023-01:Slide:InReceiver", this);

	slide1CartridgeStatus_ = new AMReadOnlyPVControl(name()+"Slide1Status", "BFF2023-01:Slide:1", this);
	slide2CartridgeStatus_ = new AMReadOnlyPVControl(name()+"Slide2Status", "BFF2023-01:Slide:2", this);
	slide3CartridgeStatus_ = new AMReadOnlyPVControl(name()+"Slide3Status", "BFF2023-01:Slide:3", this);
	slide4CartridgeStatus_ = new AMReadOnlyPVControl(name()+"Slide4Status", "BFF2023-01:Slide:4", this);
	slide5CartridgeStatus_ = new AMReadOnlyPVControl(name()+"Slide5Status", "BFF2023-01:Slide:5", this);
	slide6CartridgeStatus_ = new AMReadOnlyPVControl(name()+"Slide6Status", "BFF2023-01:Slide:6", this);
	slide7CartridgeStatus_ = new AMReadOnlyPVControl(name()+"Slide7Status", "BFF2023-01:Slide:7", this);
	slide8CartridgeStatus_ = new AMReadOnlyPVControl(name()+"Slide8Status", "BFF2023-01:Slide:8", this);
	slide9CartridgeStatus_ = new AMReadOnlyPVControl(name()+"Slide9Status", "BFF2023-01:Slide:9", this);
	slide10CartridgeStatus_ = new AMReadOnlyPVControl(name()+"Slide10Status", "BFF2023-01:Slide:10", this);
}

BioXASFilterFlipper::~BioXASFilterFlipper()
{

}

bool BioXASFilterFlipper::isConnected() const
{
	bool connected = (
				status_ && status_->isConnected() &&
				runMode_ && runMode_->isConnected() &&
				goToNextStep_ && goToNextStep_->isConnected() &&
				currentSlide_ && currentSlide_->isConnected() &&
				currentSlideStatus_ && currentSlideStatus_->isConnected() &&
				slide1CartridgeStatus_ && slide1CartridgeStatus_->isConnected() &&
				slide2CartridgeStatus_ && slide2CartridgeStatus_->isConnected() &&
				slide3CartridgeStatus_ && slide3CartridgeStatus_->isConnected() &&
				slide4CartridgeStatus_ && slide4CartridgeStatus_->isConnected() &&
				slide5CartridgeStatus_ && slide5CartridgeStatus_->isConnected() &&
				slide6CartridgeStatus_ && slide6CartridgeStatus_->isConnected() &&
				slide7CartridgeStatus_ && slide7CartridgeStatus_->isConnected() &&
				slide8CartridgeStatus_ && slide8CartridgeStatus_->isConnected() &&
				slide9CartridgeStatus_ && slide9CartridgeStatus_->isConnected() &&
				slide10CartridgeStatus_ && slide10CartridgeStatus_->isConnected()
				);

	return connected;
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

void BioXASFilterFlipper::setFilter(int index, AMElement *element, double thickness)
{
	setFilter(index, createFilter(element, thickness));
}

void BioXASFilterFlipper::setFilter(int index, BioXASFilterFlipperFilter *newFilter)
{
	BioXASFilterFlipperFilter *existingFilter = filters_.at(index);

	if (existingFilter != newFilter) {
		filters_.insert(index, newFilter);

		emit filterChanged(index, newFilter);
	}
}

BioXASFilterFlipperFilter* BioXASFilterFlipper::createFilter(AMElement *element, double thickness)
{
	return new BioXASFilterFlipperFilter(element, thickness, this);
}
