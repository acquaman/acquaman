#include "BioXASMainMonochromatorCrystalChangeControl.h"
#include <QDebug>

BioXASMainMonochromatorCrystalChangeControl::BioXASMainMonochromatorCrystalChangeControl(BioXASMainMonochromator *mono, QObject *parent) :
    QObject(parent)
{
    // Initialize variables.

    mono_ = 0;
    state_ = None;

    // Current settings.

    setMono(mono);
}

BioXASMainMonochromatorCrystalChangeControl::~BioXASMainMonochromatorCrystalChangeControl()
{

}

QString BioXASMainMonochromatorCrystalChangeControl::stateToString(BioXASMainMonochromatorCrystalChangeControl::State state) const
{
    switch (state) {
    case None:
        return "None";
    case Initialized:
        return "Initialized";
    case Running:
        return "Running";
    case CompleteSuccess:
        return "CompleteSuccess";
    case CompleteFail:
        return "CompleteFail";
    default:
        return "Unknown";
    }
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

void BioXASMainMonochromatorCrystalChangeControl::setState(State newState)
{
    if (state_ != newState) {
        state_ = newState;
        emit stateChanged(state_);
    }

    // There are certain states we don't want to dwell in (ie. CompleteSuccess, CompleteFail),
    // and calling updateState() after a state change should make sure we always end up in the
    // appropriate state, no matter what newState is.

    updateState();
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

        if (mono_->isConnected() && state_ == None) {
            // The mono is valid and connected. No actions running.
            // The conditions for Initialize are fulfilled.
            setState(Initialized);

        } else if (mono_->isConnected() && (state_ == CompleteSuccess || state_ == CompleteFail)) {
            // We have reached a crystal change terminal state, no actions running,
            // and the conditions for Initialize are fulfilled.
            setState(Initialized);

        } else if (!mono_->isConnected() && state_ == Initialized) {
            // The mono is valid but not connected.
            // Cannot perform a crystal change.
            setState(None);

        } else if (!mono_->isConnected() && (state_ == CompleteSuccess || state_ == CompleteFail)) {
            // The mono is valid but not connected, and we have reached
            // a crystal change terminal state (not necessarily in that order).
            // In any case, cannot perform a crystal change.
            setState(None);
        }


    } else {
        // If the current mono is not valid, set state to None.
        setState(None);
    }

    // A note: we don't touch the Running state here, because it is set according
    // to feedback from the running actions themselves.
}
