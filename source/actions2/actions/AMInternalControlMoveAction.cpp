#include "AMInternalControlMoveAction.h"
#include "beamline/AMControl.h"

AMInternalControlMoveAction::AMInternalControlMoveAction(AMControl *control, double setpoint, QObject *parent)
	: AMControlMoveAction(new AMControlMoveActionInfo(
							  AMControlInfo(control ? control->name() : "internalControl",
											setpoint,
											control ? control->minimumValue() : 0,
											control ? control->maximumValue() : 0,
											control ? control->units() : "n/a",
											control ? control->tolerance() : 0,
											control ? control->description() : "[Internal Use Only]")),
						  parent) {
	internalControl_ = control;
}

bool AMInternalControlMoveAction::setSetpoint(double setpoint)
{
	if(state() != AMAction::Constructed)
		return false;

	controlMoveInfo()->setSetpoint(setpoint);
	return true;
}
