#include "BioXASCarbonFilterFarmWindowOption.h"

BioXASCarbonFilterFarmWindowOption::BioXASCarbonFilterFarmWindowOption(const QString &name, const AMNumber &setpoint, const AMNumber &minPosition, const AMNumber &maxPosition, const AMNumber &filter, QObject *parent) :
	QObject(parent)
{
	// Initialize class variables.

	name_ = "";
	setpointPosition_ = AMNumber::InvalidError;
	minPosition_ = AMNumber::InvalidError;
	maxPosition_ = AMNumber::InvalidError;
	filter_ = AMNumber::InvalidError;

	// Current settings.

	setName(name);
	setSetpointPosition(setpoint);
	setMinPosition(minPosition);
	setMaxPosition(maxPosition);
	setFilter(filter);
}

BioXASCarbonFilterFarmWindowOption::~BioXASCarbonFilterFarmWindowOption()
{

}

bool BioXASCarbonFilterFarmWindowOption::isValid() const
{
	bool valid = (
				!name_.isEmpty() &&
				setpointPosition_.isValid() &&
				minPosition_.isValid() &&
				maxPosition_.isValid() &&
				double(minPosition_) <= double(maxPosition_) &&
				filter_.isValid()
				);

	return valid;
}

bool BioXASCarbonFilterFarmWindowOption::containsPosition(double position) const
{
	bool inPosition = false;

	if (isValid())
		inPosition = ( position >= double(minPosition_) && position <= double(maxPosition_) );

	return inPosition;
}

void BioXASCarbonFilterFarmWindowOption::setName(const QString &newName)
{
	if (name_ != newName) {
		name_ = newName;
		emit nameChanged(name_);
	}
}

void BioXASCarbonFilterFarmWindowOption::setSetpointPosition(const AMNumber &newPosition)
{
	if (setpointPosition_ != newPosition) {
		setpointPosition_ = newPosition;
		emit setpointPositionChanged(setpointPosition_);
	}
}

void BioXASCarbonFilterFarmWindowOption::setMinPosition(const AMNumber &newPosition)
{
	if (minPosition_ != newPosition) {
		minPosition_ = newPosition;
		emit minPositionChanged(minPosition_);
	}
}

void BioXASCarbonFilterFarmWindowOption::setMaxPosition(const AMNumber &newPosition)
{
	if (maxPosition_ != newPosition) {
		maxPosition_ = newPosition;
		emit maxPositionChanged(maxPosition_);
	}
}

void BioXASCarbonFilterFarmWindowOption::setFilter(const AMNumber &newFilter)
{
	if (filter_ != newFilter) {
		filter_ = newFilter;
		emit filterChanged(filter_);
	}
}
