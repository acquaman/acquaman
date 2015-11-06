#include "BioXASCarbonFilterFarmActuatorWindowControl.h"
#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"

BioXASCarbonFilterFarmActuatorWindowControl::BioXASCarbonFilterFarmActuatorWindowControl(const QString &name, QObject *parent) :
	AMPseudoMotorEnumeratedControl(name, "", parent)
{
	// Initialize inherited variables.

	setContextKnownDescription("Actuator Window Control");
	setAllowsMovesWhileMoving(false);
}

BioXASCarbonFilterFarmActuatorWindowControl::~BioXASCarbonFilterFarmActuatorWindowControl()
{

}

QList<BioXASCarbonFilterFarmWindowOption*> BioXASCarbonFilterFarmActuatorWindowControl::windowsContaining(double position) const
{
	QList<BioXASCarbonFilterFarmWindowOption*> result;

	foreach (BioXASCarbonFilterFarmWindowOption *window, windows_) {
		if (window && window->containsPosition(position))
			result << window;
	}

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

BioXASCarbonFilterFarmWindowOption* BioXASCarbonFilterFarmActuatorWindowControl::windowAt(int index) const
{
	BioXASCarbonFilterFarmWindowOption *result = 0;

	if (index >= 0 && index < windows_.count())
		result = windows_.at(index);

	return result;
}

int BioXASCarbonFilterFarmActuatorWindowControl::indexOf(BioXASCarbonFilterFarmWindowOption *window) const
{
	return windows_.indexOf(window);
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

void BioXASCarbonFilterFarmActuatorWindowControl::updateValue()
{
	double newValue = enumNames().indexOf("Unknown");

	if (isConnected()) {
		QList<BioXASCarbonFilterFarmWindowOption*> windows = windowsContaining(control_->value());
		if (!windows.isEmpty())
			newValue = windows_.indexOf(windows.first());
	}

	setValue(newValue);
}

AMAction3* BioXASCarbonFilterFarmActuatorWindowControl::createMoveAction(double setpoint)
{
	AMAction3 *action = 0;

	BioXASCarbonFilterFarmWindowOption *window = windows_.at(int(setpoint));

	if (window && window->isValid())
		action = AMActionSupport::buildControlMoveAction(control_, window->setpointPosition() );

	return action;
}

QStringList BioXASCarbonFilterFarmActuatorWindowControl::generateEnumStates() const
{
	QStringList enumOptions = generateMoveEnumStates();

	// We want to have an "Unknown" option--it's the default value.
	// Because it isn't a 'move enum' (we don't ever want to move to "Unknown")
	// it must be at the end of the enum list, after all of the move enums.

	enumOptions << "Unknown";

	return enumOptions;
}

QStringList BioXASCarbonFilterFarmActuatorWindowControl::generateMoveEnumStates() const
{
	QStringList moveOptions;

	foreach (BioXASCarbonFilterFarmWindowOption *window, windows_) {
		if (window)
			moveOptions << window->name();
	}

	return moveOptions;
}
