#include "BioXASCarbonFilterFarmFilterControl.h"
#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"

BioXASCarbonFilterFarmFilterControl::BioXASCarbonFilterFarmFilterControl(const QString &name, const QString &units, QObject *parent) :
	AMPseudoMotorControl(name, units, parent)
{
	// Initialize inherited variables.

	value_ = -1;
	setpoint_ = -1;
	minimumValue_ = 0;
	maximumValue_ = 0;

	setContextKnownDescription("Filter Control");

	// Initialize local variables.

	upstreamFilter_ = 0;
	downstreamFilter_ = 0;
}

BioXASCarbonFilterFarmFilterControl::~BioXASCarbonFilterFarmFilterControl()
{

}

bool BioXASCarbonFilterFarmFilterControl::canMeasure() const
{
	bool result = false;

	if (isConnected())
		result = ( upstreamFilter_->canMeasure() && downstreamFilter_->canMeasure() );

	return result;
}

bool BioXASCarbonFilterFarmFilterControl::canMove() const
{
	bool result = false;

	if (isConnected())
		result = ( upstreamFilter_->canMove() && downstreamFilter_->canMove() );

	return result;
}

bool BioXASCarbonFilterFarmFilterControl::canStop() const
{
	bool result = false;

	if (isConnected())
		result = ( upstreamFilter_->canStop() && downstreamFilter_->canStop() );

	return result;
}

bool BioXASCarbonFilterFarmFilterControl::validValue(double value) const
{
	bool result = (value >= 0 && value < enumNames().count());
	return result;
}

bool BioXASCarbonFilterFarmFilterControl::validSetpoint(double value) const
{
	bool result = (value >= 0 && value < moveEnumNames().count());
	return result;
}

double BioXASCarbonFilterFarmFilterControl::filterAt(int index) const
{
	double result = -1;

	if (index >= 0 && index < filters_.count())
		result = filters_.at(index);

	return result;
}

int BioXASCarbonFilterFarmFilterControl::indexOf(double filter) const
{
	return filters_.indexOf(filter);
}

int BioXASCarbonFilterFarmFilterControl::indexOf(const QString filterString) const
{
	return enumNames().indexOf(filterString);
}

void BioXASCarbonFilterFarmFilterControl::setUpstreamFilter(BioXASCarbonFilterFarmActuatorFilterControl *newControl)
{
	if (upstreamFilter_ != newControl) {

		if (upstreamFilter_)
			removeChildControl(upstreamFilter_);

		upstreamFilter_ = newControl;

		if (upstreamFilter_) {
			addChildControl(upstreamFilter_);

			connect( upstreamFilter_, SIGNAL(filtersChanged()), this, SLOT(updateFilters()) );
		}

		updateStates();

		emit upstreamFilterChanged(upstreamFilter_);
	}
}

void BioXASCarbonFilterFarmFilterControl::setDownstreamFilter(BioXASCarbonFilterFarmActuatorFilterControl *newControl)
{
	if (downstreamFilter_ != newControl) {

		if (downstreamFilter_)
			removeChildControl(downstreamFilter_);

		downstreamFilter_ = newControl;

		if (downstreamFilter_) {
			addChildControl(downstreamFilter_);

			connect( downstreamFilter_, SIGNAL(filtersChanged()), this, SLOT(updateFilters()) );
		}

		updateStates();

		emit downstreamFilterChanged(downstreamFilter_);
	}
}

void BioXASCarbonFilterFarmFilterControl::updateStates()
{
	updateConnected();
	updateFilters();
	updateEnumStates();
	updateMaximumValue();
	updateValue();
	updateMoving();
}

void BioXASCarbonFilterFarmFilterControl::updateConnected()
{
	bool isConnected = (
				upstreamFilter_ && upstreamFilter_->isConnected() &&
				downstreamFilter_ && downstreamFilter_->isConnected()
				);

	setConnected(isConnected);
}

void BioXASCarbonFilterFarmFilterControl::updateValue()
{
	double newValue = enumNames().indexOf("Unknown");

	if (isConnected()) {
		int newIndex = totalFilterIndex(upstreamFilter_->filterAt(int(upstreamFilter_->value())), downstreamFilter_->filterAt(int(downstreamFilter_->value())));

		if (newIndex > -1)
			newValue = newIndex;
	}

	setValue(newValue);
}

void BioXASCarbonFilterFarmFilterControl::updateMoving()
{
	bool isMoving = false;

	if (isConnected())
		isMoving = ( upstreamFilter_->isMoving() || downstreamFilter_->isMoving() );

	setIsMoving(isMoving);
}

