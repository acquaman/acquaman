#include "BioXASMainMonochromatorCrystalChangeControl.h"
#include <QDebug>

BioXASMainMonochromatorCrystalChangeControl::BioXASMainMonochromatorCrystalChangeControl(BioXASMainMonochromator *mono, QObject *parent) :
    QObject(parent)
{
    // Initialize variables.

    mono_ = 0;
    state_ = NotInitialized;
    step_ = None;

    // Current settings.

    setMono(mono);
}

BioXASMainMonochromatorCrystalChangeControl::~BioXASMainMonochromatorCrystalChangeControl()
{

}

QString BioXASMainMonochromatorCrystalChangeControl::stateToString(BioXASMainMonochromatorCrystalChangeControl::State state)
{
    QString result = "";

    switch (state) {
    case NotInitialized:
        result = "NotInitialized";
        break;
    case Initialized:
        result = "Initialized";
        break;
    case Running:
        result = "Running";
        break;
    case CompleteSuccess:
        result = "CompleteSuccess";
        break;
    case CompleteFail:
        result = "CompleteFail";
        break;
    default:
        result = "Unknown";
    }

    return result;
}

QString BioXASMainMonochromatorCrystalChangeControl::stepToString(BioXASMainMonochromatorCrystalChangeControl::Step step)
{
    QString result = "";

    switch (step) {
    case None:
        result = "None";
        break;
    case SlitsClosed:
        result = "SlitsClosed";
        break;
    case PaddleOut:
        result = "PaddleOut";
        break;
    case KeyEnabled:
        result = "KeyEnabled";
        break;
    case BraggNotAtCrystalChangePosition:
        result = "BraggNotAtCrystalChangePosition";
        break;
    case BraggAtCrystalChangePosition:
        result = "BraggAtCrystalChangePosition";
        break;
    case BrakeDisabled:
        result = "BrakeDisabled";
        break;
    case CrystalChangeMotorNotAtLimit:
        result = "CrystalChangeMotorNotAtLimit";
        break;
    case CrystalChangeMotorAtLimit:
        result = "CrystalChangeMotorAtLimit";
        break;
    case BrakeEnabled:
        result = "BrakeEnabled";
        break;
    case BraggNotAtNewRegion:
        result = "BraggNotAtNewRegion";
        break;
    case BraggAtNewRegion:
        result = "BraggAtNewRegion";
        break;
    case KeyDisabled:
        result = "KeyDisabled";
        break;
    default:
        result = "Unknown";
    }

    return result;
}

void BioXASMainMonochromatorCrystalChangeControl::setMono(BioXASMainMonochromator *newMono)
{
    if (mono_ != newMono) {

        if (mono_)
            disconnect( mono_, 0, this, 0 );

        mono_ = newMono;

        if (mono_) {
            connect( mono_, SIGNAL(connected(bool)), this, SLOT(onMonoConnectedChanged()) );
        }

        emit monoChanged(mono_);

        onMonoConnectedChanged();
    }
}

void BioXASMainMonochromatorCrystalChangeControl::startCrystalChange()
{
    // The crystal change actions can only be executed once we are in the Initialized state.
    // This pretty much just means we have a valid, connected mono to start with.

    if (state_ == Initialized) {
		AMListAction3 *crystalChangeAction = qobject_cast<AMListAction3*>(createCrystalChangeAction());

        if (crystalChangeAction) {

            // Connect to action signals.
            connect( crystalChangeAction, SIGNAL(started()), this, SLOT(onCrystalChangeActionsStarted()) );
            connect( crystalChangeAction, SIGNAL(currentSubActionChanged(int)), this, SLOT(onCrystalChangeSubActionChanged(int)) );
            connect( crystalChangeAction, SIGNAL(succeeded()), this, SLOT(onCrystalChangeActionsSucceeded()) );
            connect( crystalChangeAction, SIGNAL(cancelled()), this, SLOT(onCrystalChangeActionsFailed()) );
            connect( crystalChangeAction, SIGNAL(destroyed()), this, SLOT(onCrystalChangeActionsFailed()) );
            connect( crystalChangeAction, SIGNAL(failed()), this, SLOT(onCrystalChangeActionsFailed()) );

            // Begin crystal change.
            crystalChangeAction->start();

        } else {
            qDebug() << "Crystal change actions provided not valid.";
        }

    } else {
        qDebug() << "Control not initialized. Cannot start crystal change.";
    }
}

