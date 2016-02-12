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
		result = ( firstBlade_->canMeasure() && secondBlade_->canMeasure() );

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

		if (firstBlade_)
			removeFirstBlade();

		firstBlade_ = newControl;

		if (firstBlade_) {
			bladeOrientationMap_.insert(firstBlade_, orientation);
			addChildControl(firstBlade_);
		}

		emit firstBladeChanged(firstBlade_);
	}
}

void AMSlitControl::removeFirstBlade()
{
	if (firstBlade_) {
		bladeOrientationMap_.remove(firstBlade_);
		removeChildControl(firstBlade_);
	}
}

void AMSlitControl::setSecondBlade(AMControl *newControl, double orientation)
{
	if (secondBlade_ != newControl) {

		if (secondBlade_)
			removeSecondBlade();

		secondBlade_ = newControl;

		if (secondBlade_) {
			bladeOrientationMap_.insert(secondBlade_, orientation);
			addChildControl(secondBlade_);
		}

		emit secondBladeChanged(secondBlade_);
	}
}

void AMSlitControl::removeSecondBlade()
{
	if (secondBlade_) {
		bladeOrientationMap_.remove(secondBlade_);
		removeChildControl(secondBlade_);
	}
}

void AMSlitControl::updateMoving()
{
	bool moving = (
				(firstBlade_ && firstBlade_->isMoving()) ||
				(secondBlade_ && secondBlade_->isMoving())
				);

	setIsMoving(moving);
}

void AMSlitControl::updateUnits()
{
	QString newUnits = "";

	if (firstBlade_ && secondBlade_) {
		QString firstBladeUnits = firstBlade_->units();
		QString secondBladeUnits = secondBlade_->units();

		if (firstBladeUnits == secondBladeUnits)
			newUnits = firstBladeUnits;
	}

	setUnits(newUnits);
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

double AMSlitControl::calculateFirstBladeValue(double bladeOrientation, double gap, double center) const
{
	double result = -1;

	if (bladeOrientation == AMSlit::OpensPositively)
		result = center + (gap / 2.0);
	else if (bladeOrientation == AMSlit::OpensNegatively)
		result = center - (gap / 2.0);

	return result;
}

double AMSlitControl::calculateSecondBladeValue(double bladeOrientation, double gap, double center) const
{
	double result = -1;

	if (bladeOrientation == AMSlit::OpensPositively)
		result = center + (gap / 2.0);
	else if (bladeOrientation == AMSlit::OpensNegatively)
		result = center - (gap / 2.0);

	return result;
}
