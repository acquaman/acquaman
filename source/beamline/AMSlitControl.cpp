#include "AMSlitControl.h"
#include "beamline/AMSlit.h"

AMSlitControl::AMSlitControl(const QString &name, const QString &units, QObject *parent) :
	AMControl(name, units, parent)
{
	// Initialize class variables.

	upperBlade_ = 0;
	lowerBlade_ = 0;
}

AMSlitControl::~AMSlitControl()
{

}

bool AMSlitControl::isConnected() const
{
	return false;
}

double AMSlitControl::upperBladeValue() const
{
	return upperBlade_->value();
}

double AMSlitControl::lowerBladeValue() const
{
	return lowerBlade_->value();
}

void AMSlitControl::setUpperBlade(AMControl *newControl, double orientation)
{
	if (upperBlade_ != newControl) {

		if (upperBlade_) {
			removeChildControl(upperBlade_); // disconnects all signals.
			bladeOrientationMap_.remove(upperBlade_);
		}

		upperBlade_ = newControl;

		if (upperBlade_) {
			addChildControl(upperBlade_);
			bladeOrientationMap_.insert(upperBlade_, orientation);
			connect( upperBlade_, SIGNAL(valueChanged(double)), this, SIGNAL(upperBladeValueChanged(double)) );
		}

		emit upperBladeChanged(upperBlade_);
	}
}

void AMSlitControl::setLowerBlade(AMControl *newControl, double orientation)
{
	if (lowerBlade_ != newControl) {

		if (lowerBlade_) {
			removeChildControl(lowerBlade_);
			bladeOrientationMap_.remove(lowerBlade_);
		}

		lowerBlade_ = newControl;

		if (lowerBlade_) {
			addChildControl(lowerBlade_);
			bladeOrientationMap_.insert(lowerBlade_, orientation);
			connect( lowerBlade_, SIGNAL(valueChanged(double)), this, SIGNAL(lowerBladeValueChanged(double)) );
		}

		emit lowerBladeChanged(lowerBlade_);
	}
}

double AMSlitControl::calculateGap(double upperBladeValue, double upperBladeOrientation, double lowerBladeValue, double lowerBladeOrientation)
{
	double result = -1;

	if (upperBladeOrientation == AMSlit::OpensPositively && lowerBladeOrientation == AMSlit::OpensNegatively) {
		result = upperBladeValue - lowerBladeValue;

	} else if (upperBladeOrientation == AMSlit::OpensNegatively && lowerBladeOrientation == AMSlit::OpensPositively) {
		result = lowerBladeValue - upperBladeValue;
	}

	return result;
}

double AMSlitControl::calculateCenter(double upperBladeValue, double upperBladeOrientation, double lowerBladeValue, double lowerBladeOrientation)
{
	double result = -1;

	if (upperBladeOrientation == AMSlit::OpensPositively && lowerBladeOrientation == AMSlit::OpensNegatively) {
		result = ((lowerBladeValue - upperBladeValue) / 2.0 + upperBladeValue);

	} else if (upperBladeOrientation == AMSlit::OpensNegatively && lowerBladeOrientation == AMSlit::OpensPositively) {
		result = ((upperBladeValue - lowerBladeValue) / 2.0 + lowerBladeValue);
	}

	return result;
}