void BioXASMainMonochromatorCrystalChangeControl::reset()
{
    setState(NotInitialized);
    updateState();
}

void BioXASMainMonochromatorCrystalChangeControl::setState(State newState)
{
    if (state_ != newState) {
        state_ = newState;
        emit stateChanged(state_);
    }
}

void BioXASMainMonochromatorCrystalChangeControl::onMonoConnectedChanged()
{
    updateState();
}

void BioXASMainMonochromatorCrystalChangeControl::onCrystalChangeActionsStarted()
{
    setState(Running);
}

void BioXASMainMonochromatorCrystalChangeControl::onCrystalChangeSubActionChanged(int actionIndex)
{
    AMListAction3 *crystalChangeAction = qobject_cast<AMListAction3*>(sender());

    if (crystalChangeAction) {
        QPair<double, double> progress = crystalChangeAction->progress();
        AMAction3 *currentAction = crystalChangeAction->subActionAt(actionIndex);

        if (currentAction) {
            emit currentActionChanged(currentAction->info()->shortDescription(), currentAction->info()->longDescription());
            emit progressChanged(progress.first, progress.second);
        }
    }
}

void BioXASMainMonochromatorCrystalChangeControl::onCrystalChangeActionsSucceeded()
{
    deleteAction(sender());
    setState(CompleteSuccess);
}

void BioXASMainMonochromatorCrystalChangeControl::onCrystalChangeActionsFailed()
{
    deleteAction(sender());
    setState(CompleteFail);
}

AMAction3* BioXASMainMonochromatorCrystalChangeControl::createWaitForSlitsClosedAction()
{
	AMAction3 *action = 0;

	if (mono_ && mono_->slitsClosedControl()->isConnected()) {
		AMControlInfo setpoint = mono_->slitsClosedControl()->toInfo();
		setpoint.setValue(1);

		action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_SLITS_CLOSED, AMControlWaitActionInfo::MatchEqual), mono_->slitsClosedControl());
		action->info()->setShortDescription("Waiting for slits closed...");
		action->info()->setLongDescription("");
	}

	return action;
}

AMAction3* BioXASMainMonochromatorCrystalChangeControl::createWaitForPaddleRemovedAction()
{
	AMAction3 *action = 0;

	if (mono_ && mono_->phosphorPaddleOutControl() && mono_->phosphorPaddleOutControl()->isConnected()) {
		AMControlInfo setpoint = mono_->phosphorPaddleOutControl()->toInfo();
		setpoint.setValue(1);

		action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_PADDLE_OUT, AMControlWaitActionInfo::MatchEqual), mono_->phosphorPaddleOutControl());
		action->info()->setShortDescription("Waiting for paddle removed...");
		action->info()->setLongDescription("");
	}

	return action;
}

AMAction3* BioXASMainMonochromatorCrystalChangeControl::createWaitForKeyEnabledAction()
{
	AMAction3 *action = 0;

	if (mono_ && mono_->keyStatusControl() && mono_->keyStatusControl()->isConnected()) {
		AMControlInfo setpoint = mono_->keyStatusControl()->toInfo();
		setpoint.setValue(1);

		action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_KEY_STATUS_CHANGE, AMControlWaitActionInfo::MatchEqual), mono_->keyStatusControl());
		action->info()->setShortDescription("Enable crystal change.");
		action->info()->setLongDescription("Turn the crystal change status key to 'Enabled', counter-clockwise.");
	}

	return action;
}

