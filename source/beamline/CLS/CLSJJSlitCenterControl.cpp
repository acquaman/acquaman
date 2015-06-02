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
		double newCenter = calculateCenterPosition(upperBladeControl_->value(), lowerBladeControl_->value());
		setValue(newCenter);
	}
}

void CLSJJSlitCenterControl::updateMinimumValue()
{
	if (isConnected()) {
		setMinimumValue( calculateMinimumValue(CLSJJSLITBLADESCONTROL_VALUE_MIN, calculateGap( upperBladeControl_->value(), lowerBladeControl_->value() )) );
	}
}

void CLSJJSlitCenterControl::updateMaximumValue()
{
	if (isConnected()) {
		setMaximumValue( calculateMaximumValue(CLSJJSLITBLADESCONTROL_VALUE_MAX, calculateGap( upperBladeControl_->value(), lowerBladeControl_->value() )) );
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

		moveAction = new AMListAction3(new AMListActionInfo3(QString(name()+" move to %1").arg(centerPosition), QString(name()+" move to %1").arg(centerPosition)), AMListAction3::Parallel);
		moveAction->addSubAction(AMActionSupport::buildControlMoveAction(upperBladeControl_, upperPosition));
		moveAction->addSubAction(AMActionSupport::buildControlMoveAction(lowerBladeControl_, lowerPosition));
	}

	return moveAction;
}

double CLSJJSlitCenterControl::calculateMinimumValue(double minimumPosition, double gap)
{
	double result = minimumPosition + fabs(gap/2.0);
	return result;
}

double CLSJJSlitCenterControl::calculateMaximumValue(double maximumPosition, double gap)
{
	double result = maximumPosition - fabs(gap/2.0);
	return result;
}


