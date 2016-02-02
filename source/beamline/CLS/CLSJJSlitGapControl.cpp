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
	}
}

void CLSJJSlitGapControl::updateMinimumValue()
{
	if (isConnected()) {
		setMinimumValue( calculateMinimumValue(CLSJJSLITBLADESCONTROL_VALUE_MIN, calculateCenterPosition( upperBladeControl_->value(), lowerBladeControl_->value() )) );
	}
}

void CLSJJSlitGapControl::updateMaximumValue()
{
	if (isConnected()) {
		setMaximumValue( calculateMaximumValue(CLSJJSLITBLADESCONTROL_VALUE_MAX, calculateCenterPosition( upperBladeControl_->value(), lowerBladeControl_->value() )) );
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

		moveAction = new AMListAction3(new AMListActionInfo3(QString(name()+" move to %1").arg(gap), QString(name()+" move to %1").arg(gap)), AMListAction3::Parallel);

		moveAction->addSubAction(AMActionSupport::buildControlMoveAction(upperBladeControl_, upperPosition));
		moveAction->addSubAction(AMActionSupport::buildControlMoveAction(lowerBladeControl_, lowerPosition));
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
