#include "BioXASCarbonFilterFarmActuatorWindowControl.h"
#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"

BioXASCarbonFilterFarmActuatorWindowControl::BioXASCarbonFilterFarmActuatorWindowControl(const QString &name, QObject *parent) :
	AMPseudoMotorControl(name, "", parent)
{
	// Initialize local variables.

	currentPosition_ = 0;

	// Initialize inherited variables.

	value_ = -1;
	setpoint_ = -1;
	minimumValue_ = 0;
	maximumValue_ = 0;

	setContextKnownDescription("Actuator Window Control");
	setAllowsMovesWhileMoving(false);
}

BioXASCarbonFilterFarmActuatorWindowControl::~BioXASCarbonFilterFarmActuatorWindowControl()
{

}

bool BioXASCarbonFilterFarmActuatorWindowControl::canMeasure() const
{
	bool result = false;

	if (isConnected())
		result = currentPosition_->canMeasure();

	return result;
}

bool BioXASCarbonFilterFarmActuatorWindowControl::canMove() const
{
	bool result = false;

	if (isConnected())
		result = currentPosition_->canMove();

	return result;
}

bool BioXASCarbonFilterFarmActuatorWindowControl::canStop() const
{
	bool result = false;

	if (isConnected())
		result = currentPosition_->canStop();

	return result;
}

bool BioXASCarbonFilterFarmActuatorWindowControl::validValue(double value) const
{
	return (value >= 0 && value < enumNames().count());
}

bool BioXASCarbonFilterFarmActuatorWindowControl::validSetpoint(double value) const
{
	return (value >= 0 && value < moveEnumNames().count());
}

BioXASCarbonFilterFarmWindowOption* BioXASCarbonFilterFarmActuatorWindowControl::windowAt(int index) const
{
	BioXASCarbonFilterFarmWindowOption *result = 0;

	if (index >= 0 && index < windows_.count())
		result = windows_.at(index);

	return result;
}

QList<BioXASCarbonFilterFarmWindowOption*> BioXASCarbonFilterFarmActuatorWindowControl::windowsWithFilter(double filter) const
{
	QList<BioXASCarbonFilterFarmWindowOption*> result;

	foreach (BioXASCarbonFilterFarmWindowOption *option, windows_) {
		if (option && option->isValid() && double(option->filter()) == filter)
			result << option;
	}

	return result;
}

int BioXASCarbonFilterFarmActuatorWindowControl::indexOf(BioXASCarbonFilterFarmWindowOption *window) const
{
	return windows_.indexOf(window);
}

int BioXASCarbonFilterFarmActuatorWindowControl::indexOf(const QString windowString) const
{
	return enumNames().indexOf(windowString);
}

void BioXASCarbonFilterFarmActuatorWindowControl::setCurrentPosition(AMControl *newControl)
{
	if (currentPosition_ != newControl) {

		if (currentPosition_)
			removeChildControl(currentPosition_);

		currentPosition_ = newControl;

		if (currentPosition_)
			addChildControl(currentPosition_);

		updateStates();

		emit currentPositionChanged(currentPosition_);
	}
}

void BioXASCarbonFilterFarmActuatorWindowControl::addWindow(BioXASCarbonFilterFarmWindowOption *newOption)
{
	if (!windows_.contains(newOption) && newOption && newOption->isValid()) {
		windows_.append(newOption);
		updateEnumStates();

		connect( newOption, SIGNAL(nameChanged(QString)), this, SLOT(updateEnumStates()) );

		emit windowsChanged();
	}
}

void BioXASCarbonFilterFarmActuatorWindowControl::removeWindow(BioXASCarbonFilterFarmWindowOption *option)
{
	if (windows_.contains(option)) {

		disconnect( option, 0, this, 0 );

		windows_.removeOne(option);
		updateEnumStates();

		emit windowsChanged();
	}
}

void BioXASCarbonFilterFarmActuatorWindowControl::clearWindows()
{
	foreach (BioXASCarbonFilterFarmWindowOption *window, windows_)
		removeWindow(window);
}

void BioXASCarbonFilterFarmActuatorWindowControl::updateStates()
{
	updateConnected();
	updateEnumStates();
	updateMaximumValue();
	updateValue();
	updateMoving();
}

void BioXASCarbonFilterFarmActuatorWindowControl::updateConnected()
{
	bool isConnected = ( currentPosition_ && currentPosition_->isConnected() );
	setConnected(isConnected);
}

void BioXASCarbonFilterFarmActuatorWindowControl::updateValue()
{
	double newValue = enumNames().indexOf("Unknown");

	if (isConnected()) {
		QList<BioXASCarbonFilterFarmWindowOption*> windows = windowsAtPosition(currentPosition_->value());
		if (!windows.isEmpty())
			newValue = windows_.indexOf(windows.first());
	}

	setValue(newValue);
}

void BioXASCarbonFilterFarmActuatorWindowControl::updateMoving()
{
	bool isMoving = false;

	if (isConnected())
		isMoving = currentPosition_->isMoving();

	setIsMoving(isMoving);
}

void BioXASCarbonFilterFarmActuatorWindowControl::updateMaximumValue()
{
	setMaximumValue(enumNames().count() - 1);
}

void BioXASCarbonFilterFarmActuatorWindowControl::updateEnumStates()
{
	setEnumStates(generateEnumStates(windows_));
	setMoveEnumStates(generateMoveEnumStates(windows_));
}

AMAction3* BioXASCarbonFilterFarmActuatorWindowControl::createMoveAction(double setpoint)
{
	AMAction3 *action = 0;

	BioXASCarbonFilterFarmWindowOption *window = windows_.at(int(setpoint));

	if (window && window->isValid())
		action = AMActionSupport::buildControlMoveAction(currentPosition_, window->setpointPosition() );

	return action;
}

QStringList BioXASCarbonFilterFarmActuatorWindowControl::generateEnumStates(const QList<BioXASCarbonFilterFarmWindowOption*> &windowOptions) const
{
	QStringList enumOptions = generateMoveEnumStates(windowOptions);

	// We always want to have an "Unknown" option--it's the default value.
	// Because it isn't a 'move enum' (we don't ever want to move to "Unknown")
	// it must be at the end of the enum list, after all of the move enums.

	enumOptions << "Unknown";

	return enumOptions;
}

QStringList BioXASCarbonFilterFarmActuatorWindowControl::generateMoveEnumStates(const QList<BioXASCarbonFilterFarmWindowOption*> &windowOptions) const
{
	QStringList moveOptions;

	foreach (BioXASCarbonFilterFarmWindowOption *window, windowOptions) {
		if (window)
			moveOptions << window->name();
	}

	return moveOptions;
}

QList<BioXASCarbonFilterFarmWindowOption*> BioXASCarbonFilterFarmActuatorWindowControl::windowsAtPosition(double position) const
{
	QList<BioXASCarbonFilterFarmWindowOption*> result;

	foreach (BioXASCarbonFilterFarmWindowOption *window, windows_) {
		if (window && window->containsPosition(position))
			result << window;
	}

	return result;
}