AMAction3* BioXASMainMonochromatorCrystalChangeControl::createMoveToCrystalChangePositionAction()
{
	AMAction3 *action = 0;

	if (mono_) {
		action = mono_->createMoveBraggMotorAction(SETPOINT_BRAGG_MOTOR_CRYSTAL_CHANGE_POSITION);

		if (action) {
			action->info()->setShortDescription("Moving to crystal change position...");
			action->info()->setLongDescription("");
		}
	}

	return action;
}

AMAction3* BioXASMainMonochromatorCrystalChangeControl::createWaitForAtCrystalChangePositionAction()
{
	AMAction3 *action = 0;

	if (mono_ && mono_->braggMotorAtCrystalChangePositionStatusControl() && mono_->braggMotorAtCrystalChangePositionStatusControl()->isConnected()) {
		AMControlInfo setpoint = mono_->braggMotorAtCrystalChangePositionStatusControl()->toInfo();
		setpoint.setValue(1);

		action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_CRYSTAL_CHANGE_POSITION_REACHED, AMControlWaitActionInfo::MatchEqual), mono_->braggMotorAtCrystalChangePositionStatusControl());
		action->info()->setShortDescription("Waiting for move to crystal change position...");
		action->info()->setLongDescription("");
	}

	return action;
}

AMAction3* BioXASMainMonochromatorCrystalChangeControl::createWaitForBrakeDisabledAction()
{
	AMAction3 *action = 0;

	if (mono_ && mono_->brakeStatusControl() && mono_->brakeStatusControl()->isConnected()) {
		AMControlInfo setpoint = mono_->brakeStatusControl()->toInfo();
		setpoint.setValue(1);

		action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_BRAKE_STATUS_CHANGE, AMControlWaitActionInfo::MatchEqual), mono_->brakeStatusControl());
		action->info()->setShortDescription("Disable brake.");
		action->info()->setLongDescription("Flip the brake switch to 'Disabled'. There is a mandatory 30s wait after the switch is flipped.");
	}

	return action;
}

AMAction3* BioXASMainMonochromatorCrystalChangeControl::createWaitForCrystalChangeMotorLimitReached(bool cwLimit)
{
	AMAction3 *action = 0;

	if (mono_) {
		if (cwLimit && mono_->crystalChangeMotorCWLimitStatusControl() && mono_->crystalChangeMotorCCWLimitStatusControl()->isConnected()) {
			AMControlInfo setpoint = mono_->crystalChangeMotorCWLimitStatusControl()->toInfo();
			setpoint.setValue(1);

			action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_CRYSTAL_CHANGE_MOTOR_LIMIT_REACHED, AMControlWaitActionInfo::MatchEqual), mono_->crystalChangeMotorCWLimitStatusControl());
			action->info()->setShortDescription("Waiting for move to crystal change motor CW limit...");
			action->info()->setLongDescription("");

		} else if (!cwLimit && mono_->crystalChangeMotorCCWLimitStatusControl() && mono_->crystalChangeMotorCCWLimitStatusControl()->isConnected()){
			AMControlInfo setpoint = mono_->crystalChangeMotorCCWLimitStatusControl()->toInfo();
			setpoint.setValue(1);

			action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_CRYSTAL_CHANGE_MOTOR_LIMIT_REACHED, AMControlWaitActionInfo::MatchEqual), mono_->crystalChangeMotorCCWLimitStatusControl());
			action->info()->setShortDescription("Waiting for move to crystal change motor CCW limit...");
			action->info()->setLongDescription("");
		}
	}

	return action;
}

AMAction3* BioXASMainMonochromatorCrystalChangeControl::createWaitForBrakeEnabledAction()
{
	AMAction3 *action = 0;

	if (mono_ && mono_->brakeStatusControl() && mono_->brakeStatusControl()->isConnected()) {
		AMControlInfo setpoint = mono_->brakeStatusControl()->toInfo();
		setpoint.setValue(0);

		action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_BRAKE_STATUS_CHANGE, AMControlWaitActionInfo::MatchEqual), mono_->brakeStatusControl());
		action->info()->setShortDescription("Enable brake.");
		action->info()->setLongDescription("Flip the brake switch to 'Enabled'. There is a mandatory 30s wait after the switch is flipped.");
	}

	return action;
}

