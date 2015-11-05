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

	setContextKnownDescription("Actuator Filter Control");
	setAllowsMovesWhileMoving(false);
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

		if (currentWindow_) {
			addChildControl(currentWindow_);

			connect( currentWindow_, SIGNAL(windowsChanged()), this, SLOT(updateFilters()) );
		}

		updateStates();

		emit currentWindowChanged(currentWindow_);
	}
}

void BioXASCarbonFilterFarmActuatorFilterControl::setFilterWindowPreference(double filter, BioXASCarbonFilterFarmWindowOption *window)
{
	if (filters_.contains(filter) && window && window->isValid() && currentWindow_->indexOf(window) != -1 && double(window->filter()) == filter)
		filterWindowPreferenceMap_.insert(filter, window);
}

void BioXASCarbonFilterFarmActuatorFilterControl::clearFilterWindowPreference(double filter)
{
	if (filterWindowPreferenceMap_.keys().contains(filter))
		filterWindowPreferenceMap_.remove(filter);
}

void BioXASCarbonFilterFarmActuatorFilterControl::clearFilterWindowPreference(BioXASCarbonFilterFarmWindowOption *window)
{
	if (filterWindowPreferenceMap_.values().contains(window)) {
		QList<double> filters = filterWindowPreferenceMap_.keys(window);

		foreach (double filter, filters)
			filterWindowPreferenceMap_.remove(filter);
	}
}

void BioXASCarbonFilterFarmActuatorFilterControl::clearFilterWindowPreferences()
{
	filterWindowPreferenceMap_.clear();
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

void BioXASCarbonFilterFarmActuatorFilterControl::addFilter(BioXASCarbonFilterFarmWindowOption *newOption)
{
	if (newOption && newOption->isValid() && !filters_.contains(newOption->filter())) {
		filters_.append(newOption->filter());
		filterWindowMap_.insert(double(newOption->filter()), newOption);

		updateEnumStates();

		emit filtersChanged();
	}
}

void BioXASCarbonFilterFarmActuatorFilterControl::clearFilters()
{
	filters_.clear();
	filterWindowMap_.clear();

	// The preferences map is not cleared here, because preferences would be lost
	// every time the filters list needs to be refreshed. See updateFilters().

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
			if (window && window->isValid())
				addFilter(window);
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
	// Convert the given setpoint index into an actual filter value.
	// Identify the corresponding window.

	double filter = filterAt(int(setpoint));

	int windowSetpoint = -1;

	if (filterWindowPreferenceMap_.keys().contains(filter) && currentWindow_->indexOf(filterWindowPreferenceMap_.value(filter)) != -1) {

		// First check to see if there is a window preference set for this particular filter,
		// and that the window is a valid window option according to the current window control.

		windowSetpoint = currentWindow_->indexOf(filterWindowPreferenceMap_.value(filter));

	} else {

		// If there is no valid preference, identify the window setpoint by looking at the filter-window
		// map. If there are multiple entries, pick the first.

		QList<BioXASCarbonFilterFarmWindowOption*> windowOptions = filterWindowMap_.values(filter);
		if (!windowOptions.isEmpty())
			windowSetpoint = currentWindow_->indexOf(windowOptions.first());
	}

	// Create and return action that moves the window control to the given window setpoint.

	AMAction3 *action = AMActionSupport::buildControlMoveAction(currentWindow_, windowSetpoint);
	return action;
}

QStringList BioXASCarbonFilterFarmActuatorFilterControl::generateEnumStates(const QList<double> &filterOptions) const
{
	QStringList enumOptions = generateMoveEnumStates(filterOptions);

	// We always want to have an "Unknown" option--it's the default value.
	// Because it isn't a 'move enum' (we don't ever want to move to "Unknown")
	// it must be at the end of the enum list, after all of the move enums.

	enumOptions << "Unknown";

	return enumOptions;
}

QStringList BioXASCarbonFilterFarmActuatorFilterControl::generateMoveEnumStates(const QList<double> &filterOptions) const
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

