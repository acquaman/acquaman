/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


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
       control_ = AMBeamline::bl()->exposedControlByInfo(info->controlInfo());

   info->setShortDescription(QString("Wait for Control: %1").arg(control_->name()));
}

AMControlWaitAction::~AMControlWaitAction(){}


AMControlWaitAction::AMControlWaitAction(const AMControlWaitAction &other)
   : AMAction3(other)
{
   const AMControlWaitActionInfo *info = qobject_cast<const AMControlWaitActionInfo*>(other.info());

   if (info)
       control_ = AMBeamline::bl()->exposedControlByInfo(info->controlInfo());
   else
       control_ = 0;
}


#include <QDebug>
void AMControlWaitAction::startImplementation()
{
    const AMControlInfo& setpoint = controlWaitInfo()->controlInfo();

	qDebug() << "Starting wait for" << setpoint.name() << "to" << setpoint.value();

    // If you still don't have a control, check the exposed controls one last time.
    if (!control_)
	control_ = AMBeamline::bl()->exposedControlByInfo(controlWaitInfo()->controlInfo());

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

	timeoutTimer_.setInterval(int(controlWaitInfo()->timeout()*1000));
    timeoutTimer_.start();

    connect( control_, SIGNAL(valueChanged(double)), this, SLOT(onControlValueChanged(double)) );
    connect( &timeoutTimer_, SIGNAL(timeout()), this, SLOT(onTimeoutTimerTimedOut()) );

	qDebug() << "Wait for" << setpoint.name() << "to" << setpoint.value() << "started.";
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
	//Q_UNUSED(newValue)
	qDebug() << control_->name() << "value changed to" << newValue;

    if (checkCurrentControlValue()) {
        cleanup();
		qDebug() << "Wait for" << control_->name() << "to reach" << newValue << "succeeded.";
        setSucceeded();
    }
}



void AMControlWaitAction::onTimeoutTimerTimedOut()
{
	qDebug() << "Wait for" << control_->name() << "timed out.";

	const AMControlInfo& setpoint = controlWaitInfo()->controlInfo();

	AMErrorMon::alert(this,
					  AMCONTROLWAITACTION_TIMEOUT,
					  QString("There was an error waiting for the control '%1' to reach %2 %3. The wait appears to have timed out.")
					  .arg(control_->name())
					  .arg(setpoint.value())
					  .arg(setpoint.units()));

    cleanup();
    setFailed();
}



bool AMControlWaitAction::checkCurrentControlValue()
{
    const AMControlInfo& setpoint = controlWaitInfo()->controlInfo();

//	AMErrorMon::debug(this, AMCONTROLWAITACTION_OUTPUTVALUESDEBUG, QString("%1 Current: %2 New %3")
//					  .arg(control_->name())
//					  .arg(control_->value())
//					  .arg(setpoint.value()));

    if (controlWaitInfo()->matchType() == AMControlWaitActionInfo::MatchEqual) {
        if (control_->value() == setpoint.value())
            return true;

    } else if (controlWaitInfo()->matchType() == AMControlWaitActionInfo::MatchGreaterThan) {
        if (control_->value() > setpoint.value())
            return true;

    } else if (controlWaitInfo()->matchType() == AMControlWaitActionInfo::MatchLessThan) {
        if (control_->value() < setpoint.value())
            return true;
	} else if (controlWaitInfo()->matchType() == AMControlWaitActionInfo::MatchWithinTolerance) {
		if (control_->withinTolerance(setpoint.value()))
			return true;
	}

    return false;
}
