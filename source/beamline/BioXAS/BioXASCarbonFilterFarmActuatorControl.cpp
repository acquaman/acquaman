#include "BioXASCarbonFilterFarmActuatorControl.h"
#include "util/AMErrorMonitor.h"

BioXASCarbonFilterFarmActuatorControl::BioXASCarbonFilterFarmActuatorControl(QString name, QObject *parent) :
	AMCompositeControl("BioXASCarbonFilterFarmActuatorControl", QString(), parent, "BioXAS Carbon Filter Farm Actuator Control")
{
	// Initialize local variables.

	value_ = Window::Invalid;
	setpoint_ = Window::Invalid;
	moveInProgress_ = false;

	position_ = new AMPVControl("Carbon Filter Farm Actuator Position Control", name+":mm:fbk", name+":mm", name+":stop", this, 0.001);
	connect( position_, SIGNAL(valueChanged(double)), this, SLOT(updateWindow()) );
	addChildControl(position_);

	status_ = new AMReadOnlyPVControl("Carbon Filter Farm Actuator Status Control", name+":inPosition", this);
	addChildControl(status_);

	cancelledMapper_ = new QSignalMapper(this);
	connect( cancelledMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onMoveCancelled(QObject*)) );

	failedMapper_ = new QSignalMapper(this);
	connect( failedMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onMoveFailed(QObject*)) );

	succeededMapper_ = new QSignalMapper(this);
	connect( succeededMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onMoveSucceeded(QObject*)) );

	// Initialize inherited variables.

	setEnumStates(QStringList() << windowToString(Window::Invalid) << windowToString(Window::None) << windowToString(Window::Bottom) << windowToString(Window::Top));
	setMoveEnumStates(QStringList() << windowToString(Window::None) << windowToString(Window::Bottom) << windowToString(Window::Top));
	setAllowsMovesWhileMoving(false);
	setContextKnownDescription("Filter Farm Actuator Control");

	// Emit enumChanged signals when connected, value changes, and setpoint changes. This is to make sure the control is viewed as an enum.

	connect( this, SIGNAL(connected(bool)), this, SIGNAL(enumChanged()) );
	connect( this, SIGNAL(valueChanged(double)), this, SIGNAL(enumChanged()) );
	connect( this, SIGNAL(setpointChanged(double)), this, SIGNAL(enumChanged()) );

	// Current settings.

	updateWindow();
}

BioXASCarbonFilterFarmActuatorControl::~BioXASCarbonFilterFarmActuatorControl()
{

}

bool BioXASCarbonFilterFarmActuatorControl::canStop() const
{
	return isConnected() && position_->canStop();
}

bool BioXASCarbonFilterFarmActuatorControl::validWindow(double value)
{
	bool result = false;

	switch ((int)value) {
	case Window::None:
		result = true;
		break;
	case Window::Bottom:
		result = true;
		break;
	case Window::Top:
		result = true;
		break;
	default:
		break;
	}

	return result;
}

bool BioXASCarbonFilterFarmActuatorControl::validWindowSetpoint(double value)
{
	bool result = false;

	switch ((int)value) {
	case Window::None:
		result = true;
		break;
	case Window::Bottom:
		result = true;
		break;
	case Window::Top:
		result = true;
		break;
	default:
		break;
	}

	return result;
}

QString BioXASCarbonFilterFarmActuatorControl::windowToString(Window::Selection value)
{
	QString result = QString();

	switch (value) {
	case Window::None:
		result = "None";
		break;
	case Window::Bottom:
		result = "Bottom";
		break;
	case Window::Top:
		result = "Top";
		break;
	default:
		result = "Invalid";
		break;
	}

	return result;
}

BioXASCarbonFilterFarmActuatorControl::Window::Selection BioXASCarbonFilterFarmActuatorControl::stringToWindow(const QString &string)
{
	Window::Selection result = Window::Invalid;

	if (string == "None")
		result = Window::None;
	else if (string == "Bottom")
		result = Window::Bottom;
	else if (string == "Top")
		result = Window::Top;

	return result;
}

BioXASCarbonFilterFarmActuatorControl::Window::Selection BioXASCarbonFilterFarmActuatorControl::window(double index)
{
	Window::Selection result = Window::Invalid;

	switch ((int)index) {
	case Window::None:
		result = Window::None;
		break;
	case Window::Bottom:
		result = Window::Bottom;
		break;
	case Window::Top:
		result = Window::Top;
		break;
	default:
		break;
	}

	return result;
}

BioXASCarbonFilterFarmActuatorControl::Window::Selection BioXASCarbonFilterFarmActuatorControl::windowAtPosition(double position) const
{
	return positionMap_.key(position, Window::Invalid);
}

double BioXASCarbonFilterFarmActuatorControl::positionOfWindow(Window::Selection window) const
{
	return positionMap_.value(window, 0);
}

