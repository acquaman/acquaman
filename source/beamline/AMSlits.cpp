#include "AMSlits.h"
#include "beamline/AMSlit.h"

AMSlits::AMSlits(const QString &name, QObject *parent) :
	AMControl(name, "", parent)
{
	// Initialize vertical slit.

	verticalSlit_ = new AMSlit(QString("%1%2").arg(name).arg("VerticalSlit"), this);
	addChildControl(verticalSlit_);

	connect( verticalSlit_, SIGNAL(firstBladeValueChanged(double)), this, SIGNAL(upperBladeValueChanged(double)) );
	connect( verticalSlit_, SIGNAL(firstBladeChanged(AMControl*)), this, SIGNAL(upperBladeChanged(AMControl*)) );
	connect( verticalSlit_, SIGNAL(secondBladeValueChanged(double)), this, SIGNAL(lowerBladeValueChanged(double)) );
	connect( verticalSlit_, SIGNAL(secondBladeChanged(AMControl*)), this, SIGNAL(lowerBladeChanged(AMControl*)) );
	connect( verticalSlit_, SIGNAL(gapValueChanged(double)), this, SIGNAL(verticalGapValueChanged(double)) );
	connect( verticalSlit_, SIGNAL(centerValueChanged(double)), this, SIGNAL(verticalCenterValueChanged(double)) );

	// Initialize horizontal slit.

	horizontalSlit_ = new AMSlit(QString("%1%2").arg(name).arg("HorizontalSlit"), this);
	addChildControl(horizontalSlit_);

	connect( horizontalSlit_, SIGNAL(firstBladeValueChanged(double)), this, SIGNAL(inboardBladeValueChanged(double)) );
	connect( horizontalSlit_, SIGNAL(firstBladeChanged(AMControl*)), this, SIGNAL(inboardBladeChanged(AMControl*)) );
	connect( horizontalSlit_, SIGNAL(secondBladeValueChanged(double)), this, SIGNAL(outboardBladeValueChanged(double)) );
	connect( horizontalSlit_, SIGNAL(secondBladeChanged(AMControl*)), this, SIGNAL(outboardBladeChanged(AMControl*)) );
	connect( horizontalSlit_, SIGNAL(gapValueChanged(double)), this, SIGNAL(horizontalGapValueChanged(double)) );
	connect( horizontalSlit_, SIGNAL(centerValueChanged(double)), this, SIGNAL(horizontalCenterValueChanged(double)) );

}

AMSlits::~AMSlits()
{

}

double AMSlits::upperBladeValue() const
{
	double result = -1;

	if (verticalSlit_ && verticalSlit_->firstBlade() && verticalSlit_->firstBlade()->canMeasure())
		result = verticalSlit_->firstBlade()->value();

	return result;
}

double AMSlits::lowerBladeValue() const
{
	double result = -1;

	if (verticalSlit_ && verticalSlit_->secondBlade() && verticalSlit_->secondBlade()->canMeasure())
		result = verticalSlit_->secondBlade()->value();

	return result;
}

double AMSlits::inboardBladeValue() const
{
	double result = -1;

	if (horizontalSlit_ && horizontalSlit_->firstBlade() && horizontalSlit_->firstBlade()->canMeasure())
		result = horizontalSlit_->firstBlade()->value();

	return result;
}

double AMSlits::outboardBladeValue() const
{
	double result = -1;

	if (horizontalSlit_ && horizontalSlit_->secondBlade() && horizontalSlit_->secondBlade()->canMeasure())
		result = horizontalSlit_->secondBlade()->value();

	return result;
}

double AMSlits::verticalGapValue() const
{
	double result = -1;

	if (verticalSlit_)
		result = verticalSlit_->gapValue();

	return result;
}

double AMSlits::verticalCenterValue() const
{
	double result = -1;

	if (verticalSlit_)
		result = verticalSlit_->centerValue();

	return result;
}

