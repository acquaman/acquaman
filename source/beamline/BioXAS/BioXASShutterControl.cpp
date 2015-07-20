#include "BioXASShutterControl.h"

BioXASShuttersControl::BioXASShuttersControl(const QString &name, QObject *parent) :
	AMPseudoMotorControl(name, "", parent)
{
	// Initialize local variables.

	photonShutterUpstreamFE_ = 0;
	photonShutterDownstreamFE_ = 0;
	safetyShutterFE_ = 0;
	safetyShutterES_ = 0;

	// Initialize inherited variables.

	value_ = None;
	setpoint_ = None;
	minimumValue_ = Open;
	maximumValue_ = None;

	setEnumStates(QStringList() << "Open" << "Closed" << "Between" << "None");
	setMoveEnumStates(QStringList() << "Open" << "Closed");
	setAllowsMovesWhileMoving(false);
	setContextKnownDescription("Shutters Control");

	// Current settings.

	updateStates();
}

BioXASShuttersControl::~BioXASShuttersControl()
{

}

bool BioXASShuttersControl::canMeasure() const
{
	return false;
}

bool BioXASShuttersControl::canMove() const
{
	return false;
}

bool BioXASShuttersControl::canStop() const
{
	return false;
}

bool BioXASShuttersControl::validValue(double value) const
{
	bool result = false;

	switch (int(value)) {
	case Open:
		result = true;
		break;
	case Closed:
		result = true;
		break;
	case Between:
		result = true;
		break;
	case None:
		result = true;
		break;
	default:
		break;
	}

	return result;
}

bool BioXASShuttersControl::validSetpoint(double value) const
{
	bool result = false;

	switch (int(value)) {
	case Open:
		result = true;
		break;
	case Closed:
		result = true;
		break;
	default:
		break;
	}

	return result;
}

void BioXASShuttersControl::setPhotonShutterUpstreamFE(CLSBiStateControl *newControl)
{
	if (photonShutterUpstreamFE_ != newControl) {

		if (photonShutterUpstreamFE_)
			removeChildControl(photonShutterUpstreamFE_);

		photonShutterUpstreamFE_ = newControl;

		if (photonShutterUpstreamFE_)
			addChildControl(photonShutterUpstreamFE_);

		emit photonShutterUpstreamFEChanged(photonShutterUpstreamFE_);
	}
}

void BioXASShuttersControl::setPhotonShutterDownstreamFE(CLSBiStateControl *newControl)
{
	if (photonShutterDownstreamFE_ != newControl) {

		if (photonShutterDownstreamFE_)
			removeChildControl(photonShutterDownstreamFE_);

		photonShutterDownstreamFE_ = newControl;

		if (photonShutterDownstreamFE_)
			addChildControl(photonShutterDownstreamFE_);

		emit photonShutterDownstreamFEChanged(photonShutterDownstreamFE_);
	}
}

void BioXASShuttersControl::setSafetyShutterFE(CLSBiStateControl *newControl)
{
	if (safetyShutterFE_ != newControl) {

		if (safetyShutterFE_)
			removeChildControl(safetyShutterFE_);

		safetyShutterFE_ = newControl;

		if (safetyShutterFE_)
			addChildControl(safetyShutterFE_);

		emit safetyShutterFEChanged(safetyShutterFE_);
	}
}

void BioXASShuttersControl::setSafetyShutterES(CLSBiStateControl *newControl)
{
	if (safetyShutterES_ != newControl) {

		if (safetyShutterES_)
			removeChildControl(safetyShutterES_);

		safetyShutterES_ = newControl;

		if (safetyShutterES_)
			addChildControl(safetyShutterES_);

		emit safetyShutterESChanged(safetyShutterES_);
	}
}

void BioXASShuttersControl::updateConnected()
{
	bool isConnected = (
				photonShutterUpstreamFE_ && photonShutterUpstreamFE_->isConnected() &&
				photonShutterDownstreamFE_ && photonShutterDownstreamFE_->isConnected() &&
				safetyShutterFE_ && safetyShutterFE_->isConnected() &&
				safetyShutterES_ && safetyShutterES_->isConnected()
				);

	setConnected(isConnected);
}

void BioXASShuttersControl::updateValue()
{
	double value = None;

	if (isConnected()) {

		if (photonShutterUpstreamFE_->isOpen() && photonShutterDownstreamFE_->isOpen() && safetyShutterFE_->isOpen() && safetyShutterES_->isOpen())
			value = Open;

		else
			value = Closed;
	}

	setValue(value);
}

void BioXASShuttersControl::updateMoving()
{
	if (isConnected()) {
		bool isMoving = (
					photonShutterUpstreamFE_->isMoving() ||
					photonShutterDownstreamFE_->isMoving() ||
					safetyShutterFE_->isMoving() ||
					safetyShutterES_->isMoving()
					);

		setIsMoving(isMoving);
	}
}

AMAction3* BioXASShuttersControl::createMoveAction(double newState)
{
	return 0;
}
