#include "CLSJJSlitGapControl.h"

#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

CLSJJSlitGapControl::CLSJJSlitGapControl(const QString &name, AMControl *upperBladeControl, AMControl *lowerBladeControl, QObject *parent) :
	CLSJJSlitBladesControl(name, upperBladeControl, lowerBladeControl, parent)
{
	// Set inherited variables.

	setContextKnownDescription("GapControl");

	// Make connections.

	connect( this, SIGNAL(centerPositionChanged(double)), this, SLOT(updateMinimumValue()) );
	connect( this, SIGNAL(centerPositionChanged(double)), this, SLOT(updateMaximumValue()) );

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
	}
}

void CLSJJSlitGapControl::updateMinimumValue()
{
	if (isConnected()) {
		setMinimumValue( calculateMinimumValue(CLSJJSLITBLADESCONTROL_VALUE_MIN, centerPosition_) );
	}
}

void CLSJJSlitGapControl::updateMaximumValue()
{
	if (isConnected()) {
		setMaximumValue( calculateMaximumValue(CLSJJSLITBLADESCONTROL_VALUE_MAX, centerPosition_) );
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

double CLSJJSlitGapControl::calculateMinimumValue(double minimumPosition, double centerPosition)
{
	double result = 0;

	if (centerPosition <= 0 ) {
		result = (minimumPosition - centerPosition) * 2;
	} else {
		result = (minimumPosition + centerPosition) * 2;
	}

	return result;
}

double CLSJJSlitGapControl::calculateMaximumValue(double maximumPosition, double centerPosition)
{
	double result = 0;

	if (centerPosition <= 0) {
		result = (maximumPosition + centerPosition) * 2;
	} else {
		result = (maximumPosition - centerPosition) * 2;
	}

	return result;
}
