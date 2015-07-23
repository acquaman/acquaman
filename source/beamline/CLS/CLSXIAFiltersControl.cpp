#include "CLSXIAFiltersControl.h"

CLSXIAFiltersControl::CLSXIAFiltersControl(const QString &name, QObject *parent) :
	AMPseudoMotorControl(name, "mm", parent)
{

}

CLSXIAFiltersControl::~CLSXIAFiltersControl()
{

}

void CLSXIAFiltersControl::addFilterActuatorControl(AMPVControl *newControl, double filterThickness)
{
	if (newControl && !children_.contains(newControl) && !filterThicknessMap_.contains(newControl)) {
		addChildControl(newControl);
		filterThicknessMap_.insert(newControl, filterThickness);

		emit filterActuatorControlAdded(newControl);
	}
}

void CLSXIAFiltersControl::removeFilterActuatorControl(AMPVControl *control)
{
	if (control && children_.contains(control) && filterThicknessMap_.contains(control)) {
		removeChildControl(control);
		filterThicknessMap_.remove(control);

		emit filterActuatorControlRemoved(control);
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
