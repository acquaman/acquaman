#include "AMSlit.h"
#include "beamline/AMSlitGap.h"

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

double AMSlit::gapValue() const
{
	double result = -1;

	if (gap_ && gap_->canMeasure())
		result = gap_->value();

	return result;
}

double AMSlit::centerValue() const
{
	double result = -1;

	if (center_ && center_->canMeasure())
		result = center_->value();

	return result;
}

void AMSlit::addBlade(AMControl *newControl, double orientation)
{
	if (newControl && !blades_.contains(newControl)) {
		blades_.append(newControl);

		if (gap_)
			gap_->addBlade(newControl, orientation);

		if (center_)
			center_->addBlade(newControl, orientation);

		emit bladesChanged();
	}
}

void AMSlit::removeBlade(AMControl *control)
{
	if (control && blades_.contains(control)) {
		blades_.removeFirst(control);

		if (gap_)
			gap_->removeBlade(control, orientation);

		if (center_)
			center_->removeBlade(control, orientation);

		emit bladesChanged();
	}
}

void AMSlit::clearBlades()
{
	blades_.clear();

	if (gap_)
		gap_->clearBlades();

	if (center_)
		center_->clearBlades();

	emit bladesChanged();
}

void AMSlit::setGap(AMControl *newControl)
{
	if (gap_ != newControl) {

		if (gap_)
			removeChildControl(gap_);

		gap_ = newControl;

		if (gap_) {
			addChildControl(gap_);
			connect( gap_, SIGNAL(valueChanged(double)), this, SIGNAL(gapValueChanged(double)) );
		}

		emit gapChanged(gap_);
	}
}

void AMSlit::setCenter(AMControl *newControl)
{
	if (center_ != newControl) {

		if (center_)
			removeChildControl(center_);

		center_ = newControl;

		if (center_) {
			addChildControl(center_);
			connect( center_, SIGNAL(valueChanged(double)), this, SIGNAL(centerValueChanged(double)) );
		}

		emit centerChanged(center_);
	}
}
