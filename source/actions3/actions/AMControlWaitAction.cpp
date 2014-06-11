#include "AMControlWaitAction.h"

#include "beamline/AMControl.h"
#include "beamline/AMBeamline.h"
#include "util/AMErrorMonitor.h"

AMControlWaitAction::AMControlWaitAction(AMControlWaitActionInfo *info, AMControl *control, QObject *parent)
   : AMAction3(info, parent)
{
   if (control)
       control_ = control;
   else
       control_ = AMBeamline::bl()->exposedControlByInfo(*(info->controlInfo()));
}

AMControlWaitAction::~AMControlWaitAction(){}


AMControlWaitAction::AMControlWaitAction(const AMControlWaitAction &other)
   : AMAction3(other)
{
   const AMControlWaitActionInfo *info = qobject_cast<const AMControlWaitActionInfo*>(other.info());

   if (info)
       control_ = AMBeamline::bl()->exposedControlByInfo(*(info->controlInfo()));
   else
       control_ = 0;
}


void AMControlWaitAction::startImplementation()
{
    const AMControlInfo& setpoint = *(controlWaitInfo()->controlInfo());

    // If you still don't have a control, check the exposed controls one last time.
    if (!control_)
        control_ = AMBeamline::bl()->exposedControlByInfo(*(controlWaitInfo()->controlInfo()));

    // Must have a control, and it must be able to move.
    if(!control_) {
        AMErrorMon::alert(this,
                          AMCONTROLWAITACTION_INVALIDCONTROL,
                          QString("There was an error waiting for the control '%1' into position, because the control was not found. Please report this problem to the Acquaman developers.").arg(setpoint.name()));
        setFailed();
        return;
    }

    // check that the destination is in range...
    if( (controlWaitInfo()->matchType() == AMControlWaitActionInfo::MatchEqual && control_->valueOutOfRange(setpoint.value()))
            || (controlWaitInfo()->matchType() == AMControlWaitActionInfo::MatchGreaterThan && control_->maximumValue() < setpoint.value())
            || (controlWaitInfo()->matchType() == AMControlWaitActionInfo::MatchLessThan && control_->minimumValue() > setpoint.value()) ) {
        AMErrorMon::alert(this,
                          AMCONTROLWAITACTION_WAITVALUEOUTOFRANGE,
                          QString("There was an error waiting for the control '%1' into position, because the destination %2 %3 was outside its range with MatchType %4. Please report this problem to the beamline staff.")
                          .arg(control_->name())
                          .arg(setpoint.value())
                          .arg(setpoint.units())
                          .arg(controlWaitInfo()->matchType()));
        setFailed();
        return;
    }

    if (checkCurrentControlValue()) {
        setStarted();
        setSucceeded();
        return;
    }

    timeoutTimer_.setInterval(controlWaitInfo()->timeout()*1000);
    timeoutTimer_.start();

    connect( control_, SIGNAL(valueChanged(double)), this, SLOT(onControlValueChanged(double)) );
    connect( &timeoutTimer_, SIGNAL(timeout()), this, SLOT(onTimeoutTimerTimedOut()) );

    setStarted();
}



void AMControlWaitAction::cleanup()
{
    timeoutTimer_.stop();
    disconnect( control_, SIGNAL(valueChanged(double)), this, SLOT(onControlValueChanged(double)) );
    disconnect( &timeoutTimer_, SIGNAL(timeout()), this, SLOT(onTimeoutTimerTimedOut()) );
}



void AMControlWaitAction::cancelImplementation()
{
    cleanup();
    setCancelled();
}



void AMControlWaitAction::onControlValueChanged(double newValue)
{
	Q_UNUSED(newValue)
    if (checkCurrentControlValue()) {
        cleanup();
        setSucceeded();
    }
}



void AMControlWaitAction::onTimeoutTimerTimedOut()
{
    qDebug() << "We timed out and failed";
    cleanup();
    setFailed();
}



bool AMControlWaitAction::checkCurrentControlValue()
{
    const AMControlInfo& setpoint = *(controlWaitInfo()->controlInfo());

    if (controlWaitInfo()->matchType() == AMControlWaitActionInfo::MatchEqual) {
        if (control_->value() == setpoint.value())
            return true;

    } else if (controlWaitInfo()->matchType() == AMControlWaitActionInfo::MatchGreaterThan) {
        if (control_->value() > setpoint.value())
            return true;

    } else if (controlWaitInfo()->matchType() == AMControlWaitActionInfo::MatchLessThan) {
        if (control_->value() < setpoint.value())
            return true;
    }

    return false;
}
