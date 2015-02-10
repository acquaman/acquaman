#include "AMChangeToleranceAction.h"

#include "beamline/AMControl.h"
#include "beamline/AMBeamline.h"
#include "util/AMErrorMonitor.h"

AMChangeToleranceAction::AMChangeToleranceAction(AMChangeToleranceActionInfo *info, AMControl *control, QObject *parent)
	: AMAction3(info, parent)
{
	if (control)
	    control_ = control;
	else
	    control_ = AMBeamline::bl()->exposedControlByInfo(info->controlInfo());

	info->setShortDescription(QString("Change Tolerance for Control: %1").arg(control_->name()));
}

AMChangeToleranceAction::AMChangeToleranceAction(const AMChangeToleranceAction &other)
	: AMAction3(other)
{
	const AMChangeToleranceActionInfo *info = qobject_cast<const AMChangeToleranceActionInfo *>(other.info());

	if (info)
	    control_ = AMBeamline::bl()->exposedControlByInfo(info->controlInfo());
	else
	    control_ = 0;
}

AMChangeToleranceAction::~AMChangeToleranceAction()
{

}

AMAction3 *AMChangeToleranceAction::createCopy() const
{
	return new AMChangeToleranceAction(*this);
}

void AMChangeToleranceAction::startImplementation()
{
	const AMControlInfo &controlInfo = changeToleranceControlInfo()->controlInfo();

	// If you still don't have a control, check the exposed controls one last time.
	if (!control_)
	    control_ = AMBeamline::bl()->exposedControlByInfo(controlInfo);

	// Must have a control, and it must be able to move.
	if(!control_) {
	    AMErrorMon::alert(this,
			      AMCHANGETOLERANCEACTION_INVALID_CONTROL,
			      QString("There was an error setting the tolerance for the control '%1' into position, because the control was not found. Please report this problem to the Acquaman developers.").arg(controlInfo.name()));
	    setFailed();
	    return;
	}

	if (controlInfo.tolerance() <= 0.0){

		AMErrorMon::alert(this,
				  AMCHANGETOLERANCEACTION_INVALID_NEW_TOLERANCE,
				  QString("There was an error setting the tolerance for control '%1' because the new tolerance was negative (%2).  Please contact Acquaman developpers.")
				  .arg(controlInfo.name())
				  .arg(controlInfo.tolerance(), 0, 'g' , 3));

		setFailed();
		return;
	}

	setStarted();
	control_->setTolerance(changeToleranceControlInfo()->tolerance());
	setSucceeded();
}