AMAction3* BioXASMainMonochromatorCrystalChangeControl::createWaitForBraggMotorLimitReachedAction(bool cwLimit)
{
	AMAction3 *action = 0;

	if (mono_) {
		if (cwLimit && mono_->braggMotorAtCWLimitStatusControl() && mono_->braggMotorAtCWLimitStatusControl()->isConnected()) {
			AMControlInfo setpoint = mono_->braggMotorAtCWLimitStatusControl()->toInfo();
			setpoint.setValue(0);

			action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_BRAGG_MOTOR_LIMIT_REACHED, AMControlWaitActionInfo::MatchEqual), mono_->braggMotorAtCWLimitStatusControl());
			action->info()->setShortDescription("Waiting for move to bragg motor CW limit...");
			action->info()->setLongDescription("");

		} else if (!cwLimit && mono_->braggMotorAtCCWLimitStatusControl() && mono_->braggMotorAtCCWLimitStatusControl()->isConnected()) {
			AMControlInfo setpoint = mono_->braggMotorAtCCWLimitStatusControl()->toInfo();
			setpoint.setValue(0);

			action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_BRAGG_MOTOR_LIMIT_REACHED, AMControlWaitActionInfo::MatchEqual), mono_->braggMotorAtCCWLimitStatusControl());
			action->info()->setShortDescription("Waiting for move to bragg motor CCW limit...");
			action->info()->setLongDescription("");
		}
	}

	return action;
}

AMAction3* BioXASMainMonochromatorCrystalChangeControl::createWaitForMoveToNewRegion(BioXASMainMonochromator::Region destinationRegion)
{
	AMAction3 *action = 0;

	if (mono_) {
		if (destinationRegion == BioXASMainMonochromator::A && mono_->regionAStatusControl() && mono_->regionAStatusControl()->isConnected()) {
			AMControlInfo setpoint = mono_->regionAStatusControl()->toInfo();
			setpoint.setValue(1);

			action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_REGION_STATE_CHANGED, AMControlWaitActionInfo::MatchEqual), mono_->regionAStatusControl());
			action->info()->setShortDescription("Waiting for move into region A...");
			action->info()->setLongDescription("");

		} else if (destinationRegion == BioXASMainMonochromator::B && mono_->regionBStatusControl() && mono_->regionBStatusControl()->isConnected()) {
			AMControlInfo setpoint = mono_->regionBStatusControl()->toInfo();
			setpoint.setValue(1);

			action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_REGION_STATE_CHANGED, AMControlWaitActionInfo::MatchEqual), mono_->regionBStatusControl());
			action->info()->setShortDescription("Waiting for move into region B...");
			action->info()->setLongDescription("");
		}
	}

	return action;
}

AMAction3* BioXASMainMonochromatorCrystalChangeControl::createWaitForKeyDisabledAction()
{
	AMAction3 *action = 0;

	if (mono_) {
		AMControlInfo setpoint = mono_->keyStatusControl()->toInfo();
		setpoint.setValue(0);

		action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_KEY_STATUS_CHANGE, AMControlWaitActionInfo::MatchEqual), mono_->keyStatusControl());
		action->info()->setShortDescription("Disable crystal change.");
		action->info()->setLongDescription("Turn crystal change status key to 'Disabled', clockwise.");
	}

	return action;
}