double AMSlits::horizontalGapValue() const
{
	double result = -1;

	if (horizontalSlit_)
		result = horizontalSlit_->gapValue();

	return result;
}

double AMSlits::horizontalCenterValue() const
{
	double result = -1;

	if (horizontalSlit_)
		result = horizontalSlit_->centerValue();

	return result;
}

AMControl* AMSlits::upperBlade() const
{
	AMControl *result = 0;

	if (verticalSlit_)
		result = verticalSlit_->firstBlade();

	return result;
}

AMControl* AMSlits::lowerBlade() const
{
	AMControl *result = 0;

	if (verticalSlit_)
		result = verticalSlit_->secondBlade();

	return result;
}

AMControl* AMSlits::inboardBlade() const
{
	AMControl *result = 0;

	if (horizontalSlit_)
		result = horizontalSlit_->firstBlade();

	return result;
}

AMControl* AMSlits::outboardBlade() const
{
	AMControl *result = 0;

	if (horizontalSlit_)
		result = horizontalSlit_->secondBlade();

	return result;
}

AMControl* AMSlits::verticalGap() const
{
	return verticalSlit_->gap();
}

AMControl* AMSlits::verticalCenter() const
{
	return verticalSlit_->center();
}

AMControl* AMSlits::horizontalGap() const
{
	return horizontalSlit_->gap();
}

AMControl* AMSlits::horizontalCenter() const
{
	return horizontalSlit_->center();
}

void AMSlits::setUpperBlade(AMControl *newControl, AMSlit::BladeOrientation orientation)
{
	if (verticalSlit_)
		verticalSlit_->setFirstBlade(newControl, orientation);
}

void AMSlits::setLowerBlade(AMControl *newControl, AMSlit::BladeOrientation orientation)
{
	if (verticalSlit_)
		verticalSlit_->setSecondBlade(newControl, orientation);
}

void AMSlits::setInboardBlade(AMControl *newControl, AMSlit::BladeOrientation orientation)
{
	if (horizontalSlit_)
		horizontalSlit_->setFirstBlade(newControl, orientation);
}

void AMSlits::setOutboardBlade(AMControl *newControl, AMSlit::BladeOrientation orientation)
{
	if (horizontalSlit_)
		horizontalSlit_->setSecondBlade(newControl, orientation);
}

void AMSlits::setVerticalSlitOpenValues(double upperBladeOpenValue, double lowerBladeOpenValue)
{
	if (verticalSlit_)
		verticalSlit_->setOpenValues(upperBladeOpenValue, lowerBladeOpenValue);
}

void AMSlits::removeVerticalSlitOpenValues()
{
	if (verticalSlit_)
		verticalSlit_->removeOpenValues();
}

void AMSlits::setHorizontalSlitOpenValues(double inboardBladeOpenValue, double outboardBladeOpenValue)
{
	if (horizontalSlit_)
		horizontalSlit_->setOpenValues(inboardBladeOpenValue, outboardBladeOpenValue);
}

void AMSlits::removeHorizontalSlitOpenValues()
{
	if (horizontalSlit_)
		horizontalSlit_->removeOpenValues();
}

void AMSlits::setVerticalSlitClosedValues(double upperBladeClosedValue, double lowerBladeClosedValue)
{
	if (verticalSlit_)
		verticalSlit_->setClosedValues(upperBladeClosedValue, lowerBladeClosedValue);
}

void AMSlits::removeVerticalSlitClosedValues()
{
	if (verticalSlit_)
		verticalSlit_->removeClosedValues();
}

void AMSlits::setHorizontalSlitClosedValues(double inboardBladeClosedValue, double outboardBladeClosedValue)
{
	if (horizontalSlit_)
		horizontalSlit_->setClosedValues(inboardBladeClosedValue, outboardBladeClosedValue);
}

void AMSlits::removeHorizontalSlitClosedValues()
{
	if (horizontalSlit_)
		horizontalSlit_->removeClosedValues();
}
