#include "AMSlit.h"

AMSlit::AMSlit(const QString &name, QObject *parent) :
	AMControl(parent)
{
	// Initialize class variables.

	gap_ = 0;
	center_ = 0;
}

AMSlit::~AMSlit()
{

}

bool AMSlit::isConnected() const
{
	return allControls_->isConnected();
}

double AMSlit::gapValue() const
{
	return gap_->value();
}

double AMSlit::centerValue() const
{
	return center_->value();
}

void AMSlit::setGapControl(AMControl *newControl)
{
	if (gap_ != newControl) {

		if (gap_)
			removeChildControl(gap_); // disconnects all signals.

		gap_ = newControl;

		if (gap_) {
			addChildControl(gap_);
			connect( gap_, SIGNAL(valueChanged(double)), this, SIGNAL(gapValueChanged(double)) );
		}

		emit gapChanged(gap_);
	}
}

void AMSlit::setCenterControl(AMControl *newControl)
{
	if (center_ != newControl) {

		if (center_)
			removeChildControl(center_); // disconnects all signals.

		center_ = newControl;

		if (center_) {
			addChildControl(center_);
			connect( center_, SIGNAL(valueChanged(double)), this, SIGNAL(centerValueChanged(double)) );
		}

		emit centerChanged(center_);
	}
}
