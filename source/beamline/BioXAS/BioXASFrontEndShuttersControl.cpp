#include "BioXASFrontEndShuttersControl.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

BioXASFrontEndShuttersControl:: BioXASFrontEndShuttersControl(const QString &name, QObject *parent) :
	AMPseudoMotorControl(name, "", parent)
{
	// Initialize inherited variables.

	value_ = None;
	setpoint_ = None;
	minimumValue_ = Open;
	maximumValue_ = Between;

	setEnumStates(QStringList() << valueToString(Open) << valueToString(Closed) << valueToString(None) << valueToString(Between));
	setMoveEnumStates(QStringList() << valueToString(Open) << valueToString(Closed));

	setContextKnownDescription("ShuttersControl");

	// Initialize class variables.

	photonShutterUpstream_ = 0;
	photonShutterDownstream_ = 0;
	safetyShutter_ = 0;

	// Current settings.

	updateStates();
}

BioXASFrontEndShuttersControl::~BioXASFrontEndShuttersControl()
{

}

bool BioXASFrontEndShuttersControl::canMeasure() const
{
	bool result = false;

	if (isConnected())
		result = (photonShutterUpstream_->canMeasure() && photonShutterDownstream_->canMeasure() && safetyShutter_->canMeasure());

	return result;
}

bool BioXASFrontEndShuttersControl::canMove() const
{
	bool result = false;

	if (isConnected())
		result = (photonShutterUpstream_->canMove() && photonShutterDownstream_->canMove() && safetyShutter_->canMove());

	return result;
}

bool BioXASFrontEndShuttersControl::validValue(double value) const
{
	bool result = false;

	switch (int(value)) {
	case None:
		result = true;
		break;
	case Open:
		result = true;
		break;
	case Between:
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

bool BioXASFrontEndShuttersControl::validSetpoint(double value) const
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

bool BioXASFrontEndShuttersControl::isOpen() const
{
	bool result = false;

	if (isConnected())
		result = ((photonShutterUpstream_->value() == 1) && photonShutterDownstream_->isOpen() && safetyShutter_->isOpen());

	return result;
}

bool BioXASFrontEndShuttersControl::isClosed() const
{
	bool result = false;

	if (isConnected())
		result = ((photonShutterUpstream_->value() == 4) && photonShutterDownstream_->isClosed() && safetyShutter_->isClosed());

	return result;
}

QString BioXASFrontEndShuttersControl::valueToString(BioXASFrontEndShuttersControl::Value value) const
{
	QString result;

	switch (value) {
	case None:
		result = "None";
		break;
	case Open:
		result = "Open";
		break;
	case Between:
		result = "Between";
		break;
	case Closed:
		result = "Closed";
		break;
	default:
		break;
	}

	return result;
}

void BioXASFrontEndShuttersControl::setUpstreamPhotonShutter(AMControl *newControl)
{
	if (photonShutterUpstream_ != newControl) {

		if (photonShutterUpstream_)
			removeChildControl(photonShutterUpstream_);

		photonShutterUpstream_ = newControl;

		if (photonShutterUpstream_)
			addChildControl(photonShutterUpstream_);

		updateStates();

		emit photonShutterUpstreamChanged(photonShutterUpstream_);
	}
}

void BioXASFrontEndShuttersControl::setDownstreamPhotonShutter(CLSBiStateControl *newControl)
{
	if (photonShutterDownstream_ != newControl) {

		if (photonShutterDownstream_)
			removeChildControl(photonShutterDownstream_);

		photonShutterDownstream_ = newControl;

		if (photonShutterDownstream_)
			addChildControl(photonShutterDownstream_);

		updateStates();

		emit photonShutterDownstreamChanged(photonShutterDownstream_);
	}
}

void BioXASFrontEndShuttersControl::setSafetyShutter(CLSBiStateControl *newControl)
{
	if (safetyShutter_ != newControl) {

		if (safetyShutter_)
			removeChildControl(safetyShutter_);

		safetyShutter_ = newControl;

		if (safetyShutter_)
			addChildControl(safetyShutter_);

		updateStates();

		emit safetyShutterChanged(safetyShutter_);
	}
}

void BioXASFrontEndShuttersControl::updateConnected()
{
	bool connected = (
				photonShutterUpstream_ && photonShutterUpstream_->isConnected() &&
				photonShutterDownstream_ && photonShutterDownstream_->isConnected() &&
				safetyShutter_ && safetyShutter_->isConnected()
				);

	setConnected(connected);
}

void BioXASFrontEndShuttersControl::updateValue()
{
	Value newValue = None;

	if (isOpen())
		newValue = Open;
	else if (isClosed())
		newValue = Closed;
	else if (isConnected())
		newValue = Between;

	setValue(newValue);
}

void BioXASFrontEndShuttersControl::updateMoving()
{
	bool moving = false;

	if (isConnected())
		moving = (
					photonShutterUpstream_->isMoving() ||
					photonShutterDownstream_->isMoving() ||
					safetyShutter_->isMoving()
					);

	setIsMoving(moving);
}

AMAction3* BioXASFrontEndShuttersControl::createMoveAction(double setpoint)
{
	AMAction3 *result = 0;

	if (setpoint == Open)
		result = createOpenShuttersAction();
	else if (setpoint == Closed)
		result = createCloseShuttersAction();

	return result;
}

AMAction3* BioXASFrontEndShuttersControl::createOpenShuttersAction()
{
	AMListAction3 *actionList = new AMListAction3(new AMListActionInfo3("Opening front-end shutters", "Opening front-end shutters"), AMListAction3::Sequential);
	actionList->addSubAction(AMActionSupport::buildControlWaitAction(photonShutterUpstream_, 1));
	actionList->addSubAction(AMActionSupport::buildControlMoveAction(safetyShutter_, CLSBiStateControl::Open));
	actionList->addSubAction(AMActionSupport::buildControlMoveAction(photonShutterDownstream_, CLSBiStateControl::Open));

	return actionList;
}

AMAction3* BioXASFrontEndShuttersControl::createCloseShuttersAction()
{
	AMListAction3 *actionList = new AMListAction3(new AMListActionInfo3("Closing front-end shutters", "Closing front-end shutters"), AMListAction3::Sequential);
	actionList->addSubAction(AMActionSupport::buildControlMoveAction(photonShutterDownstream_, CLSBiStateControl::Closed));
	actionList->addSubAction(AMActionSupport::buildControlMoveAction(safetyShutter_, CLSBiStateControl::Closed));

	return actionList;
}
