#include "AMSlitGap.h"
#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"
#include "beamline/AMSlit.h"

AMSlitGap::AMSlitGap(const QString &name, QObject *parent) :
	AMSlitControl(name, parent)
{

}

AMSlitGap::~AMSlitGap()
{

}

void AMSlitGap::updateValue()
{
	if (canMeasure())
		setValue(currentGap());
}

AMAction3* AMSlitGap::createMoveAction(double setpoint)
{
	AMListAction3 *action = new AMListAction3(new AMListActionInfo3("Moving slit gap.", "Moving slit gap."), AMListAction3::Parallel);
	action->addSubAction(AMActionSupport::buildControlMoveAction(firstBlade_, calculateBladeValue(firstBladeOrientation(), setpoint, currentCenter())));
	action->addSubAction(AMActionSupport::buildControlMoveAction(secondBlade_, calculateBladeValue(secondBladeOrientation(), setpoint, currentCenter())));

	return action;
}


