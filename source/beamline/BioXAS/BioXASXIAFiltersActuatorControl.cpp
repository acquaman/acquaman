#include "BioXASXIAFiltersActuatorControl.h"
#include "actions3/AMActionSupport.h"

BioXASXIAFiltersActuatorControl::BioXASXIAFiltersActuatorControl(const QString &name, QObject *parent) :
	AMEnumeratedControl(name, "", parent)
{
	// Initialize inherited variables.

	setContextKnownDescription("Actuator State");
	setAllowsMovesWhileMoving(false);
	setAllowsDuplicateOptions(false);

	// Initialize class variables.

	status_ = 0;
	oprIn_ = 0;
	oprOut_ = 0;

	// Setup the allowed value options.

	addOption(In, stateToString(In));
	addOption(Out, stateToString(Out));
}

BioXASXIAFiltersActuatorControl::~BioXASXIAFiltersActuatorControl()
{

}

bool BioXASXIAFiltersActuatorControl::canMeasure() const
{
	bool result = false;

	if (status_)
		result = status_->canMeasure();

	return result;
}

bool BioXASXIAFiltersActuatorControl::canMove() const
{
	bool result = false;

	if (isConnected())
		result = oprIn_->canMove() && oprOut_->canMove();

	return result;
}

bool BioXASXIAFiltersActuatorControl::canStop() const
{
	bool result = false;

	if (isConnected())
		result = oprIn_->canStop() && oprOut_->canStop();

	return result;
}

QString BioXASXIAFiltersActuatorControl::stateToString(double state)
{
	QString result = "Unknown";

	switch (int(state)) {
	case In:
		result = "In";
		break;
	case Out:
		result = "Out";
		break;
	default:
		break;
	}

	return result;
}

void BioXASXIAFiltersActuatorControl::setStatus(AMReadOnlyPVControl *newControl)
{
	if (status_ != newControl) {

		if (status_)
			removeChildControl(status_);

		status_ = newControl;

		if (status_)
			addChildControl(status_);

		updateStates();

		emit statusChanged(status_);
	}
}

void BioXASXIAFiltersActuatorControl::setIn(AMPVControl *newControl)
{
	if (oprIn_ != newControl) {

		if (oprIn_)
			removeChildControl(oprIn_);

		oprIn_ = newControl;

		if (oprIn_)
			addChildControl(oprIn_);

		updateStates();

		emit inChanged(oprIn_);
	}
}

void BioXASXIAFiltersActuatorControl::setOut(AMPVControl *newControl)
{
	if (oprOut_ != newControl) {

		if (oprOut_)
			removeChildControl(oprOut_);

		oprOut_ = newControl;

		if (oprOut_)
			addChildControl(oprOut_);

		updateStates();

		emit outChanged(oprOut_);
	}
}

void BioXASXIAFiltersActuatorControl::updateConnected()
{
	bool isConnected = (
				status_ && status_->isConnected() &&
				oprIn_ && oprIn_->isConnected() &&
				oprOut_ && oprOut_->isConnected()
				);

	setConnected(isConnected);
}

void BioXASXIAFiltersActuatorControl::updateMoving()
{
	setIsMoving(false);
}

AMAction3* BioXASXIAFiltersActuatorControl::createMoveAction(double indexSetpoint)
{
	AMAction3 *result = 0;

	switch (int(indexSetpoint)) {
	case In:
		result = AMActionSupport::buildControlMoveAction(oprIn_, 1);
		break;
	case Out:
		result = AMActionSupport::buildControlMoveAction(oprOut_, 1);
		break;
	default:
		break;
	}

	return result;
}

int BioXASXIAFiltersActuatorControl::currentIndex() const
{
	// Initialize the new index to "Unknown".

	int currentIndex = enumNames().indexOf("Unknown");

	if (status_) {

		// Identify the index corresponding to the status's current value.

		currentIndex = int(status_->value());
	}

	return currentIndex;
}
