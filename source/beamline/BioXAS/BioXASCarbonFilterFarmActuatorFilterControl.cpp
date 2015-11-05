#include "BioXASCarbonFilterFarmActuatorFilterControl.h"
#include "actions3/AMActionSupport.h"

BioXASCarbonFilterFarmActuatorFilterControl::BioXASCarbonFilterFarmActuatorFilterControl(const QString &name, const QString &units, QObject *parent) :
	AMPseudoMotorControl(name, units, parent)
{
	// Initialize local variables.

	currentWindow_ = 0;

	// Initialize inherited variables.

	value_ = -1;
	setpoint_ = -1;
	minimumValue_ = 0;
	maximumValue_ = 0;

	setAllowsMovesWhileMoving(false);
	setContextKnownDescription("Actuator Filter Control");
}

BioXASCarbonFilterFarmActuatorFilterControl::~BioXASCarbonFilterFarmActuatorFilterControl()
{

}

bool BioXASCarbonFilterFarmActuatorFilterControl::canMeasure() const
{
	bool result = false;

	if (isConnected())
		result = currentWindow_->canMeasure();

	return result;
}

bool BioXASCarbonFilterFarmActuatorFilterControl::canMove() const
{
	bool result = false;

	if (isConnected())
		result = currentWindow_->canMove();

	return result;
}

bool BioXASCarbonFilterFarmActuatorFilterControl::canStop() const
{
	bool result = false;

	if (isConnected())
		result = currentWindow_->canStop();

	return result;
}

bool BioXASCarbonFilterFarmActuatorFilterControl::validValue(double value) const
{
	bool result = (value >= 0 && value < enumNames().count());
	return result;
}

bool BioXASCarbonFilterFarmActuatorFilterControl::validSetpoint(double value) const
{
	bool result = (value >= 0 && value < filters_.count());
	return result;
}

double BioXASCarbonFilterFarmActuatorFilterControl::filterAt(int index) const
{
	double result = -1;

	if (index >= 0 && index < filters_.count())
		result = filters_.at(index);

	return result;
}

int BioXASCarbonFilterFarmActuatorFilterControl::indexOf(double filter) const
{
	return filters_.indexOf(filter);
}

int BioXASCarbonFilterFarmActuatorFilterControl::indexOf(const QString filterString) const
{
	return enumNames().indexOf(filterString);
}

void BioXASCarbonFilterFarmActuatorFilterControl::setCurrentWindow(BioXASCarbonFilterFarmActuatorWindowControl *newControl)
{
	if (currentWindow_ != newControl) {

		if (currentWindow_)
			removeChildControl(currentWindow_);

		currentWindow_ = newControl;

		if (currentWindow_)
			addChildControl(currentWindow_);

		updateStates();

		emit currentWindowChanged(currentWindow_);
	}
}

void BioXASCarbonFilterFarmActuatorFilterControl::setFilterWindow(double filter, BioXASCarbonFilterFarmWindowOption *window)
{
	if (filters_.contains(filter) && window && window->isValid() && window->filter() == filter)
		filterWindowMap_.insert(filter, window);
}

void BioXASCarbonFilterFarmActuatorFilterControl::updateStates()
{
	updateConnected();
	updateFilters();
	updateEnumStates();
	updateMaximumValue();
	updateValue();
	updateMoving();
}

void BioXASCarbonFilterFarmActuatorFilterControl::updateConnected()
{
	bool connected = ( currentWindow_ && currentWindow_->isConnected() );
	setConnected(connected);
}

void BioXASCarbonFilterFarmActuatorFilterControl::updateValue()
{
	double newValue = enumNames().indexOf("Unknown");

	if (isConnected()) {
		BioXASCarbonFilterFarmWindowOption *currentWindow = currentWindow_->windowAt(int(currentWindow_->value()));

		if (currentWindow && currentWindow->isValid()) {
			double currentFilter = currentWindow->filter();
			int currentIndex = indexOf(currentFilter);

			if (currentIndex >= 0)
				newValue = currentIndex;
		}
	}

	setValue(newValue);
}

void BioXASCarbonFilterFarmActuatorFilterControl::updateMoving()
{
	bool isMoving = false;

	if (isConnected())
		isMoving = (currentWindow_->isMoving());

	setIsMoving(isMoving);
}

void BioXASCarbonFilterFarmActuatorFilterControl::updateMaximumValue()
{
	setMaximumValue(enumNames().count() - 1);
}

void BioXASCarbonFilterFarmActuatorFilterControl::addFilter(double filter)
{
	if (!filters_.contains(filter)) {
		filters_.append(filter);
		updateEnumStates();

		emit filtersChanged();
	}
}

void BioXASCarbonFilterFarmActuatorFilterControl::clearFilters()
{
	filters_.clear();
	filterWindowMap_.clear();

	updateEnumStates();

	emit filtersChanged();
}

void BioXASCarbonFilterFarmActuatorFilterControl::updateFilters()
{
	// Clear the existing list of filters.

	clearFilters();

	// Update filters list based on window contents.

	if (currentWindow_) {
		foreach (BioXASCarbonFilterFarmWindowOption *window, currentWindow_->windows()) {
			if (window && window->isValid()) {
				addFilter(window->filter());
				setFilterWindow(window->filter(), window);
			}
		}
	}
}

void BioXASCarbonFilterFarmActuatorFilterControl::updateEnumStates()
{
	setEnumStates(generateEnumStates(filters_));
	setMoveEnumStates(generateMoveEnumStates(filters_));
}

AMAction3* BioXASCarbonFilterFarmActuatorFilterControl::createMoveAction(double setpoint)
{
	AMAction3 *action = 0;

	double filter = filterAt(int(setpoint));
	BioXASCarbonFilterFarmWindowOption *window = filterWindowMap_.value(filter, 0);

	if (currentWindow_)
		action = AMActionSupport::buildControlMoveAction(currentWindow_, currentWindow_->indexOf(window));

	return action;
}

QStringList BioXASCarbonFilterFarmActuatorFilterControl::generateEnumStates(QList<double> filterOptions)
{
	QStringList enumOptions = generateMoveEnumStates(filterOptions);

	// We always want to have an "Unknown" option--it's the default value.
	// Because it isn't a 'move enum' (we don't ever want to move to "Unknown")
	// it must be at the end of the enum list, after all of the move enums.

	enumOptions << "Unknown";

	return enumOptions;
}

QStringList BioXASCarbonFilterFarmActuatorFilterControl::generateMoveEnumStates(QList<double> filterOptions)
{
	QStringList moveOptions;

	foreach (double option, filterOptions)
		moveOptions << filterToString(option);

	return moveOptions;
}

QString BioXASCarbonFilterFarmActuatorFilterControl::filterToString(double filter) const
{
	QString result = "Unknown";

	if (filters_.contains(filter))
		result = QString::number(filter, 'f', 0);

	return result;
}

