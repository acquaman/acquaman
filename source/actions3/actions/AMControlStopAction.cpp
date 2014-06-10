#include "AMControlStopAction.h"

#include "beamline/AMControl.h"
#include "beamline/AMBeamline.h"
#include "util/AMErrorMonitor.h"

 AMControlStopAction::~AMControlStopAction(){}
AMControlStopAction::AMControlStopAction(AMControlStopActionInfo *info, AMControl *control, QObject *parent)
	: AMAction3(info, parent)
{
	if (control)
		control_ = control;

	else
		control_ = AMBeamline::bl()->exposedControlByInfo(*(info->controlInfo()));
}

AMControlStopAction::AMControlStopAction(const AMControlStopAction &other)
	: AMAction3(other)
{
	const AMControlStopActionInfo *info = qobject_cast<const AMControlStopActionInfo *>(other.info());

	if (info)
		control_ = AMBeamline::bl()->exposedControlByInfo(*(info->controlInfo()));
	else
		control_ = 0;
}

void AMControlStopAction::startImplementation()
{
	AMControlStopActionInfo *controlStopInfo = qobject_cast<AMControlStopActionInfo *>(info());
	const AMControlInfo& setpoint = *(controlStopInfo->controlInfo());

	// If you still don't have a control, check the exposed controls one last time.
	if (!control_)
		control_ = AMBeamline::bl()->exposedControlByInfo(setpoint);

	// Must have a control, and it must be able to move.
	if(!control_) {
		AMErrorMon::alert(this,
						  AMCONTROLSTOPACTION_NO_CONTROL,
						  QString("There was an error moving the control '%1' into position, because the control was not found. Please report this problem to the Acquaman developers.").arg(setpoint.name()));
		setFailed();
		return;
	}

	// check we can stop...
	if(!control_->canStop()) {
		AMErrorMon::alert(this,
						  AMCONTROLSTOPACTION_CANT_STOP,
						  QString("There was an error moving the control '%1' into position, because the control was not connected and ready to move. Please report this problem to the beamline staff.")
						  .arg(control_->name()));
		setFailed();
		return;
	}

	connect(control_, SIGNAL(movingChanged(bool)), this, SLOT(onMovingChanged(bool)));

	bool errorOnStart = control_->stop();

	if (!errorOnStart){

		AMErrorMon::alert(this,
						  AMCONTROLSTOPACTION_IMMEDIATE_FAILURE,
						  QString("There was an immediate error stopping control %1.").arg(control_->name()));
		setFailed();
		return;
	}
}

void AMControlStopAction::onMovingChanged(bool isMoving)
{
	// This is really bad.  The control should be moving already and then stopping... not restarting.
	if (isMoving){

		AMErrorMon::alert(this,
						  AMCONTROLSTOPACTION_COULDNT_STOP_FAILURE,
						  QString("The control %1 was unable to stop.").arg(control_->name()));
		setFailed();
	}

	else
		setSucceeded();
}
