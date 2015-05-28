#include "CLSJJSlitCenterControl.h"

#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

CLSJJSlitCenterControl::CLSJJSlitCenterControl(const QString &name, AMControl *upperBladeControl, AMControl *lowerBladeControl, QObject *parent) :
	CLSJJSlitBladesControl(name, upperBladeControl, lowerBladeControl, parent)
{
	// Set inherited variables.

	setContextKnownDescription("CenterControl");

	// Make connections.

	connect( this, SIGNAL(gapChanged(double)), this, SLOT(updateMinimumValue()) );
	connect( this, SIGNAL(gapChanged(double)), this, SLOT(updateMaximumValue()) );

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

#include <QDebug>

void CLSJJSlitCenterControl::updateMinimumValue()
{
	if (isConnected()) {
		qDebug() << "\nUpdating" << name() << "minimum value...";
		setMinimumValue( calculateMinimumValue(CLSJJSLITBLADESCONTROL_VALUE_MIN, gap_) );
		qDebug() << "\t" << name() << ":" << minimumValue() << "\n";
	}
}

void CLSJJSlitCenterControl::updateMaximumValue()
{
	if (isConnected()) {
		qDebug() << "\nUpdating" << name() << "maximum value...";
		setMaximumValue( calculateMaximumValue(CLSJJSLITBLADESCONTROL_VALUE_MAX, gap_) );
		qDebug() << "\t" << name() << ":" << maximumValue() << "\n";
	}
}

#include <QDebug>

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

		moveAction = move;
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


