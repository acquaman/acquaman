#include "CLSJJSlitGapControl.h"

#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

CLSJJSlitGapControl::CLSJJSlitGapControl(const QString &name, AMControl *upperBladeControl, AMControl *lowerBladeControl, QObject *parent) :
	CLSJJSlitsBladesControl(name, upperBladeControl, lowerBladeControl, parent, "mm")
{
	// Set inherited variables.

	setContextKnownDescription("GapControl");

	// Current settings.

	updateControlStates();
}

CLSJJSlitGapControl::~CLSJJSlitGapControl()
{

}

void CLSJJSlitGapControl::updateValue()
{
	if (isConnected()) {
		setValue( calculateGap(upperBladeControl_->value(), lowerBladeControl_->value()) );
	}
}

AMAction3* CLSJJSlitGapControl::createMoveAction(double gap)
{
	AMListAction3 *moveAction = 0;

	if (isConnected()) {
		moveAction = new AMListAction3(new AMListActionInfo3("JJSlitsGapControlMove", "JJSlitsGapControlMove"), AMListAction3::Parallel);

		double lowerPosition = calculateLowerPosition(gap, centerPosition_);
		double upperPosition = calculateUpperPosition(gap, centerPosition_);

		moveAction->addSubAction(AMActionSupport::buildControlMoveAction(lowerBladeControl_, lowerPosition));
		moveAction->addSubAction(AMActionSupport::buildControlMoveAction(upperBladeControl_, upperPosition));
	}

	return moveAction;
}
