#include "CLSJJSlitCenterControl.h"

#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

CLSJJSlitCenterControl::CLSJJSlitCenterControl(const QString &name, AMControl *upperBladeControl, AMControl *lowerBladeControl, QObject *parent) :
	CLSJJSlitBladesControl(name, upperBladeControl, lowerBladeControl, parent, "mm")
{
	// Set inherited variables.

	setContextKnownDescription("CenterControl");

	// Current settings.

	updateControlStates();
}

CLSJJSlitCenterControl::~CLSJJSlitCenterControl()
{

}

void CLSJJSlitCenterControl::updateValue()
{
	if (isConnected()) {
		setValue( calculateCenterPosition(upperBladeControl_->value(), lowerBladeControl_->value()) );
	}
}

AMAction3* CLSJJSlitCenterControl::createMoveAction(double centerPosition)
{
	AMListAction3 *moveAction = 0;

	if (isConnected()) {
		moveAction = new AMListAction3(new AMListActionInfo3("JJSlitsCenterControlMove", "JJSlitsCenterControlMove"), AMListAction3::Parallel);

		double lowerPosition = calculateLowerPosition(gap_, centerPosition);
		double upperPosition = calculateUpperPosition(gap_, centerPosition);

		moveAction->addSubAction(AMActionSupport::buildControlMoveAction(upperBladeControl_, upperPosition));
		moveAction->addSubAction(AMActionSupport::buildControlMoveAction(lowerBladeControl_, lowerPosition));
	}

	return moveAction;
}
