#include "BioXASCarbonFilterFarmActuatorFilterThicknessControl.h"

BioXASCarbonFilterFarmActuatorFilterThicknessControl::BioXASCarbonFilterFarmActuatorFilterThicknessControl(const QString &name, const QString &units, AMControl *positionControl, AMControl *statusControl, QObject *parent) :
	BioXASCarbonFilterFarmActuatorControl(name, units, parent)
{
	// Initialize inherited variables.

	value_ = 0;
	setpoint_ = 0;
	minimumValue_ = 0;
	maximumValue_ = 1000;

	setAllowsMovesWhileMoving(false);
	setContextKnownDescription("Filter Farm Actuator Filter Thickness Control");

	// Initialize member variables.

	windowControl_ = new BioXASCarbonFilterFarmActuatorWindowControl(positionControl, statusControl, this);

	// Current settings.

	setPositionControl(positionControl);
	setStatusControl(statusControl);

	updateStates();
}

BioXASCarbonFilterFarmActuatorFilterThicknessControl::~BioXASCarbonFilterFarmActuatorFilterThicknessControl()
{

}

bool BioXASCarbonFilterFarmActuatorFilterThicknessControl::validValue(double value) const
{
	return windowFilterMap_.values().contains(value);
}

QString BioXASCarbonFilterFarmActuatorFilterThicknessControl::filterThicknessToString(double filterThickness)
{
	return QString::number(filterThickness, 'f', 0);
}

double BioXASCarbonFilterFarmActuatorFilterThicknessControl::filterThicknessAtWindow(BioXASCarbonFilterFarm::Actuator::Window window)
{
	return windowFilterMap_.value(window, -1);
}

QList<BioXASCarbonFilterFarm::Actuator::Window> BioXASCarbonFilterFarmActuatorFilterThicknessControl::windowsWithFilterThickness(double filterThickness)
{
	return windowFilterMap_.keys(filterThickness);
}

void BioXASCarbonFilterFarmActuatorFilterThicknessControl::setPositionControl(AMControl *newControl)
{
	// The window control should be modified first, as the positionControlChanged() signal is
	// emitted when BioXASCarbonFilterFarmActuatorControl::setPosition() is called.

	windowControl_->setPositionControl(newControl);
	BioXASCarbonFilterFarmActuatorControl::setPositionControl(newControl);
}

void BioXASCarbonFilterFarmActuatorFilterThicknessControl::setStatusControl(AMControl *newControl)
{
	// The window control should be modified first, as the statusControlChanged() signal is
	// emitted when BioXASCarbonFilterFarmActuatorControl::setStatus() is called.

	windowControl_->setStatusControl(newControl);
	BioXASCarbonFilterFarmActuatorControl::setStatusControl(newControl);
}

void BioXASCarbonFilterFarmActuatorFilterThicknessControl::setWindowFilterThickness(BioXASCarbonFilterFarm::Actuator::Window window, double filterThickness)
{
	windowFilterMap_.insert(window, filterThickness);
}

void BioXASCarbonFilterFarmActuatorFilterThicknessControl::updateValue()
{
	if (isConnected())
		setValue( filterThicknessAtWindow(windowControl_->value()) );
	else
		setValue(-1);
}

AMAction3* BioXASCarbonFilterFarmActuatorFilterThicknessControl::createMoveAction(double filterThickness)
{
	// The tests to make sure this control is connected and the given filter thickness is valid
	// should have already been performed in this class' validSetpoint(double) and in
	// AMPseudoMotorControl::move(double).

	AMAction3 *moveAction = AMActionSupport::buildControlMoveAction(windowControl_, windowsWithFilterThickness(filterThickness).first());

	return moveAction;
}


