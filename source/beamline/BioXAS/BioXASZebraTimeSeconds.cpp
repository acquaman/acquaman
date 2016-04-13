#include "BioXASZebraTimeSeconds.h"
#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"

BioXASZebraTimeSeconds::BioXASZebraTimeSeconds(const QString &name, QObject *parent) :
	AMPseudoMotorControl(name, "s", parent)
{
	// Initialize inherited variables.

	setTolerance(0.001);
	setContextKnownDescription("ZebraTime");

	// Initialize class variables.

	timeValue_ = 0;
	timeUnits_ = 0;

	// Current settings.

	setMinimumValue(BIOXASZEBRATIMESECONDS_VALUE_MIN);
	setMaximumValue(BIOXASZEBRATIMESECONDS_VALUE_MAX);

	updateStates();
}

BioXASZebraTimeSeconds::~BioXASZebraTimeSeconds()
{

}

bool BioXASZebraTimeSeconds::canMeasure() const
{
	bool result = (
				timeValue_ && timeValue_->canMeasure() &&
				timeUnits_ && timeUnits_->canMeasure()
				);

	return result;
}

bool BioXASZebraTimeSeconds::canMove() const
{
	bool result = (
				timeValue_ && timeValue_->canMove() &&
				timeUnits_ && timeUnits_->canMove()
				);

	return result;
}

void BioXASZebraTimeSeconds::setTimeValueControl(AMPVControl *newControl)
{
	if (timeValue_ != newControl) {

		if (timeValue_)
			removeChildControl(timeValue_);

		timeValue_ = newControl;

		if (timeValue_)
			addChildControl(timeValue_);

		updateStates();

		emit timeValueControlChanged(timeValue_);
	}
}

void BioXASZebraTimeSeconds::setTimeUnitsControl(AMPVControl *newControl)
{
	if (timeUnits_ != newControl) {

		if (timeUnits_)
			removeChildControl(timeUnits_);

		timeUnits_ = newControl;

		if (timeUnits_)
			addChildControl(timeUnits_);

		emit timeUnitsControlChanged(timeUnits_);
	}
}

void BioXASZebraTimeSeconds::updateConnected()
{
	setConnected( childrenConnected() );
}

void BioXASZebraTimeSeconds::updateValue()
{
	if (canMeasure() && !moveInProgress()) {
		double newValue = convertTimeValue(timeValue_->value(), timeUnits_->value(), Seconds);
		setValue(newValue);
	}
}

void BioXASZebraTimeSeconds::updateMoving()
{
	setIsMoving( childrenMoving() );
}

bool BioXASZebraTimeSeconds::validTimeValue(double timeValue) const
{
	bool result = false;

	if (timeValue >= BIOXASZEBRATIMESECONDS_TIME_VALUE_MIN && timeValue <= BIOXASZEBRATIMESECONDS_TIME_VALUE_MAX)
		result = true;

	return result;
}

bool BioXASZebraTimeSeconds::validTimeUnits(double timeUnits) const
{
	bool result = false;

	switch (int(timeUnits)) {
	case MSeconds:
		result = true;
		break;
	case Seconds:
		result = true;
		break;
	case DSeconds:
		result = true;
		break;
	default:
		break;
	}

	return result;
}

double BioXASZebraTimeSeconds::convertTimeValue(double timeValue, double timeUnits, double desiredTimeUnits) const
{
	double result = timeValue;

	if (timeUnits != desiredTimeUnits) {

		// Convert time value to seconds.

		double timeValueSeconds = 0;

		if (timeUnits == MSeconds) // from ms
			timeValueSeconds = timeValue / 1000.0;
		else if (timeUnits == Seconds) // from s
			timeValueSeconds = timeValue;
		else if (timeUnits == DSeconds) // from 10s
			timeValueSeconds = timeValue * 10.0;

		// Complete conversion to the desired units.

		if (desiredTimeUnits == MSeconds) // to ms
			result = timeValueSeconds * 1000.0;
		else if (desiredTimeUnits == Seconds) // to s
			result = timeValueSeconds;
		else if (desiredTimeUnits == DSeconds) // to 10s
			result = timeValueSeconds / 10.0;
	}

	return result;
}

bool BioXASZebraTimeSeconds::childrenConnected() const
{
	bool connected = (
				timeValue_ && timeValue_->isConnected() &&
				timeUnits_ && timeUnits_->isConnected()
				);

	return connected;
}

bool BioXASZebraTimeSeconds::childrenMoving() const
{
	bool moving = (
				(timeValue_ && timeValue_->isMoving()) ||
				(timeUnits_ && timeUnits_->isMoving())
				);

	return moving;
}

AMAction3* BioXASZebraTimeSeconds::createMoveAction(double setpointS)
{
	AMAction3 *result = 0;

	double timeValueSetpoint = -1;
	double timeUnitsSetpoint = -1;

	// Resolve the given setpoint in seconds to other time values and units.

	double setpointMS = convertTimeValue(setpointS, Seconds, MSeconds);
	double setpointDS = convertTimeValue(setpointS, Seconds, DSeconds);

	// Starting with finest resolution, find the first valid setpoint.

	if (validTimeValue(setpointMS)) {
		timeValueSetpoint = setpointMS;
		timeUnitsSetpoint = MSeconds;

	} else if (validTimeValue(setpointS)) {
		timeValueSetpoint = setpointS;
		timeUnitsSetpoint = Seconds;

	} else if (validTimeValue(setpointDS)) {
		timeValueSetpoint = setpointDS;
		timeUnitsSetpoint = DSeconds;
	}

	// If the time value and units setpoints are valid, create and return valid action.

	if (validTimeValue(timeValueSetpoint) && validTimeUnits(timeUnitsSetpoint)) {
		AMListAction3 *moveAction = new AMListAction3(new AMListActionInfo3(QString("Moving %1").arg(name()), QString("Moving %1").arg(name())), AMListAction3::Sequential);
		moveAction->addSubAction(AMActionSupport::buildControlMoveAction(timeUnits_, timeUnitsSetpoint));
		moveAction->addSubAction(AMActionSupport::buildControlMoveAction(timeValue_, timeValueSetpoint));

		result = moveAction;
	}

	return result;
}
