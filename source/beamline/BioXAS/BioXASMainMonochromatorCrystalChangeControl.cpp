#include "BioXASMainMonochromatorCrystalChangeControl.h"

BioXASMainMonochromatorCrystalChangeControl::BioXASMainMonochromatorCrystalChangeControl(BioXASMainMonochromator *mono, QObject *parent) :
    QObject(parent)
{
    // Initialize variables.

    mono_ = 0;
    crystalChangeRunning_ = false;

    // Current settings.

    setMono(mono);
}

BioXASMainMonochromatorCrystalChangeControl::~BioXASMainMonochromatorCrystalChangeControl()
{

}

void BioXASMainMonochromatorCrystalChangeControl::setMono(BioXASMainMonochromator *newMono)
{
    if (mono_ != newMono)
        emit monoChanged(mono_ = newMono);
}

void BioXASMainMonochromatorCrystalChangeControl::startCrystalChange()
{
    if (mono_ && !crystalChangeRunning_) {
        AMListAction3 *crystalChangeAction = qobject_cast<AMListAction3*>(mono_->createCrystalChangeAction());

        if (crystalChangeAction) {

            // connect to action signals.
            connect( crystalChangeAction, SIGNAL(started()), this, SIGNAL(crystalChangeStarted()) );
            connect( crystalChangeAction, SIGNAL(currentSubActionChanged(int)), this, SLOT(onCrystalChangeSubActionChanged(int)) );
            connect( crystalChangeAction, SIGNAL(succeeded()), this, SLOT(onCrystalChangeActionsSucceeded()) );
            connect( crystalChangeAction, SIGNAL(cancelled()), this, SLOT(onCrystalChangeActionsFailed()) );
            connect( crystalChangeAction, SIGNAL(destroyed()), this, SLOT(onCrystalChangeActionsFailed()) );
            connect( crystalChangeAction, SIGNAL(failed()), this, SLOT(onCrystalChangeActionsFailed()) );

            // update crystal change action running status.
            setCrystalChangeRunning(true);

            // begin crystal change.
            crystalChangeAction->start();
        }
    }
}

void BioXASMainMonochromatorCrystalChangeControl::setCrystalChangeRunning(bool isRunning)
{
    if (crystalChangeRunning_ != isRunning)
        emit crystalChangeRunningChanged(crystalChangeRunning_ = isRunning);
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
    setCrystalChangeRunning(false);
    emit crystalChangeEnded(true);
}

void BioXASMainMonochromatorCrystalChangeControl::onCrystalChangeActionsFailed()
{
    deleteAction(sender());
    setCrystalChangeRunning(false);
    emit crystalChangeEnded(false);
}

void BioXASMainMonochromatorCrystalChangeControl::deleteAction(QObject *crystalChangeAction)
{
    disconnect( crystalChangeAction, 0, this, 0 );
    crystalChangeAction->deleteLater();
}
