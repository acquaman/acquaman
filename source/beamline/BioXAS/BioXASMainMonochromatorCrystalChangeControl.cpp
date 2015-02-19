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

QString BioXASMainMonochromatorCrystalChangeControl::stateToString(BioXASMainMonochromatorCrystalChangeControl::State state) const
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

QString BioXASMainMonochromatorCrystalChangeControl::stepToString(BioXASMainMonochromatorCrystalChangeControl::Step step) const
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
        AMListAction3 *crystalChangeAction = qobject_cast<AMListAction3*>(mono_->createCrystalChangeAction());

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
