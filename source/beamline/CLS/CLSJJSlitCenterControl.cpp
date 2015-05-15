#include "CLSJJSlitCenterControl.h"

#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

CLSJJSlitCenterControl::CLSJJSlitCenterControl(const QString &name, AMControl *upperBladeControl, AMControl *lowerBladeControl, QObject *parent) :
	CLSJJSlitBladesControl(name, upperBladeControl, lowerBladeControl, parent)
{
	// Set inherited variables.

	setContextKnownDescription("CenterControl");

	// Current settings.

	updateStates();
}

CLSJJSlitCenterControl::~CLSJJSlitCenterControl()
{

}

void CLSJJSlitCenterControl::updateValue()
{
	if (isConnected()) {
		setValue( calculateCenterPosition(upperBladeControl_->value(), lowerBladeControl_->value()) );

		if (gap_ < 0) {
			setMinimumValue(CLSJJSLITBLADESCONTROL_VALUE_MIN - abs(gap_/2.0));
			setMaximumValue( -minimumValue() );
		} else {
			setMaximumValue(CLSJJSLITBLADESCONTROL_VALUE_MAX - abs(gap_/2.0));
			setMinimumValue( -maximumValue() );
		}
	}
}

AMAction3* CLSJJSlitCenterControl::createMoveAction(double centerPosition)
{
	AMListAction3 *moveAction = 0;

	if (isConnected()) {
		updateGap();
		updateCenterPosition();

		double lowerPosition = calculateLowerPosition(gap_, centerPosition);
		double upperPosition = calculateUpperPosition(gap_, centerPosition);

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
