#include "CLSJJSlitBladesControl.h"

#include "util/AMErrorMonitor.h"

CLSJJSlitBladesControl::CLSJJSlitBladesControl(const QString &name, AMControl *upperBladeControl, AMControl *lowerBladeControl, QObject *parent) :
	AMPseudoMotorControl(name, "mm", parent)
{
	// Initialize inherited variables.

	value_ = 0.0;
	setpoint_ = 0.0;
	minimumValue_ = CLSJJSLITBLADESCONTROL_VALUE_MIN;
	maximumValue_ = CLSJJSLITBLADESCONTROL_VALUE_MAX;

	setAllowsMovesWhileMoving(false);
	setTolerance(0.005);
	setDisplayPrecision(2);

	// Initialize local variables.

	gap_ = 0.0;
	centerPosition_ = 0.0;

	upperBladeControl_ = 0;
	lowerBladeControl_ = 0;

	// Make connections.

	connect( this, SIGNAL(valueChanged(double)), this, SLOT(updateMinimumValue()) );
	connect( this, SIGNAL(valueChanged(double)), this, SLOT(updateMaximumValue()) );

	// Current settings.

	setUpperBladeControl(upperBladeControl);
	setLowerBladeControl(lowerBladeControl);
}

CLSJJSlitBladesControl::~CLSJJSlitBladesControl()
{

}

bool CLSJJSlitBladesControl::canMeasure() const
{
	bool result = false;

	if (isConnected())
		result = (upperBladeControl_->canMeasure() && lowerBladeControl_->canMeasure());

	return result;
}

bool CLSJJSlitBladesControl::canMove() const
{
	bool result = false;

	if (isConnected())
		result = (upperBladeControl_->canMove() && lowerBladeControl_->canMove());

	return result;
}

bool CLSJJSlitBladesControl::canStop() const
{
	bool result = false;

	if (isConnected())
		result = (upperBladeControl_->canStop() && lowerBladeControl_->canStop());

	return result;
}

bool CLSJJSlitBladesControl::validValue(double value) const
{
	bool result = false;

	if (value >= minimumValue() && value <= maximumValue())
		result = true;

	return result;
}

bool CLSJJSlitBladesControl::validSetpoint(double value) const
{
	bool result = false;

	if (value >= minimumValue() && value <= maximumValue())
		result = true;

	return result;
}

void CLSJJSlitBladesControl::setUpperBladeControl(AMControl *newControl)
{
	if (upperBladeControl_ != newControl) {

		if (upperBladeControl_)
			removeChildControl(upperBladeControl_);

		upperBladeControl_ = newControl;

		if (upperBladeControl_)
			addChildControl(upperBladeControl_);

		updateStates();

		emit upperBladeControlChanged(upperBladeControl_);
	}
}

void CLSJJSlitBladesControl::setLowerBladeControl(AMControl *newControl)
{
	if (lowerBladeControl_ != newControl) {

		if (lowerBladeControl_)
			removeChildControl(lowerBladeControl_);

		lowerBladeControl_ = newControl;

		if (lowerBladeControl_)
			addChildControl(lowerBladeControl_);

		updateStates();

		emit lowerBladeControlChanged(lowerBladeControl_);
	}
}

void CLSJJSlitBladesControl::setGap(double newValue)
{
	if (gap_ != newValue) {
		gap_ = newValue;
		emit gapChanged(gap_);
	}
}

void CLSJJSlitBladesControl::setCenterPosition(double newValue)
{
	if (centerPosition_ != newValue) {
		centerPosition_ = newValue;
		emit centerPositionChanged(centerPosition_);
	}
}

void CLSJJSlitBladesControl::updateStates()
{
	updateConnected();
	updateGap();
	updateCenterPosition();
	updateValue();
	updateMoving();
	updateMinimumValue();
	updateMaximumValue();
}

void CLSJJSlitBladesControl::updateConnected()
{
	bool upperControlOK = (upperBladeControl_ && upperBladeControl_->isConnected());
	bool lowerControlOK = (lowerBladeControl_ && lowerBladeControl_->isConnected());

	bool connected = (upperControlOK && lowerControlOK);

	setConnected(connected);
}

void CLSJJSlitBladesControl::updateMoving()
{
	if (isConnected()) {
		setIsMoving( upperBladeControl_->isMoving() || lowerBladeControl_->isMoving() );
	}
}

void CLSJJSlitBladesControl::updateGap()
{
	if (isConnected()) {
		setGap( calculateGap(upperBladeControl_->value(), lowerBladeControl_->value()) );
	}
}

void CLSJJSlitBladesControl::updateCenterPosition()
{
	if (isConnected()) {
		setCenterPosition( calculateCenterPosition(upperBladeControl_->value(), lowerBladeControl_->value()) );
	}
}

double CLSJJSlitBladesControl::calculateLowerPosition(double gap, double center)
{
	return center + (gap / 2.0);
}

double CLSJJSlitBladesControl::calculateUpperPosition(double gap, double center)
{
	return center - (gap / 2.0);
}

double CLSJJSlitBladesControl::calculateGap(double upperBladePosition, double lowerBladePosition)
{
	// Upper blade opens with a negative setpoint; lower blade opens with positive.
	return (lowerBladePosition - upperBladePosition);
}

double CLSJJSlitBladesControl::calculateCenterPosition(double upperBladePosition, double lowerBladePosition)
{
	return ((upperBladePosition - lowerBladePosition) / 2.0 + lowerBladePosition);
}
