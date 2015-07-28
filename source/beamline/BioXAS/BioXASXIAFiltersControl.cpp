#include "BioXASXIAFiltersControl.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

BioXASXIAFiltersControl::BioXASXIAFiltersControl(const QString &name, QObject *parent) :
	CLSXIAFiltersControl(name, "mm", parent)
{
	// Initialize local variables.

	firstFilterActuator_ = 0;
	secondFilterActuator_ = 0;
	thirdFilterActuator_ = 0;
	fourthFilterActuator_ = 0;
}

BioXASXIAFiltersControl::~BioXASXIAFiltersControl()
{

}

bool BioXASXIAFiltersControl::canMeasure() const
{
	bool result = false;

	if (isConnected()) {
		result = (
					firstFilterActuator_->canMeasure() &&
					secondFilterActuator_->canMeasure() &&
					thirdFilterActuator_->canMeasure() &&
					fourthFilterActuator_->canMeasure()
					);
	}

	return result;
}

bool BioXASXIAFiltersControl::canMove() const
{
	bool result = false;

	if (isConnected()) {
		result = (
					firstFilterActuator_->canMove() &&
					secondFilterActuator_->canMove() &&
					thirdFilterActuator_->canMove() &&
					fourthFilterActuator_->canMove()
					);
	}

	return result;
}

bool BioXASXIAFiltersControl::canStop() const
{
	bool result = false;

	if (isConnected()) {
		result = (
					firstFilterActuator_->canStop() &&
					secondFilterActuator_->canStop() &&
					thirdFilterActuator_->canStop() &&
					fourthFilterActuator_->canStop()
					);
	}

	return result;
}

bool BioXASXIAFiltersControl::validValue(double value) const
{
	bool result = false;

	if (value == 0.0 ||
			value == 0.25 ||
			value == 0.5 ||
			value == 0.75 ||
			value == 1.0 ||
			value == 1.25 ||
			value == 1.5 ||
			value == 1.75 ||
			value == 10.0
			)
		result = true;

	return result;
}

void BioXASXIAFiltersControl::setFirstFilterActuatorControl(CLSBiStateControl *newControl, double filterThickness)
{
	if (firstFilterActuator_ != newControl) {

		if (firstFilterActuator_)
			removeFilterActuatorControl(firstFilterActuator_);

		firstFilterActuator_ = newControl;

		if (firstFilterActuator_)
			addFilterActuatorControl(firstFilterActuator_, filterThickness);

		emit firstFilterActuatorControlChanged(firstFilterActuator_);
	}
}

void BioXASXIAFiltersControl::setSecondFilterActuatorControl(CLSBiStateControl *newControl, double filterThickness)
{
	if (secondFilterActuator_ != newControl) {

		if (secondFilterActuator_)
			removeFilterActuatorControl(secondFilterActuator_);

		secondFilterActuator_ = newControl;

		if (secondFilterActuator_)
			addFilterActuatorControl(secondFilterActuator_, filterThickness);

		emit secondFilterActuatorControlChanged(secondFilterActuator_);
	}
}

void BioXASXIAFiltersControl::setThirdFilterActuatorControl(CLSBiStateControl *newControl, double filterThickness)
{
	if (thirdFilterActuator_ != newControl) {

		if (thirdFilterActuator_)
			removeFilterActuatorControl(thirdFilterActuator_);

		thirdFilterActuator_ = newControl;

		if (thirdFilterActuator_)
			addFilterActuatorControl(thirdFilterActuator_, filterThickness);

		emit thirdFilterActuatorControlChanged(thirdFilterActuator_);
	}
}

void BioXASXIAFiltersControl::setFourthFilterActuatorControl(CLSBiStateControl *newControl, double filterThickness)
{
	if (fourthFilterActuator_ != newControl) {

		if (fourthFilterActuator_)
			removeFilterActuatorControl(fourthFilterActuator_);

		fourthFilterActuator_ = newControl;

		if (fourthFilterActuator_)
			addFilterActuatorControl(fourthFilterActuator_, filterThickness);

		emit fourthFilterActuatorControlChanged(fourthFilterActuator_);
	}
}

void BioXASXIAFiltersControl::updateConnected()
{
	bool isConnected = (
				firstFilterActuator_ && firstFilterActuator_->isConnected() &&
				secondFilterActuator_ && secondFilterActuator_->isConnected() &&
				thirdFilterActuator_ && thirdFilterActuator_->isConnected() &&
				fourthFilterActuator_ && fourthFilterActuator_->isConnected()
				);

	setConnected(isConnected);
}

bool BioXASXIAFiltersControl::filterIsIn(AMControl *control) const
{
	bool result = false;

	CLSBiStateControl *actuator = qobject_cast<CLSBiStateControl*>(control);

	if (actuator && validFilterActuator(actuator))
		result = ( int(actuator->value()) == Filter::In );

	return result;
}

