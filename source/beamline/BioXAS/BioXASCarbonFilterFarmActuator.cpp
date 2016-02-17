#include "BioXASCarbonFilterFarmActuator.h"

BioXASCarbonFilterFarmActuator::BioXASCarbonFilterFarmActuator(const QString &deviceName, QObject *parent) :
	BioXASBeamlineComponent(deviceName, parent)
{
	// Create actuator position control.

	position_ = new BioXASCarbonFilterFarmActuatorPositionControl(name()+"Position", "mm", this);
	addChildControl(position_);

	connect( position_, SIGNAL(valueChanged(double)), this, SLOT(onPositionControlValueChanged()) );
	connect( position_, SIGNAL(statusValueChanged(double)), this, SLOT(onPositionControlStatusValueChanged()) );
	connect( position_, SIGNAL(motorChanged(CLSMAXvMotor*)), this, SIGNAL(motorChanged(CLSMAXvMotor*)) );
	connect( position_, SIGNAL(statusChanged(AMControl*)), this, SIGNAL(positionStatusChanged(AMControl*)) );

	// Create actuator window control.

	window_ = new BioXASCarbonFilterFarmActuatorWindowControl(name()+"Window", this);
	addChildControl(window_);

	window_->setPositionControl(position_);

	connect( window_, SIGNAL(valueChanged(double)), this, SLOT(onWindowControlValueChanged()) );
	connect( window_, SIGNAL(windowsChanged()), this, SIGNAL(windowsChanged()) );

	// Create actuator filter control.

	filter_ = new BioXASCarbonFilterFarmActuatorFilterControl(name()+"Filter", "um", this);
	addChildControl(filter_);

	filter_->setWindowControl(window_);

	connect( filter_, SIGNAL(valueChanged(double)), this, SLOT(onFilterControlValueChanged()) );
	connect( filter_, SIGNAL(filtersChanged()), this, SIGNAL(filtersChanged()) );
	connect( filter_, SIGNAL(windowPreferencesChanged()), this, SIGNAL(windowPreferencesChanged()) );
}

BioXASCarbonFilterFarmActuator::~BioXASCarbonFilterFarmActuator()
{

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

double BioXASCarbonFilterFarmActuator::positionValue() const
{
	double result = -1;

	if (position_ && position_->canMeasure())
		result = position_->value();

	return result;
}

double BioXASCarbonFilterFarmActuator::positionStatusValue() const
{
	double result = -1;

	if (position_ && position_->status() && position_->status()->canMeasure())
		result = position_->status()->value();

	return result;
}

double BioXASCarbonFilterFarmActuator::windowValue() const
{
	double result = -1;

	if (window_ && window_->canMeasure())
		result = window_->value();

	return result;
}

double BioXASCarbonFilterFarmActuator::filterValue() const
{
	double result = -1;

	if (filter_ && filter_->canMeasure())
		result = filter_->value();

	return result;
}

CLSMAXvMotor* BioXASCarbonFilterFarmActuator::motor() const
{
	CLSMAXvMotor *result = 0;

	if (position_)
		result = position_->motor();

	return result;
}

AMControl* BioXASCarbonFilterFarmActuator::positionStatus() const
{
	AMControl *result = 0;

	if (position_)
		result = position_->status();

	return result;
}

bool BioXASCarbonFilterFarmActuator::setMotor(CLSMAXvMotor *newControl)
{
	bool result = false;

	if (position_) {
		position_->setMotor(newControl);
		result = true;
	}

	return result;
}

bool BioXASCarbonFilterFarmActuator::setPositionStatus(AMControl *newControl)
{
	bool result = false;

	if (position_) {
		position_->setStatus(newControl);
		result = true;
	}

	return result;
}

void BioXASCarbonFilterFarmActuator::addWindow(int windowIndex, const QString &windowName, double positionSetpoint, double positionMin, double positionMax, double filter)
{
	if (window_)
		window_->addWindow(windowIndex, windowName, positionSetpoint, positionMin, positionMax);

	if (filter_)
		filter_->addFilter(windowIndex, filter);
}

void BioXASCarbonFilterFarmActuator::removeWindow(int windowIndex)
{
	if (window_)
		window_->removeWindow(windowIndex);

	if (filter_)
		filter_->removeFilter(windowIndex);
}

void BioXASCarbonFilterFarmActuator::clearWindows()
{
	if (window_)
		window_->clearWindows();

	if (filter_)
		filter_->clearFilters();
}

void BioXASCarbonFilterFarmActuator::setWindowPreference(double filter, int windowIndex)
{
	if (filter_)
		filter_->setWindowPreference(filter, windowIndex);
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

void BioXASCarbonFilterFarmActuator::onPositionControlValueChanged()
{
	emit positionValueChanged( positionValue() );
}

void BioXASCarbonFilterFarmActuator::onPositionControlStatusValueChanged()
{
	emit positionStatusValueChanged( positionStatusValue() );
}

void BioXASCarbonFilterFarmActuator::onWindowControlValueChanged()
{
	emit windowValueChanged( windowValue() );
}

void BioXASCarbonFilterFarmActuator::onFilterControlValueChanged()
{
	emit filterValueChanged( filterValue() );
}
