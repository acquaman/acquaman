#include "AMSlitCenter.h"
#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"

AMSlitCenter::AMSlitCenter(const QString &name, QObject *parent) :
	AMSlitControl(name, parent)
{

}

AMSlitCenter::~AMSlitCenter()
{

}

void AMSlitCenter::updateValue()
{
	if (canMeasure())
		setValue(currentCenter());
}

//void AMSlitCenter::updateMinimumValue()
//{
//	double newValue = -1;

//	if (canMeasure())
//		newValue = calculateCenter(firstBlade_->minimumValue(), firstBladeOrientation(), secondBlade_->minimumValue(), secondBladeOrientation());

//	setMinimumValue(newValue);
//}

//void AMSlitCenter::updateMaximumValue()
//{
//	double newValue = -1;

//	if (canMeasure())
//		newValue = calculateCenter(firstBlade_->maximumValue(), firstBladeOrientation(), secondBlade_->maximumValue(), secondBladeOrientation());

//	setMaximumValue(newValue);
//}

AMAction3* AMSlitCenter::createMoveAction(double setpoint)
{
	AMListAction3 *action = new AMListAction3(new AMListActionInfo3("Moving slit center.", "Moving slit center."), AMListAction3::Parallel);
	action->addSubAction(AMActionSupport::buildControlMoveAction(firstBlade_, calculateBladeValue(firstBladeOrientation(), currentGap(), setpoint)));
	action->addSubAction(AMActionSupport::buildControlMoveAction(secondBlade_, calculateBladeValue(secondBladeOrientation(), currentGap(), setpoint)));

	return action;
}