bool BioXASXIAFiltersControl::filterIsOut(AMControl *control) const
{
	bool result = false;

	CLSBiStateControl *actuator = qobject_cast<CLSBiStateControl*>(control);

	if (actuator && validFilterActuator(actuator))
		result = ( int(actuator->value()) == Filter::Out );

	return result;
}

AMAction3* BioXASXIAFiltersControl::createMoveAction(double setpoint)
{
	AMListAction3 *result = new AMListAction3(new AMListActionInfo3(QString("Move to %1").arg(setpoint), QString("Move to %1").arg(setpoint)), AMListAction3::Parallel);

	if (setpoint == 0) {
		result->addSubAction(AMActionSupport::buildControlMoveAction(firstFilterActuator_, Filter::Out));
		result->addSubAction(AMActionSupport::buildControlMoveAction(secondFilterActuator_, Filter::Out));
		result->addSubAction(AMActionSupport::buildControlMoveAction(thirdFilterActuator_, Filter::Out));
		result->addSubAction(AMActionSupport::buildControlMoveAction(fourthFilterActuator_, Filter::Out));

	} else if (setpoint == 0.25) {
		result->addSubAction(AMActionSupport::buildControlMoveAction(firstFilterActuator_, Filter::In));
		result->addSubAction(AMActionSupport::buildControlMoveAction(secondFilterActuator_, Filter::Out));
		result->addSubAction(AMActionSupport::buildControlMoveAction(thirdFilterActuator_, Filter::Out));
		result->addSubAction(AMActionSupport::buildControlMoveAction(fourthFilterActuator_, Filter::Out));

	} else if (setpoint == 0.5) {
		result->addSubAction(AMActionSupport::buildControlMoveAction(firstFilterActuator_, Filter::Out));
		result->addSubAction(AMActionSupport::buildControlMoveAction(secondFilterActuator_, Filter::In));
		result->addSubAction(AMActionSupport::buildControlMoveAction(thirdFilterActuator_, Filter::Out));
		result->addSubAction(AMActionSupport::buildControlMoveAction(fourthFilterActuator_, Filter::Out));

	} else if (setpoint == 0.75) {
		result->addSubAction(AMActionSupport::buildControlMoveAction(firstFilterActuator_, Filter::In));
		result->addSubAction(AMActionSupport::buildControlMoveAction(secondFilterActuator_, Filter::In));
		result->addSubAction(AMActionSupport::buildControlMoveAction(thirdFilterActuator_, Filter::Out));
		result->addSubAction(AMActionSupport::buildControlMoveAction(fourthFilterActuator_, Filter::Out));

	} else if (setpoint == 1.0) {
		result->addSubAction(AMActionSupport::buildControlMoveAction(firstFilterActuator_, Filter::Out));
		result->addSubAction(AMActionSupport::buildControlMoveAction(secondFilterActuator_, Filter::Out));
		result->addSubAction(AMActionSupport::buildControlMoveAction(thirdFilterActuator_, Filter::In));
		result->addSubAction(AMActionSupport::buildControlMoveAction(fourthFilterActuator_, Filter::Out));

	} else if (setpoint == 1.25) {
		result->addSubAction(AMActionSupport::buildControlMoveAction(firstFilterActuator_, Filter::In));
		result->addSubAction(AMActionSupport::buildControlMoveAction(secondFilterActuator_, Filter::Out));
		result->addSubAction(AMActionSupport::buildControlMoveAction(thirdFilterActuator_, Filter::In));
		result->addSubAction(AMActionSupport::buildControlMoveAction(fourthFilterActuator_, Filter::Out));

	} else if (setpoint == 1.5) {
		result->addSubAction(AMActionSupport::buildControlMoveAction(firstFilterActuator_, Filter::Out));
		result->addSubAction(AMActionSupport::buildControlMoveAction(secondFilterActuator_, Filter::In));
		result->addSubAction(AMActionSupport::buildControlMoveAction(thirdFilterActuator_, Filter::In));
		result->addSubAction(AMActionSupport::buildControlMoveAction(fourthFilterActuator_, Filter::Out));

	} else if (setpoint == 1.75) {
		result->addSubAction(AMActionSupport::buildControlMoveAction(firstFilterActuator_, Filter::In));
		result->addSubAction(AMActionSupport::buildControlMoveAction(secondFilterActuator_, Filter::In));
		result->addSubAction(AMActionSupport::buildControlMoveAction(thirdFilterActuator_, Filter::In));
		result->addSubAction(AMActionSupport::buildControlMoveAction(fourthFilterActuator_, Filter::Out));

	} else if (setpoint == 10.0) {
		result->addSubAction(AMActionSupport::buildControlMoveAction(firstFilterActuator_, Filter::Out));
		result->addSubAction(AMActionSupport::buildControlMoveAction(secondFilterActuator_, Filter::Out));
		result->addSubAction(AMActionSupport::buildControlMoveAction(thirdFilterActuator_, Filter::Out));
		result->addSubAction(AMActionSupport::buildControlMoveAction(fourthFilterActuator_, Filter::In));

	} else {
		result->deleteLater();
		result = 0;
	}

	return result;
}
