#include "BioXASFilterFlipper.h"

BioXASFilterFlipper::BioXASFilterFlipper(QObject *parent) :
	BioXASBeamlineComponent("BioXASFilterFlipper", parent)
{
	// Create member variables.

	currentSlide_ = 0;
	slides_ = createSlides(10);

	statusControl_ = new AMReadOnlyPVControl(name()+"Status", "BFF2023-01:Status", this);
	connect( statusControl_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	runModeControl_ = new AMSinglePVControl(name()+"RunMode", "BFF2023-01:RunMode", this);
	connect( runModeControl_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	goToNextStepControl_ = new AMSinglePVControl(name()+"NextStep", "BFF2023-01:goToNextStep", this);
	connect( goToNextStepControl_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	currentSlideControl_ = new AMPVControl(name()+"CurrentSlide", "BFF2023-01:Slide:Current", "BFF2023-01:Slide:Select", "", this);
	connect( currentSlideControl_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( currentSlideControl_, SIGNAL(valueChanged(double)), this, SLOT(updateCurrentSlide()) );

	currentSlideStatusControl_ = new AMReadOnlyPVControl(name()+"CurrentSlideStatus", "BFF2023-01:Slide:InReceiver", this);
	connect( currentSlideStatusControl_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// Current settings.

	updateConnected();
	updateCurrentSlide();
}

BioXASFilterFlipper::~BioXASFilterFlipper()
{

}

bool BioXASFilterFlipper::isConnected() const
{
	bool connected = (
				statusControl_ && statusControl_->isConnected() &&
				runModeControl_ && runModeControl_->isConnected() &&
				goToNextStepControl_ && goToNextStepControl_->isConnected() &&
				currentSlideControl_ && currentSlideControl_->isConnected() &&
				currentSlideStatusControl_ && currentSlideStatusControl_->isConnected() &&
				slidesConnected()
				);

	return connected;
}

BioXASFilterFlipperSlide* BioXASFilterFlipper::slideWithFilter(BioXASFilterFlipperFilter *filter) const
{
	BioXASFilterFlipperSlide *result = 0;

	if (filter) {
		for (int i = 0, count = slides_.count(); i < count && !result; i++) {
			BioXASFilterFlipperSlide *slide = slides_.at(i);

			if (slide && slide->filter() == filter)
				result = slide;
		}
	}

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

void BioXASFilterFlipper::setSlideFilter(int index, AMElement *element, double thickness)
{
	setSlideFilter(index, createFilter(element, thickness));
}

void BioXASFilterFlipper::setCurrentSlide(BioXASFilterFlipperSlide *currentSlide)
{
	if (currentSlide_ != currentSlide) {

		if (currentSlide_)
			disconnect( currentSlide_, 0, this, 0 );

		currentSlide_ = currentSlide;

		if (currentSlide_)
			connect( currentSlide_, SIGNAL(enabledChanged(bool)), this, SLOT(onCurrentSlideEnabledChanged()) );

		emit currentSlideChanged(currentSlide_);
	}
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

void BioXASFilterFlipper::updateCurrentSlide()
{
	// The slides list is indexed 0 --> n - 1.
	// The current slide pv is indexed 1 --> n.

	int currentSlideControlIndex = int(currentSlideControl_->value());

	if (currentSlideControlIndex == 0)
		setCurrentSlide(0);
	else if (currentSlideControlIndex > 0)
		setCurrentSlide(slides_.at(currentSlideControlIndex - 1));
}

void BioXASFilterFlipper::onCurrentSlideEnabledChanged()
{
	if (currentSlide_ && !currentSlide_->enabled())
		currentSlideControl_->move(0);
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
