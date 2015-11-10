#include "BioXASCarbonFilterFarmActuator.h"

BioXASCarbonFilterFarmActuator::BioXASCarbonFilterFarmActuator(const QString &deviceName, QObject *parent) :
	BioXASCarbonFilterFarmActuatorControl(deviceName, parent)
{	
	setPosition(new BioXASCarbonFilterFarmActuatorPositionControl(name()+"Position", "mm", this));
	setWindow(new BioXASCarbonFilterFarmActuatorWindowControl(name()+"Window", this));
	setFilter(new BioXASCarbonFilterFarmActuatorFilterControl(name()+"Filter", "mm", this));
}

BioXASCarbonFilterFarmActuator::~BioXASCarbonFilterFarmActuator()
{

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
