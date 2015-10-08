#include "BioXASFilterFlipper.h"

BioXASFilterFlipper::BioXASFilterFlipper(QObject *parent) :
	BioXASBeamlineComponent("BioXASFilterFlipper", parent)
{
	// Create member variables.

	status_ = new AMReadOnlyPVControl(name()+"Status", "BFF2023-01:Status", this);
	runMode_ = new AMSinglePVControl(name()+"RunMode", "BFF2023-01:RunMode", this);
	goToNextStep_ = new AMSinglePVControl(name()+"NextStep", "BFF2023-01:goToNextStep", this);

	slides_ = createSlides(10);

	currentSlide_ = new AMPVControl(name()+"CurrentSlide", "BFF2023-01:Slide:Current", "BFF2023-01:Slide:Select", "", this);
	currentSlideStatus_ = new AMReadOnlyPVControl(name()+"CurrentSlideStatus", "BFF2023-01:Slide:InReceiver", this);
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
				slidesConnected()
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

void BioXASFilterFlipper::setSlideFilter(int index, AMElement *element, double thickness)
{
	setSlideFilter(index, createFilter(element, thickness));
}

void BioXASFilterFlipper::setSlideFilter(int index, BioXASFilterFlipperFilter *newFilter)
{
	BioXASFilterFlipperSlide *slide = slides_.at(index-1);

	if (slide) {
		if (slide->hasFilter()) {
			BioXASFilterFlipperFilter *filter = slide->filter();
			filter->disconnect();
			slide->setFilter(0);
			filter->deleteLater();
		}

		slide->setFilter(newFilter);
	}
}

QList<BioXASFilterFlipperSlide*> BioXASFilterFlipper::createSlides(int slideCount)
{
	QList<BioXASFilterFlipperSlide*> slides;

	for (int i = 1; i <= slideCount; i++) {
		QString slideName = QString(name()+QString("Slide%1").arg(i));

		BioXASFilterFlipperSlide *slide = new BioXASFilterFlipperSlide(slideName, this);
		slide->setEnabled(true);
		slide->setCartridgeStatusControl(new AMReadOnlyPVControl(slideName+"Status", QString("BFF2023-01:Slide:%1").arg(i), this));

		slides.append(slide);
	}

	return slides;
}

bool BioXASFilterFlipper::slidesConnected() const
{
	bool connected = true;

	for (int i = 0, slideCount = slides_.count(); i < slideCount && connected; i++) {
		BioXASFilterFlipperSlide *slide = slides_.at(i);
		connected &= (slide && slide->isConnected());
	}

	return connected;
}

BioXASFilterFlipperFilter* BioXASFilterFlipper::createFilter(AMElement *element, double thickness)
{
	return new BioXASFilterFlipperFilter(element, thickness, this);
}
