#include "BioXASCarbonFilterFarmActuator.h"

BioXASCarbonFilterFarmActuator::BioXASCarbonFilterFarmActuator(const QString &deviceName, QObject *parent) :
	BioXASBeamlineComponent(deviceName, parent)
{
	// Initialize class varibles.

	position_ = 0;
	window_ = 0;
	filter_ = 0;

	// Current settings.

	setPosition(new BioXASCarbonFilterFarmActuatorPositionControl(name()+"Position", "mm", this));
	setWindow(new BioXASCarbonFilterFarmActuatorWindowControl(name()+"Window", this));
	setFilter(new BioXASCarbonFilterFarmActuatorFilterControl(name()+"Filter", "um", this));
}

BioXASCarbonFilterFarmActuator::~BioXASCarbonFilterFarmActuator()
{

}

AMControl* BioXASCarbonFilterFarmActuator::position() const
{
	AMControl *result = 0;

	if (position_)
		result = position_->positionControl();

	return result;
}

AMControl* BioXASCarbonFilterFarmActuator::positionStatus() const
{
	AMControl *result = 0;

	if (position_)
		result = position_->statusControl();

	return result;
}

bool BioXASCarbonFilterFarmActuator::isConnected() const
{
	bool connected = (
				position_ && position_->isConnected() &&
				window_ && window_->isConnected() &&
				filter_ && filter_->isConnected()
				);

	return connected;
}

QString BioXASCarbonFilterFarmActuator::windowToString(double window)
{
	QString result;

	switch (int(window)) {
	case BioXASCarbonFilterFarmActuator::Window::None:
		result = "None";
		break;
	case BioXASCarbonFilterFarmActuator::Window::Bottom:
		result = "Bottom";
		break;
	case BioXASCarbonFilterFarmActuator::Window::Top:
		result = "Top";
		break;
	default:
		break;
	}

	return result;
}

void BioXASCarbonFilterFarmActuator::setPositionControl(AMControl *newControl)
{
	if (position_)
		position_->setPositionControl(newControl);
}

void BioXASCarbonFilterFarmActuator::setPositionStatusControl(AMControl *newControl)
{
	if (position_)
		position_->setStatusControl(newControl);
}

void BioXASCarbonFilterFarmActuator::setWindow(BioXASCarbonFilterFarmActuatorWindowControl *newControl)
{
	if (window_ != newControl) {

		if (window_)
			removeChildControl(window_); // disconnects from all signals.

		window_ = newControl;

		if (window_) {
			addChildControl(window_);

			connect( window_, SIGNAL(positionChanged(BioXASCarbonFilterFarmActuatorPositionControl*)), this, SLOT(updatePosition()) );
		}

		updateFilter();
		updateConnected();

		emit windowChanged(window_);
	}
}

void BioXASCarbonFilterFarmActuator::setFilter(BioXASCarbonFilterFarmActuatorFilterControl *newControl)
{
	if (filter_ != newControl) {

		if (filter_)
			removeChildControl(filter_); // disconnects from all signals.

		filter_ = newControl;

		if (filter_) {
			addChildControl(filter_);

			connect( filter_, SIGNAL(windowChanged(BioXASCarbonFilterFarmActuatorWindowControl*)), this, SLOT(updateWindow()) );
		}

		updateFilter();
		updateConnected();

		emit filterChanged(filter_);
	}
}

bool BioXASCarbonFilterFarmActuator::setPosition(BioXASCarbonFilterFarmActuatorPositionControl *newControl)
{
	bool result = false;

	if (position_ != newControl) {

		if (position_)
			removeChildControl(position_); // disconnects from all signals.

		position_ = newControl;

		if (position_) {
			addChildControl(position_);

			connect( position_, SIGNAL(positionControlChanged(AMControl*)), this, SIGNAL(positionChanged(AMControl*)) );
			connect( position_, SIGNAL(statusControlChanged(AMControl*)), this, SIGNAL(positionStatusChanged(AMControl*)) );
		}

		updateWindow();
		updateConnected();

		result = true;
	}

	return result;
}

void BioXASCarbonFilterFarmActuator::addWindow(Window::Option window, double positionSetpoint, double positionMin, double positionMax, double filter)
{
	if (window_)
		window_->addWindow(window, windowToString(window), positionSetpoint, positionMin, positionMax);

	if (filter_)
		filter_->addFilter(window, filter);
}

void BioXASCarbonFilterFarmActuator::removeWindow(Window::Option window)
{
	if (window_)
		window_->removeWindow(window);

	if (filter_)
		filter_->removeFilter(window);
}

void BioXASCarbonFilterFarmActuator::clearWindows()
{
	if (window_)
		window_->clearWindows();

	if (filter_)
		filter_->clearFilters();
}

void BioXASCarbonFilterFarmActuator::setWindowPreference(double filter, Window::Option window)
{
	if (filter_)
		filter_->setWindowPreference(filter, window);
}

void BioXASCarbonFilterFarmActuator::removeWindowPreference(double filter)
{
	if (filter_)
		filter_->removeWindowPreference(filter);
}

void BioXASCarbonFilterFarmActuator::clearWindowPreferences()
{
	if (filter_)
		filter_->clearWindowPreferences();
}

void BioXASCarbonFilterFarmActuator::updatePosition()
{
	if (window_)
		setPosition(window_->positionControl());
}

void BioXASCarbonFilterFarmActuator::updateWindow()
{
	if (filter_)
		setWindow(filter_->windowControl());

	if (window_)
		window_->setPositionControl(position_);
}

void BioXASCarbonFilterFarmActuator::updateFilter()
{
	if (filter_)
		filter_->setWindowControl(window_);
}
