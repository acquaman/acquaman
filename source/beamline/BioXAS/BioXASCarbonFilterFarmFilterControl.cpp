#include "BioXASCarbonFilterFarmFilterControl.h"
#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"

BioXASCarbonFilterFarmFilterControl::BioXASCarbonFilterFarmFilterControl(const QString &name, const QString &units, QObject *parent) :
	AMEnumeratedControl(name, units, parent)
{
	// Initialize inherited variables.

	setContextKnownDescription("Filter Control");
	setAllowsMovesWhileMoving(false);
	setAllowsDuplicateOptions(false);

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

double BioXASCarbonFilterFarmFilterControl::filterAt(int index) const
{
	return indexFilterMap_.value(index, -1);
}

void BioXASCarbonFilterFarmFilterControl::setUpstreamFilter(BioXASCarbonFilterFarmActuatorFilterControl *newControl)
{
	if (upstreamFilter_ != newControl) {

		if (upstreamFilter_)
			removeChildControl(upstreamFilter_); // disconnects from all signals.


		upstreamFilter_ = newControl;

		if (upstreamFilter_) {
			addChildControl(upstreamFilter_);

			connect( upstreamFilter_, SIGNAL(filtersChanged()), this, SLOT(updateOptions()) );
		}

		updateStates();

		emit upstreamFilterChanged(upstreamFilter_);
	}
}

void BioXASCarbonFilterFarmFilterControl::setDownstreamFilter(BioXASCarbonFilterFarmActuatorFilterControl *newControl)
{
	if (downstreamFilter_ != newControl) {

		if (downstreamFilter_)
			removeChildControl(downstreamFilter_); // disconnects from all signals.


		downstreamFilter_ = newControl;

		if (downstreamFilter_) {
			addChildControl(downstreamFilter_);

			connect( downstreamFilter_, SIGNAL(filtersChanged()), this, SLOT(updateOptions()) );
		}

		updateStates();

		emit downstreamFilterChanged(downstreamFilter_);
	}
}

void BioXASCarbonFilterFarmFilterControl::updateConnected()
{
	bool isConnected = (
				upstreamFilter_ && upstreamFilter_->isConnected() &&
				downstreamFilter_ && downstreamFilter_->isConnected()
				);

	setConnected(isConnected);
}

void BioXASCarbonFilterFarmFilterControl::updateOptions()
{
	// Clear filters.

	clearOptions();

	// Identify new list of possible filter options, found by iterating
	// through all options for each of the actuator filter controls.

	QList<int> upstreamIndices;
	QList<int> downstreamIndices;

	if (upstreamFilter_)
		upstreamIndices = upstreamFilter_->moveIndices();

	if (downstreamFilter_)
		downstreamIndices = downstreamFilter_->moveIndices();

	foreach (int downstreamIndex, downstreamIndices) { // Aesthetics, we want the lower filters (upstream) listed first.
		foreach (int upstreamIndex, upstreamIndices) {

			double newFilter = totalFilterByIndices(upstreamIndex, downstreamIndex);

			if (newFilter != -1)
				addFilterOption(QString::number(newFilter, 'f', 0), newFilter, upstreamIndex, downstreamIndex);
		}
	}
}

void BioXASCarbonFilterFarmFilterControl::updateMoving()
{
	bool isMoving = false;

	if (isConnected())
		isMoving = ( upstreamFilter_->isMoving() || downstreamFilter_->isMoving() );

	setIsMoving(isMoving);
}

void BioXASCarbonFilterFarmFilterControl::addFilterOption(const QString &optionString, double filter, int upstreamFilterIndex, int downstreamFilterIndex)
{
	addFilterOption(indices_.count(), optionString, filter, upstreamFilterIndex, downstreamFilterIndex);
}

void BioXASCarbonFilterFarmFilterControl::addFilterOption(int index, const QString &optionString, double filter, int upstreamFilterIndex, int downstreamFilterIndex)
{
	if (AMEnumeratedControl::addOption(index, optionString)) {
		indexFilterMap_.insert(index, filter);
		indexUpstreamFilterIndexMap_.insert(index, upstreamFilterIndex);
		indexDownstreamFilterIndexMap_.insert(index, downstreamFilterIndex);

		emit filtersChanged();
	}
}

bool BioXASCarbonFilterFarmFilterControl::removeOption(int index)
{
	bool result = false;

	if (AMEnumeratedControl::removeOption(index)) {
		indexFilterMap_.remove(index);
		indexUpstreamFilterIndexMap_.remove(index);
		indexDownstreamFilterIndexMap_.remove(index);

		emit filtersChanged();

		result = true;
	}

	return result;
}

bool BioXASCarbonFilterFarmFilterControl::clearOptions()
{
	bool result = false;

	if (AMEnumeratedControl::clearOptions()) {
		indexFilterMap_.clear();
		indexUpstreamFilterIndexMap_.clear();
		indexDownstreamFilterIndexMap_.clear();

		emit filtersChanged();

		result = true;
	}

	return result;
}

AMAction3* BioXASCarbonFilterFarmFilterControl::createMoveAction(double setpoint)
{
	AMAction3 *action = 0;

	int filterIndex = int(setpoint);
	int upstreamIndex = indexUpstreamFilterIndexMap_.value(filterIndex, -1);
	int downstreamIndex = indexDownstreamFilterIndexMap_.value(filterIndex, -1);

	if (upstreamFilter_->validSetpoint(upstreamIndex) && downstreamFilter_->validSetpoint(downstreamIndex)) {
		AMListAction3 *moveAction = new AMListAction3(new AMListActionInfo3(QString("Moving to effective filter %1%2").arg(filterAt(filterIndex)).arg(units()), QString("Moving to effective filter %1%2").arg(filterAt(filterIndex)).arg(units())), AMListAction3::Parallel);
		moveAction->addSubAction(AMActionSupport::buildControlMoveAction(upstreamFilter_, upstreamIndex));
		moveAction->addSubAction(AMActionSupport::buildControlMoveAction(downstreamFilter_, downstreamIndex));

		action = moveAction;
	}

	return action;
}

int BioXASCarbonFilterFarmFilterControl::currentIndex() const
{
	// Initialize the new index to "Unknown."

	int index = enumNames().indexOf("Unknown");

	if (isConnected()) {

		// Identify the index corresponding to the current values for the upstream and downstream filters.

		double currentFilter = totalFilterByIndices(int(upstreamFilter_->value()), int(downstreamFilter_->value()));

		if (currentFilter >= 0) {
			QList<int> possibleIndices = indexFilterMap_.keys(currentFilter);

			if (!possibleIndices.isEmpty())
				index = possibleIndices.first();
		}
	}

	return index;
}

double BioXASCarbonFilterFarmFilterControl::totalFilterByIndices(int upstreamIndex, int downstreamIndex) const
{
	double result = -1;

	if (isConnected()) {
		double upstreamFilter = upstreamFilter_->filterAt(upstreamIndex);
		double downstreamFilter = downstreamFilter_->filterAt(downstreamIndex);

		result = totalFilter(upstreamFilter, downstreamFilter);
	}

	return result;
}

double BioXASCarbonFilterFarmFilterControl::totalFilter(double upstreamFilter, double downstreamFilter) const
{
	double result = -1;

	if (upstreamFilter >= 0 && downstreamFilter >= 0)
		result = upstreamFilter + downstreamFilter;

	return result;
}