AMControl::FailureExplanation BioXASCarbonFilterFarmActuatorControl::move(double setpoint)
{
	// Check that the control is ready to move and the setpoint is valid.

	if (!isConnected()) {
		AMErrorMon::alert(this, BIOXAS_FILTER_FARM_ACTUATOR_NOT_CONNECTED, "Failed to move carbon filter farm actuator: the arm is not connected.");
		return AMControl::NotConnectedFailure;
	}

	if (isMoving()) {
		AMErrorMon::alert(this, BIOXAS_FILTER_FARM_ACTUATOR_ALREADY_MOVING, "Failed to move carbon filter farm actuator: the arm is already moving.");
		return AMControl::AlreadyMovingFailure;
	}

	if (!validWindowSetpoint(setpoint)) {
		AMErrorMon::alert(this, BIOXAS_FILTER_FARM_ACTUATOR_INVALID_SETPOINT, "Failed to move carbon filter farm actuator: invalid setpoint specified.");
		return AMControl::LimitFailure;
	}

	// Update the setpoint.

	setWindowSetpoint(setpoint);

	// Create an action that moves the actuator to the desired window, and checks that the final position is okay.

	AMAction3 *moveAction = createMoveAction(setpoint_);

	if (!moveAction) {
		AMErrorMon::alert(this, BIOXAS_FILTER_FARM_ACTUATOR_MOVE_FAILED, "Failed to move the carbon filter farm actuator.");
		return AMControl::LimitFailure;
	}

	// Create move action signal mappings.

	connect( moveAction, SIGNAL(started()), this, SLOT(onMoveStarted()) );

	cancelledMapper_->setMapping(moveAction, moveAction);
	connect( moveAction, SIGNAL(cancelled()), cancelledMapper_, SLOT(map()) );

	failedMapper_->setMapping(moveAction, moveAction);
	connect( moveAction, SIGNAL(failed()), failedMapper_, SLOT(map()) );

	succeededMapper_->setMapping(moveAction, moveAction);
	connect( moveAction, SIGNAL(succeeded()), succeededMapper_, SLOT(map()) );

	// Run action.

	moveAction->start();

	return AMControl::NoFailure;
}

void BioXASCarbonFilterFarmActuatorControl::setWindowPosition(Window::Selection window, double position)
{
	positionMap_.insert(window, position);
}

void BioXASCarbonFilterFarmActuatorControl::setWindow(double newWindow)
{
	if (value_ != newWindow) {
		value_ = newWindow;
		emit valueChanged(value_);
	}
}

void BioXASCarbonFilterFarmActuatorControl::setWindowSetpoint(double newWindow)
{
	if (setpoint_ != newWindow) {
		setpoint_ = newWindow;
		emit setpointChanged(setpoint_);
	}
}

void BioXASCarbonFilterFarmActuatorControl::setMoveInProgress(bool isMoving)
{
	if (moveInProgress_ != isMoving) {
		moveInProgress_ = isMoving;
		emit movingChanged(moveInProgress_);

		if (moveInProgress_)
			emit moveStarted();
	}
}

void BioXASCarbonFilterFarmActuatorControl::onMoveStarted()
{
	setMoveInProgress(true);
}

void BioXASCarbonFilterFarmActuatorControl::onMoveCancelled(QObject *action)
{
	moveCleanup(action);
	emit moveFailed(AMControl::WasStoppedFailure);
}

void BioXASCarbonFilterFarmActuatorControl::onMoveFailed(QObject *action)
{
	moveCleanup(action);
	emit moveFailed(AMControl::OtherFailure);
}

void BioXASCarbonFilterFarmActuatorControl::onMoveSucceeded(QObject *action)
{
	moveCleanup(action);
	emit moveSucceeded();
}

void BioXASCarbonFilterFarmActuatorControl::updateWindow()
{
	if (isConnected()) {
		setWindow( windowAtPosition(position_->value()) );

	} else {
		setWindow(Window::Invalid);
	}
}

AMAction3* BioXASCarbonFilterFarmActuatorControl::createMoveAction(double setpoint)
{
	AMListAction3 *action = 0;

	bool setpointOkay = validWindowSetpoint(setpoint);

	if (isConnected() && setpointOkay) {
		Window::Selection windowSetpoint = window(setpoint);
		bool mappingOkay = positionMap_.contains(windowSetpoint);

		if (mappingOkay) {
			action = new AMListAction3(new AMListActionInfo3("Move BioXAS Carbon Filter Farm Actuator", "Move BioXAS Carbon Filter Farm Actuator"), AMListAction3::Sequential);

			AMAction3 *move = AMActionSupport::buildControlMoveAction(position_, positionOfWindow(windowSetpoint));
			action->addSubAction(move);

			AMAction3 *check = AMActionSupport::buildControlWaitAction(status_, Position::Valid, TIMEOUT_MOVE);
			action->addSubAction(check);
		}
	}

	return action;
}

void BioXASCarbonFilterFarmActuatorControl::moveCleanup(QObject *action)
{
	if (action) {
		setMoveInProgress(false);

		action->disconnect();

		cancelledMapper_->removeMappings(action);
		failedMapper_->removeMappings(action);
		succeededMapper_->removeMappings(action);

		action->deleteLater();
	}
}
