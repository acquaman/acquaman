#include "VESPERSToggleControl.h"

#include "util/AMErrorMonitor.h"

VESPERSToggleControl::VESPERSToggleControl(const QString &name, const QString &readPVName, const QString &writePVName, QObject *parent, double tolerance, double completionTimeoutSeconds, const QString &description)
	: AMPVControl(name, readPVName, writePVName, QString(), parent, tolerance, completionTimeoutSeconds, 1, description)
{
	disconnect(writePV_, SIGNAL(valueChanged(double)), this, SLOT(onSetpointChanged(double)));
}

VESPERSToggleControl::~VESPERSToggleControl()
{

}

AMControl::FailureExplanation VESPERSToggleControl::move(double setpoint)
{
	if(!canMove()) {
		AMErrorMon::debug(this, AMPVCONTROL_COULD_NOT_MOVE_BASED_ON_CANMOVE, QString("AMPVControl: Could not move %1 (%2) to %3.").arg(name()).arg(writePV_->pvName()).arg(setpoint_));
		return NotConnectedFailure;
	}

	// new move target:
	setpoint_ = setpoint;
	// Issue the move, check on attemptMoveWhenWithinTolerance
	if(!attemptMoveWhenWithinTolerance_ && inPosition()){
		emit moveSucceeded();
	}
	else{
		// We're now moving! Let's hope this control makes it... (No way to actually check.)
		emit movingChanged(moveInProgress_ = true);
		// emit the signal that we started:
		emit moveStarted();

		// Are we in-position? [With the default tolerance of AMCONTROL_TOLERANCE_DONT_CARE, we will always be in-position, and moves will complete right away, that's the intended behaviour, because we have no other way of knowing when they'll finish.]
		if(inPosition()) {

			emit movingChanged(moveInProgress_ = false);
			emit moveSucceeded();
		}

		else
			writePV_->setValue(1.0);
	}

	return AMControl::NoFailure;
}

bool VESPERSToggleControl::stop()
{
	return false;
}
