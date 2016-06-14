#include "AMSlits.h"

AMSlits::AMSlits(const QString &name, QObject *parent) :
	AMControl(name, "", parent)
{
	allControls_ = new AMControlSet(this);
	connect( allControls_, SIGNAL(connected(bool)), this, SIGNAL(connected(bool)) );

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

bool AMSlits::isConnected() const
{
	return allControls_->isConnected();
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

bool AMSlits::addChildControl(AMControl *control)
{
	if (AMControl::addChildControl(control)){
		return allControls_->addControl(control);
    }
    return false;
}

bool AMSlits::removeChildControl(AMControl *control)
{
	if (AMControl::removeChildControl(control)){
		return !allControls_->removeControl(control);
    }
    return false;
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

void AMSlits::setVerticalSlitOpenGapValue(double openGap)
{
	if (verticalSlit_)
		verticalSlit_->setOpenGapValue(openGap);
}

void AMSlits::removeVerticalSlitOpenGapValue()
{
	if (verticalSlit_)
		verticalSlit_->removeOpenGapValue();
}

void AMSlits::setHorizontalSlitOpenGapValue(double openGap)
{
	if (horizontalSlit_)
		horizontalSlit_->setOpenGapValue(openGap);
}

void AMSlits::removeHorizontalSlitOpenGapValue()
{
	if (horizontalSlit_)
		horizontalSlit_->removeOpenGapValue();
}

void AMSlits::setVerticalSlitClosedGapValue(double closeGap)
{
	if (verticalSlit_)
		verticalSlit_->setClosedGapValue(closeGap);
}

void AMSlits::removeVerticalSlitClosedGapValue()
{
	if (verticalSlit_)
		verticalSlit_->removeClosedGapValue();
}

void AMSlits::setHorizontalSlitClosedGapValue(double closeGap)
{
	if (horizontalSlit_)
		horizontalSlit_->setClosedGapValue(closeGap);
}

void AMSlits::removeHorizontalClosedGapValue()
{
	if (horizontalSlit_)
		horizontalSlit_->removeClosedGapValue();
}
