#include "BioXASCarbonFilterFarmActuatorFilterControl.h"
#include "actions3/AMActionSupport.h"

BioXASCarbonFilterFarmActuatorFilterControl::BioXASCarbonFilterFarmActuatorFilterControl(const QString &name, const QString &units, QObject *parent) :
	AMPseudoMotorControl(name, units, parent)
{
	// Initialize local variables.

	window_ = 0;

	// Initialize inherited variables.

	value_ = 0;
	setpoint_ = 0;
	minimumValue_ = 0;
	maximumValue_ = 1000;

	setAllowsMovesWhileMoving(false);
	setContextKnownDescription("Actuator Filter Thickness Control");
}

BioXASCarbonFilterFarmActuatorFilterControl::~BioXASCarbonFilterFarmActuatorFilterControl()
{

}

bool BioXASCarbonFilterFarmActuatorFilterControl::canMeasure() const
{
	bool result = false;

	if (isConnected())
		result = window_->canMeasure();

	return result;
}

bool BioXASCarbonFilterFarmActuatorFilterControl::canMove() const
{
	bool result = false;

	if (isConnected())
		result = window_->canMove();

	return result;
}

bool BioXASCarbonFilterFarmActuatorFilterControl::canStop() const
{
	bool result = false;

	if (isConnected())
		result = window_->canStop();

	return result;
}

bool BioXASCarbonFilterFarmActuatorFilterControl::validValue(double value) const
{
	return windowFilterMap_.values().contains(value);
}

QString BioXASCarbonFilterFarmActuatorFilterControl::filterThicknessToString(double filterThickness)
{
	return QString::number(filterThickness, 'f', 0);
}

double BioXASCarbonFilterFarmActuatorFilterControl::filterThicknessAtWindow(double window)
{
	return windowFilterMap_.value(window, -1);
}

QList<double> BioXASCarbonFilterFarmActuatorFilterControl::windowsWithFilterThickness(double filterThickness)
{
	return windowFilterMap_.keys(filterThickness);
}

void BioXASCarbonFilterFarmActuatorFilterControl::setWindow(BioXASCarbonFilterFarmActuatorWindowControl *newControl)
{
	if (window_ != newControl) {

		if (window_)
			removeChildControl(window_);

		window_ = newControl;

		if (window_)
			addChildControl(window_);

		emit windowChanged(window_);

		updateStates();
	}
}

void BioXASCarbonFilterFarmActuatorFilterControl::setWindowFilter(BioXASCarbonFilterFarmActuatorWindowControl::Value window, double filterThickness)
{
	windowFilterMap_.insert(window, filterThickness);
	setupEnumStates();
}

void BioXASCarbonFilterFarmActuatorFilterControl::updateConnected()
{
	bool connected = ( window_ && window_->isConnected() );
	setConnected(connected);
}

void BioXASCarbonFilterFarmActuatorFilterControl::updateValue()
{
	double value = -1;

	if (isConnected())
		value = filterThicknessAtWindow(window_->value());

	setValue(value);
}

void BioXASCarbonFilterFarmActuatorFilterControl::updateMoving()
{
	bool isMoving = false;

	if (isConnected())
		isMoving = (window_->isMoving());

	setIsMoving(isMoving);
}

AMAction3* BioXASCarbonFilterFarmActuatorFilterControl::createMoveAction(double filterThickness)
{
	// The tests to make sure this control is connected and the given filter thickness is valid
	// should have already been performed in this class' validSetpoint(double) and in
	// AMPseudoMotorControl::move(double).

	AMAction3 *moveAction = AMActionSupport::buildControlMoveAction(window_, windowsWithFilterThickness(filterThickness).first());

	return moveAction;
}

void BioXASCarbonFilterFarmActuatorFilterControl::setupEnumStates()
{
	QStringList enumList;

	foreach (double filterThickness, windowFilterMap_) {
		QString filterString = filterThicknessToString(filterThickness);

		if (!enumList.contains(filterString))
			enumList << filterString;
	}

	setEnumStates(enumList);
	setMoveEnumStates(enumList);
}