void BioXASCarbonFilterFarmFilterControl::updateMaximumValue()
{
	setMaximumValue(enumNames().count() - 1);
}

void BioXASCarbonFilterFarmFilterControl::addFilter(double filter, double upstreamContribution, double downstreamContribution)
{
	if (filter >= 0 && !filters_.contains(filter)) {
		filters_.append(filter);
		setFilter(filter, upstreamContribution, downstreamContribution);
		updateEnumStates();

		emit filtersChanged();
	}
}

void BioXASCarbonFilterFarmFilterControl::setFilter(double filter, double upstreamContribution, double downstreamContribution)
{
	if (filters_.contains(filter)) {
		upstreamFilterMap_.insert(filter, upstreamContribution);
		downstreamFilterMap_.insert(filter, downstreamContribution);
	}
}

void BioXASCarbonFilterFarmFilterControl::clearFilters()
{
	filters_.clear();
	upstreamFilterMap_.clear();
	downstreamFilterMap_.clear();

	updateEnumStates();

	emit filtersChanged();
}

void BioXASCarbonFilterFarmFilterControl::updateFilters()
{
	// Clear filters.

	clearFilters();

	// Identify new list of possible filter options, found by iterating
	// through all options for each of the actuator filter controls.

	QList<double> upstreamFilters;
	QList<double> downstreamFilters;

	if (upstreamFilter_)
		upstreamFilters = upstreamFilter_->filters();

	if (downstreamFilter_)
		downstreamFilters = downstreamFilter_->filters();

	if (!upstreamFilters.isEmpty() && !downstreamFilters.isEmpty()) {

		foreach (double downstreamFilter, downstreamFilters) {
			foreach (double upstreamFilter, upstreamFilters) {

				double newFilter = totalFilter(upstreamFilter, downstreamFilter);
				addFilter(newFilter, upstreamFilter, downstreamFilter);
			}
		}
	}
}

void BioXASCarbonFilterFarmFilterControl::updateEnumStates()
{
	setEnumStates(generateEnumStates(filters_));
	setMoveEnumStates(generateMoveEnumStates(filters_));
}

AMAction3* BioXASCarbonFilterFarmFilterControl::createMoveAction(double setpoint)
{
	AMAction3 *action = 0;

	double filterSetpoint = filterAt(int(setpoint));
	double upstreamSetpoint = upstreamFilter_->indexOf(upstreamFilterMap_.value(setpoint, -1));
	double downstreamSetpoint = downstreamFilter_->indexOf(downstreamFilterMap_.value(setpoint, -1));

	if (upstreamFilter_->validSetpoint(upstreamSetpoint) && downstreamFilter_->validSetpoint(downstreamSetpoint)) {
		AMListAction3 *moveAction = new AMListAction3(new AMListActionInfo3(QString("Moving to effective filter %1mm").arg(filterSetpoint), QString("Moving to effective filter %1mm").arg(filterSetpoint)), AMListAction3::Parallel);
		moveAction->addSubAction(AMActionSupport::buildControlMoveAction(upstreamFilter_, upstreamSetpoint));
		moveAction->addSubAction(AMActionSupport::buildControlMoveAction(downstreamFilter_, downstreamSetpoint));

		action = moveAction;
	}

	return action;
}

QStringList BioXASCarbonFilterFarmFilterControl::generateEnumStates(QList<double> filterOptions)
{
	QStringList enumOptions = generateMoveEnumStates(filterOptions);

	// We always want to have an "Unknown" option--it's the default value.
	// Because it isn't a 'move enum' (we don't ever want to move to "Unknown")
	// it must be at the end of the enum list, after all of the move enums.

	enumOptions << "Unknown";

	return enumOptions;
}

QStringList BioXASCarbonFilterFarmFilterControl::generateMoveEnumStates(QList<double> filterOptions)
{
	QStringList moveOptions;

	foreach (double option, filterOptions)
		moveOptions << filterToString(option);

	return moveOptions;
}

int BioXASCarbonFilterFarmFilterControl::totalFilterIndex(double upstreamFilter, double downstreamFilter) const
{
	int result = indexOf( totalFilter(upstreamFilter, downstreamFilter) );
	return result;
}

double BioXASCarbonFilterFarmFilterControl::totalFilter(double upstreamFilter, double downstreamFilter) const
{
	double result = -1;

	if (upstreamFilter >= 0 && downstreamFilter >= 0)
		result = upstreamFilter + downstreamFilter;

	return result;
}

QString BioXASCarbonFilterFarmFilterControl::filterToString(double filter) const
{
	QString result = "Unknown";

	if (filters_.contains(filter))
		result = QString::number(filter, 'f', 0);

	return result;
}
