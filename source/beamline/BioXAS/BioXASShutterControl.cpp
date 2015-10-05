#include "BioXASShutterControl.h"

BioXASShuttersControl::BioXASShuttersControl(const QString &name, QObject *parent) :
	AMPseudoMotorControl(name, "", parent)
{
	// Initialize local variables.

	photonShutterUpstream_ = 0;
	photonShutterDownstream_ = 0;
	safetyShutter_ = 0;

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

bool BioXASShuttersControl::isOpen() const
{
	bool result = false;

	if (isConnected())
		result = (photonShutterUpstream_->isOpen() && photonShutterDownstream_->isOpen() && safetyShutter_->isOpen());

	return result;
}

bool BioXASShuttersControl::isClosed() const
{
	bool result = false;

	if (isConnected())
		result = (photonShutterUpstream_->isClosed() || photonShutterDownstream_->isClosed() || safetyShutter_->isClosed());

	return result;
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

void BioXASShuttersControl::setPhotonShutterUpstream(CLSBiStateControl *newControl)
{
	if (photonShutterUpstream_ != newControl) {

		if (photonShutterUpstream_)
			removeChildControl(photonShutterUpstream_);

		photonShutterUpstream_ = newControl;

		if (photonShutterUpstream_)
			addChildControl(photonShutterUpstream_);

		emit photonShutterUpstreamChanged(photonShutterUpstream_);
	}
}

void BioXASShuttersControl::setPhotonShutterDownstream(CLSBiStateControl *newControl)
{
	if (photonShutterDownstream_ != newControl) {

		if (photonShutterDownstream_)
			removeChildControl(photonShutterDownstream_);

		photonShutterDownstream_ = newControl;

		if (photonShutterDownstream_)
			addChildControl(photonShutterDownstream_);

		emit photonShutterDownstreamChanged(photonShutterDownstream_);
	}
}

void BioXASShuttersControl::setSafetyShutter(CLSBiStateControl *newControl)
{
	if (safetyShutter_ != newControl) {

		if (safetyShutter_)
			removeChildControl(safetyShutter_);

		safetyShutter_ = newControl;

		if (safetyShutter_)
			addChildControl(safetyShutter_);

		emit safetyShutterChanged(safetyShutter_);
	}
}

void BioXASShuttersControl::updateConnected()
{
	bool isConnected = (
				photonShutterUpstream_ && photonShutterUpstream_->isConnected() &&
				photonShutterDownstream_ && photonShutterDownstream_->isConnected() &&
				safetyShutter_ && safetyShutter_->isConnected()
				);

	setConnected(isConnected);
}

void BioXASShuttersControl::updateValue()
{
	double value = None;

	if (isConnected()) {

		if (isOpen())
			value = Open;

		else if (isClosed())
			value = Closed;

		else
			value = Between;
	}

	setValue(value);
}

void BioXASShuttersControl::updateMoving()
{
	if (isConnected()) {
		bool isMoving = (
					photonShutterUpstream_->isMoving() ||
					photonShutterDownstream_->isMoving() ||
					safetyShutter_->isMoving()
					);

		setIsMoving(isMoving);
	}
}

AMAction3* BioXASShuttersControl::createMoveAction(double newState)
{
	Q_UNUSED(newState)
	return 0;
}
