#include "AMSlitControl.h"
#include "beamline/AMSlit.h"

AMSlitControl::AMSlitControl(const QString &name, QObject *parent) :
	AMPseudoMotorControl(name, "", parent)
{
	// Initialize class variables.

	firstBlade_ = 0;
	secondBlade_ = 0;
}

AMSlitControl::~AMSlitControl()
{

}

bool AMSlitControl::canMeasure() const
{
	bool result = false;

	if (isConnected())
		result = ( firstBlade_->canMeasure() && bladeOrientationMap_.contains(firstBlade_) && secondBlade_->canMeasure() && bladeOrientationMap_.contains(secondBlade_));

	return result;
}

bool AMSlitControl::canMove() const
{
	bool result = false;

	if (isConnected())
		result = ( firstBlade_->canMove() && bladeOrientationMap_.contains(firstBlade_) && secondBlade_->canMove() && bladeOrientationMap_.contains(secondBlade_));

	return result;
}

double AMSlitControl::firstBladeOrientation() const
{
	double result = -1;

	if (firstBlade_ && bladeOrientationMap_.contains(firstBlade_))
		result = bladeOrientationMap_.value(firstBlade_);

	return result;
}

double AMSlitControl::secondBladeOrientation() const
{
	double result = -1;

	if (secondBlade_ && bladeOrientationMap_.contains(secondBlade_))
		result = bladeOrientationMap_.value(secondBlade_);

	return result;
}

void AMSlitControl::setFirstBlade(AMControl *newControl, double orientation)
{
	if (firstBlade_ != newControl) {

		removeFirstBlade();

		firstBlade_ = newControl;

		if (addChildControl(firstBlade_)){
			bladeOrientationMap_.insert(firstBlade_, orientation);
		}

		emit firstBladeChanged(firstBlade_);
	}
}

void AMSlitControl::setSecondBlade(AMControl *newControl, double orientation)
{
	if (secondBlade_ != newControl) {

		removeSecondBlade();

		secondBlade_ = newControl;

		if (addChildControl(secondBlade_)){
			bladeOrientationMap_.insert(secondBlade_, orientation);
		}

		emit secondBladeChanged(secondBlade_);
	}
}

void AMSlitControl::removeFirstBlade()
{
	if (firstBlade_)
		removeBlade(firstBlade_);
}

void AMSlitControl::removeSecondBlade()
{
	if (secondBlade_)
		removeBlade(secondBlade_);
}

void AMSlitControl::updateMoving()
{
	if (isConnected())
		setIsMoving( firstBlade_->isMoving() || secondBlade_->isMoving() );
}

void AMSlitControl::updateUnits()
{
	if (isConnected()) {
		QString newUnits = "";

		QString firstBladeUnits = firstBlade_->units();
		QString secondBladeUnits = secondBlade_->units();

		if (firstBladeUnits == secondBladeUnits)
			newUnits = firstBladeUnits;

		setUnits(newUnits);
	}

}

void AMSlitControl::updateTolerance()
{
	if (isConnected()) {
		double newValue = 0;

		double firstBladeTolerance = firstBlade_->tolerance();
		double secondBladeTolerance = secondBlade_->tolerance();

		if (firstBladeTolerance >= secondBladeTolerance) {
			newValue = firstBladeTolerance;
		} else {
			newValue = secondBladeTolerance;
		}

		setTolerance(newValue);
	}
}


bool AMSlitControl::removeBlade(AMControl *control)
{
	if (removeChildControl(control)){
		bladeOrientationMap_.remove(control);
		return true;
	}
	return false;
}

double AMSlitControl::currentGap() const
{
	double result = -1;

	if (canMeasure())
		result = calculateGap(firstBlade_->value(), bladeOrientationMap_.value(firstBlade_), secondBlade_->value(), bladeOrientationMap_.value(secondBlade_));

	return result;
}

double AMSlitControl::currentCenter() const
{
	double result = -1;

	if (canMeasure())
		result = calculateCenter(firstBlade_->value(), bladeOrientationMap_.value(firstBlade_), secondBlade_->value(), bladeOrientationMap_.value(secondBlade_));

	return result;
}

double AMSlitControl::calculateGap(double firstBladeValue, double firstBladeOrientation, double secondBladeValue, double secondBladeOrientation) const
{
	double result = -1;

	if (firstBladeOrientation == AMSlit::OpensPositively && secondBladeOrientation == AMSlit::OpensNegatively) {
		result = firstBladeValue - secondBladeValue;

	} else if (firstBladeOrientation == AMSlit::OpensNegatively && secondBladeOrientation == AMSlit::OpensPositively) {
		result = secondBladeValue - firstBladeValue;
	}

	return result;
}

double AMSlitControl::calculateCenter(double firstBladeValue, double firstBladeOrientation, double secondBladeValue, double secondBladeOrientation) const
{
	double result = -1;

	if (firstBladeOrientation == AMSlit::OpensPositively && secondBladeOrientation == AMSlit::OpensNegatively) {
		result = ((secondBladeValue - firstBladeValue) / 2.0 + firstBladeValue);

	} else if (firstBladeOrientation == AMSlit::OpensNegatively && secondBladeOrientation == AMSlit::OpensPositively) {
		result = ((firstBladeValue - secondBladeValue) / 2.0 + secondBladeValue);
	}

	return result;
}

double AMSlitControl::calculateBladeValue(double bladeOrientation, double gap, double center) const
{
	double result = -1;

	if (int(bladeOrientation) == AMSlit::OpensPositively) {
		result = center + (gap / 2.0);
	} else if (int(bladeOrientation) == AMSlit::OpensNegatively) {
		result = center - (gap / 2.0);
	}

	return result;
}
