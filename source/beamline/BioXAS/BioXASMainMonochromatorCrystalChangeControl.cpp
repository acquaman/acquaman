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

void BioXASMainMonochromatorCrystalChangeControl::setMono(BioXASMainMonochromator *newMono)
{
    if (mono_ != newMono) {
        mono_ = newMono;

        onMonoConnectedChanged();

        emit monoChanged(mono_);
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
            connect( crystalChangeAction, SIGNAL(started()), this, SLOT(onCrystalChangeStarted()) );
            connect( crystalChangeAction, SIGNAL(currentSubActionChanged(int)), this, SLOT(onCrystalChangeSubActionChanged(int)) );
            connect( crystalChangeAction, SIGNAL(succeeded()), this, SLOT(onCrystalChangeActionsSucceeded()) );
            connect( crystalChangeAction, SIGNAL(cancelled()), this, SLOT(onCrystalChangeActionsFailed()) );
            connect( crystalChangeAction, SIGNAL(destroyed()), this, SLOT(onCrystalChangeActionsFailed()) );
            connect( crystalChangeAction, SIGNAL(failed()), this, SLOT(onCrystalChangeActionsFailed()) );

            // Begin crystal change.
            crystalChangeAction->start();
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
        AMAction3 *currentAction = crystalChangeAction->subActionAt(actionIndex);

        if (currentAction)
            emit currentActionChanged(currentAction->info()->shortDescription(), currentAction->info()->longDescription());
    }
}

void BioXASMainMonochromatorCrystalChangeControl::onCrystalChangeActionsSucceeded()
{
    deleteAction(sender());
    setState(CompleteSuccess);
    updateState();
}

void BioXASMainMonochromatorCrystalChangeControl::onCrystalChangeActionsFailed()
{
    deleteAction(sender());
    setState(CompleteFail);
    updateState();
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
            // The mono is valid and connected.
            // The conditions for Initialize are fulfilled.
            setState(Initialized);

        } else if (mono_->isConnected() && (state_ == CompleteSuccess || state_ == CompleteFail)) {
            // We have reached a crystal change terminal state,
            // and the conditions for Initialize are fulfilled.
            setState(Initialized);

        } else if (!mono_->isConnected() && state_ == Initialized) {
            // The mono is valid but not connected.
            // Cannot perform a crystal change.
            setState(None);

        } else if (!mono_->isConnected() && (state_ == CompleteSuccess || state_ == CompleteFail)) {
            // The mono is valid but not connected, and we have reached
            // a crystal change terminal state (not necessarily in that order).
            // Cannot perform a crystal change.
            setState(None);
        }


    } else {
        // If the current mono is not valid, set state to None.
        setState(None);
    }

    // A note: we don't touch the Running state here, because it is set according
    // to feedback from the running actions themselves.
}
