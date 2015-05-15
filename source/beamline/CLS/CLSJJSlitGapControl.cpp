#include "CLSJJSlitGapControl.h"

#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

CLSJJSlitGapControl::CLSJJSlitGapControl(const QString &name, AMControl *upperBladeControl, AMControl *lowerBladeControl, QObject *parent) :
	CLSJJSlitBladesControl(name, upperBladeControl, lowerBladeControl, parent)
{
	// Set inherited variables.

	setContextKnownDescription("GapControl");

	// Current settings.

	updateStates();
}

CLSJJSlitGapControl::~CLSJJSlitGapControl()
{

}

void CLSJJSlitGapControl::updateValue()
{
	if (isConnected()) {
		setValue( calculateGap(upperBladeControl_->value(), lowerBladeControl_->value()) );

		if (centerPosition_ < 0) {
			setMinimumValue( (CLSJJSLITBLADESCONTROL_VALUE_MIN - centerPosition_) * 2 );
			setMaximumValue( -minimumValue() );
		} else {
			setMaximumValue( (CLSJJSLITBLADESCONROL_VALUE_MAX - centerPosition_) * 2 );
			setMinimumValue( -maximumValue() );
		}
	}
}

AMAction3* CLSJJSlitGapControl::createMoveAction(double gap)
{
	AMListAction3 *moveAction = 0;

	if (isConnected()) {
		updateGap();
		updateCenterPosition();

		double lowerPosition = calculateLowerPosition(gap, centerPosition_);
		double upperPosition = calculateUpperPosition(gap, centerPosition_);

		AMListAction3 *move = new AMListAction3(new AMListActionInfo3("JJSlitsCenterControlMove", "JJSlitsCenterControlMove"), AMListAction3::Parallel);

		move->addSubAction(AMActionSupport::buildControlMoveAction(upperBladeControl_, upperPosition));
		move->addSubAction(AMActionSupport::buildControlMoveAction(lowerBladeControl_, lowerPosition));

		AMListAction3 *confirm = new AMListAction3(new AMListActionInfo3("JJSlitsCenterControlConfirm", "JJSlitsCenterControlConfirm"), AMListAction3::Parallel);

		confirm->addSubAction(AMActionSupport::buildControlWaitAction(upperBladeControl_, upperPosition));
		confirm->addSubAction(AMActionSupport::buildControlWaitAction(lowerBladeControl_, lowerPosition));

		moveAction = new AMListAction3(new AMListActionInfo3("JJSlitsCenterControlMoveAndConfirm", "JJSlitsCenterControlMoveAndConfirm"), AMListAction3::Sequential);

		moveAction->addSubAction(move);
		moveAction->addSubAction(confirm);
	}

	return moveAction;
}
