#include "BioXASCarbonFilterFarmActuatorWindowFilterThicknessControl.h"
#include "actions3/AMActionSupport.h"

BioXASCarbonFilterFarmActuatorWindowFilterThicknessControl::BioXASCarbonFilterFarmActuatorWindowFilterThicknessControl(const QString &name, const QString &units, BioXASCarbonFilterFarmActuatorWindowControl *windowControl, QObject *parent) :
	BioXASCarbonFilterFarmControl(name, units, parent)
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

	// Current settings.

	setWindowControl(windowControl);
}

BioXASCarbonFilterFarmActuatorWindowFilterThicknessControl::~BioXASCarbonFilterFarmActuatorWindowFilterThicknessControl()
{

}

bool BioXASCarbonFilterFarmActuatorWindowFilterThicknessControl::canMeasure() const
{
	bool result = false;

	if (isConnected())
		result = window_->canMeasure();

	return result;
}

bool BioXASCarbonFilterFarmActuatorWindowFilterThicknessControl::canMove() const
{
	bool result = false;

	if (isConnected())
		result = window_->canMove();

	return result;
}

bool BioXASCarbonFilterFarmActuatorWindowFilterThicknessControl::canStop() const
{
	bool result = false;

	if (isConnected())
		result = window_->canStop();

	return result;
}

bool BioXASCarbonFilterFarmActuatorWindowFilterThicknessControl::validValue(double value) const
{
	return windowFilterMap_.values().contains(value);
}

QString BioXASCarbonFilterFarmActuatorWindowFilterThicknessControl::filterThicknessToString(double filterThickness)
{
	return QString::number(filterThickness, 'f', 0);
}

double BioXASCarbonFilterFarmActuatorWindowFilterThicknessControl::filterThicknessAtWindow(BioXASCarbonFilterFarm::Actuator::Window window)
{
	return windowFilterMap_.value(window, -1);
}

QList<BioXASCarbonFilterFarm::Actuator::Window> BioXASCarbonFilterFarmActuatorWindowFilterThicknessControl::windowsWithFilterThickness(double filterThickness)
{
	return windowFilterMap_.keys(filterThickness);
}

void BioXASCarbonFilterFarmActuatorWindowFilterThicknessControl::setWindowControl(BioXASCarbonFilterFarmActuatorWindowControl *newControl)
{
	if (window_ != newControl) {

		if (window_)
			removeChildControl(window_);

		window_ = newControl;

		if (window_)
			addChildControl(window_);

		emit windowControlChanged(window_);

		updateStates();
	}
}

void BioXASCarbonFilterFarmActuatorWindowFilterThicknessControl::setWindowFilterThickness(BioXASCarbonFilterFarm::Actuator::Window window, double filterThickness)
{
	windowFilterMap_.insert(window, filterThickness);
}

void BioXASCarbonFilterFarmActuatorWindowFilterThicknessControl::updateConnected()
{
	bool connected = ( window_ && window_->isConnected() );
	setConnected(connected);
}

void BioXASCarbonFilterFarmActuatorWindowFilterThicknessControl::updateValue()
{
	double value = -1;

	if (isConnected())
		value = filterThicknessAtWindow(BioXASCarbonFilterFarm::Actuator::Window(window_->value()));

	setValue(value);
}

void BioXASCarbonFilterFarmActuatorWindowFilterThicknessControl::updateMoving()
{
	bool isMoving = false;

	if (isConnected())
		isMoving = (window_->isMoving());

	setIsMoving(isMoving);
}

AMAction3* BioXASCarbonFilterFarmActuatorWindowFilterThicknessControl::createMoveAction(double filterThickness)
{
	// The tests to make sure this control is connected and the given filter thickness is valid
	// should have already been performed in this class' validSetpoint(double) and in
	// AMPseudoMotorControl::move(double).

	AMAction3 *moveAction = AMActionSupport::buildControlMoveAction(window_, windowsWithFilterThickness(filterThickness).first());

	return moveAction;
}

void BioXASCarbonFilterFarmActuatorWindowFilterThicknessControl::setupEnumStates()
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