AMAction3* BioXASMainMonochromatorCrystalChangeControl::createCrystalChangeAction(BioXASMainMonochromator::Region newRegion)
{
	AMListAction3 *action = 0;

	if (mono_ && mono_->isConnected() && mono_->region() != newRegion && newRegion != BioXASMainMonochromator::None) {

		double crystalChangeMotorDestination;
		double braggMotorDestination;

		if (newRegion == BioXASMainMonochromator::A) {
			crystalChangeMotorDestination = SETPOINT_CRYSTAL_CHANGE_MOTOR_REGION_A_DESTINATION;
			braggMotorDestination = SETPOINT_BRAGG_MOTOR_REGION_A_DESTINATION;

		} else {
			crystalChangeMotorDestination = SETPOINT_CRYSTAL_CHANGE_MOTOR_REGION_B_DESTINATION;
			braggMotorDestination = SETPOINT_BRAGG_MOTOR_REGION_B_DESTINATION;
		}

		action = new AMListAction3(new AMListActionInfo3("Crystal Change Action", "Crystal Change Action"));
		action->addSubAction(mono_->createCloseSlitsAction());
		action->addSubAction(createWaitForSlitsClosedAction());
		action->addSubAction(mono_->createRemovePaddleAction());
		action->addSubAction(createWaitForPaddleRemovedAction());
		action->addSubAction(createWaitForKeyEnabledAction());
		action->addSubAction(createMoveToCrystalChangePositionAction());
		action->addSubAction(createWaitForAtCrystalChangePositionAction());
		action->addSubAction(createWaitForBrakeDisabledAction());
		action->addSubAction(mono_->createMoveCrystalChangeMotorAction(crystalChangeMotorDestination));
		action->addSubAction(createWaitForCrystalChangeMotorLimitReached(newRegion == BioXASMainMonochromator::A));
		action->addSubAction(createWaitForBrakeEnabledAction());
		action->addSubAction(mono_->createMoveBraggMotorAction(braggMotorDestination));
		action->addSubAction(createWaitForMoveToNewRegion(newRegion));
		action->addSubAction(createWaitForKeyDisabledAction());

	}

	return action;
}

AMAction3* BioXASMainMonochromatorCrystalChangeControl::createCrystalChangeAction()
{
	AMAction3 *action = 0;

	if (mono_) {
		if (mono_->region() == BioXASMainMonochromator::A)
			action = createCrystalChangeAction(BioXASMainMonochromator::B);

		else if (mono_->region() == BioXASMainMonochromator::B)
			action = createCrystalChangeAction(BioXASMainMonochromator::A);

		else
			action = createCrystalChangeAction(BioXASMainMonochromator::A);
	}

	return action;
}

void BioXASMainMonochromatorCrystalChangeControl::deleteAction(QObject *crystalChangeAction)
{
    disconnect( crystalChangeAction, 0, this, 0 );
    crystalChangeAction->deleteLater();
}

void BioXASMainMonochromatorCrystalChangeControl::updateState()
{
    if (mono_) {

        if (mono_->isConnected() && state_ == NotInitialized) {
            // The mono is valid and connected. No actions running.
            // The conditions for Initialize are fulfilled.
            setState(Initialized);

        } else if (mono_->isConnected() && (state_ == CompleteSuccess || state_ == CompleteFail)) {
            // We have reached a crystal change terminal state, no actions running,
            // and the conditions for Initialize are fulfilled.
//            setState(Initialized);

        } else if (!mono_->isConnected() && state_ == Initialized) {
            // The mono is valid but not connected.
            // Cannot perform a crystal change.
            setState(NotInitialized);

        } else if (!mono_->isConnected() && (state_ == CompleteSuccess || state_ == CompleteFail)) {
            // The mono is valid but not connected, and we have reached
            // a crystal change terminal state (not necessarily in that order).
            // In any case, cannot perform a crystal change.
            setState(NotInitialized);
        }


    } else {
        // If the current mono is not valid, set state to None.
        setState(NotInitialized);
    }

    // A note: we don't touch the Running state here, because it is set according
    // to feedback from the running actions themselves. Response to the running
    // actions also handles setting the CompleteSuccess and CompleteFail states.
}
