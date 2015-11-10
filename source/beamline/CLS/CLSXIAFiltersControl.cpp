#include "CLSXIAFiltersControl.h"

CLSXIAFiltersControl::CLSXIAFiltersControl(const QString &name, const QString &units, QObject *parent) :
	AMPseudoMotorControl(name, units, parent)
{
	// Initialize inherited variables.

	value_ = -1;
	setpoint_ = -1;
	minimumValue_ = 0;
	maximumValue_ = 100;

	setAllowsMovesWhileMoving(false);
}

CLSXIAFiltersControl::~CLSXIAFiltersControl()
{

}

void CLSXIAFiltersControl::addFilterActuatorControl(AMControl *newControl, double filterThickness)
{
	if (newControl && !validFilterActuator(newControl)) {
		addChildControl(newControl);
		setFilterThickness(newControl, filterThickness);
		emit filterActuatorControlAdded(newControl);
	}
}

void CLSXIAFiltersControl::removeFilterActuatorControl(AMControl *control)
{
	if (validFilterActuator(control)) {
		removeChildControl(control);
		removeFilterThickness(control);

		emit filterActuatorControlRemoved(control);
	}
}

void CLSXIAFiltersControl::setFilterThickness(AMControl *control, double newThickness)
{
	filterThicknessMap_.insert(control, newThickness);
}

void CLSXIAFiltersControl::removeFilterThickness(AMControl *control)
{
	filterThicknessMap_.remove(control);
}

void CLSXIAFiltersControl::updateValue()
{
	if (isConnected()) {
		setValue( activeFilterThickness() );
	}
}

void CLSXIAFiltersControl::updateMoving()
{
	bool movingChildFound = false;

	for (int i = 0, count = children_.count(); i < count && !movingChildFound; i++) {
		AMControl *child = children_.at(i);
		if (child->isMoving())
			movingChildFound = true;
	}

	setIsMoving(movingChildFound);
}

bool CLSXIAFiltersControl::validFilterActuator(AMControl *control) const
{
	bool result = false;

	if (control && children_.contains(control) && filterThicknessMap_.contains(control))
		result = true;

	return result;
}

double CLSXIAFiltersControl::filterThickness(AMControl *control) const
{
	double result = -1;

	if (validFilterActuator(control))
		result = filterThicknessMap_.value(control);

	return result;
}

double CLSXIAFiltersControl::activeFilterThickness(AMControl *control) const
{
	double result = -1;

	if (validFilterActuator(control)) {
		if (filterIsIn(control))
			result = filterThickness(control);
		else
			result = 0;
	}

	return result;
}

double CLSXIAFiltersControl::activeFilterThickness() const
{
	double result = 0;

	bool invalidActuatorFound = false;

	for (int i = 0, actuatorCount = children_.count(); i < actuatorCount && !invalidActuatorFound; i++) {
		AMControl *child = children_.at(i);

		if (validFilterActuator(child))
			result += activeFilterThickness(child);
		else
			invalidActuatorFound = true;
	}

	if (invalidActuatorFound)
		result = -1;

	return result;
}

void CLSXIAFiltersControl::setupEnumStates()
{
	QStringList enumList;

	foreach (double thickness, filterThicknessMap_)
		enumList << thicknessToString(thickness);

	setEnumStates(enumList);
	setMoveEnumStates(enumList);
}
