#include "CLSJJSlits.h"
#include "beamline/AMSlit.h"
#include "beamline/CLS/CLSMAXvMotor.h"

CLSJJSlits::CLSJJSlits(const QString &name, const QString &upperBladePVName, const QString &lowerBladePVName, const QString &inboardBladePVName, const QString &outboardBladePVName, QObject *parent) :
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

	// Current settings.

	setUpperBlade(new CLSMAXvMotor(upperBladePVName, upperBladePVName, upperBladePVName, false, 0.005, 2.0, this));
	setLowerBlade(new CLSMAXvMotor(lowerBladePVName, lowerBladePVName, lowerBladePVName, false, 0.005, 2.0, this));

	setInboardBlade(new CLSMAXvMotor(inboardBladePVName, inboardBladePVName, inboardBladePVName, false, 0.005, 2.0, this));
	setOutboardBlade(new CLSMAXvMotor(outboardBladePVName, outboardBladePVName, outboardBladePVName, false, 0.005, 2.0, this));
}

CLSJJSlits::~CLSJJSlits()
{

}

bool CLSJJSlits::isConnected() const
{
	return allControls_->isConnected();
}

double CLSJJSlits::upperBladeValue() const
{
	double result = -1;

	if (verticalSlit_ && verticalSlit_->firstBlade() && verticalSlit_->firstBlade()->canMeasure())
		result = verticalSlit_->firstBlade()->value();

	return result;
}

double CLSJJSlits::lowerBladeValue() const
{
	double result = -1;

	if (verticalSlit_ && verticalSlit_->secondBlade() && verticalSlit_->secondBlade()->canMeasure())
		result = verticalSlit_->secondBlade()->value();

	return result;
}

double CLSJJSlits::inboardBladeValue() const
{
	double result = -1;

	if (horizontalSlit_ && horizontalSlit_->firstBlade() && horizontalSlit_->firstBlade()->canMeasure())
		result = horizontalSlit_->firstBlade()->value();

	return result;
}

double CLSJJSlits::outboardBladeValue() const
{
	double result = -1;

	if (horizontalSlit_ && horizontalSlit_->secondBlade() && horizontalSlit_->secondBlade()->canMeasure())
		result = horizontalSlit_->secondBlade()->value();

	return result;
}

double CLSJJSlits::verticalGapValue() const
{
	double result = -1;

	if (verticalSlit_)
		result = verticalSlit_->gapValue();

	return result;
}

double CLSJJSlits::verticalCenterValue() const
{
	double result = -1;

	if (verticalSlit_)
		result = verticalSlit_->centerValue();

	return result;
}

double CLSJJSlits::horizontalGapValue() const
{
	double result = -1;

	if (horizontalSlit_)
		result = horizontalSlit_->gapValue();

	return result;
}

double CLSJJSlits::horizontalCenterValue() const
{
	double result = -1;

	if (horizontalSlit_)
		result = horizontalSlit_->centerValue();

	return result;
}

AMControl* CLSJJSlits::upperBlade() const
{
	AMControl *result = 0;

	if (verticalSlit_)
		result = verticalSlit_->firstBlade();

	return result;
}

AMControl* CLSJJSlits::lowerBlade() const
{
	AMControl *result = 0;

	if (verticalSlit_)
		result = verticalSlit_->secondBlade();

	return result;
}

AMControl* CLSJJSlits::inboardBlade() const
{
	AMControl *result = 0;

	if (horizontalSlit_)
		result = horizontalSlit_->firstBlade();

	return result;
}

AMControl* CLSJJSlits::outboardBlade() const
{
	AMControl *result = 0;

	if (horizontalSlit_)
		result = horizontalSlit_->secondBlade();

	return result;
}

AMControl* CLSJJSlits::verticalGap() const
{
	return verticalSlit_->gap();
}

AMControl* CLSJJSlits::verticalCenter() const
{
	return verticalSlit_->center();
}

AMControl* CLSJJSlits::horizontalGap() const
{
	return horizontalSlit_->gap();
}

AMControl* CLSJJSlits::horizontalCenter() const
{
	return horizontalSlit_->center();
}

void CLSJJSlits::addChildControl(AMControl *control)
{
	if (allControls_->addControl(control))
		AMControl::addChildControl(control);
}

void CLSJJSlits::removeChildControl(AMControl *control)
{
	if (!allControls_->removeControl(control))
		AMControl::removeChildControl(control);
}

void CLSJJSlits::setUpperBlade(AMControl *newControl, AMSlit::BladeOrientation orientation)
{
	if (verticalSlit_)
		verticalSlit_->setFirstBlade(newControl, orientation);
}

void CLSJJSlits::setLowerBlade(AMControl *newControl, AMSlit::BladeOrientation orientation)
{
	if (verticalSlit_)
		verticalSlit_->setSecondBlade(newControl, orientation);
}

void CLSJJSlits::setInboardBlade(AMControl *newControl, AMSlit::BladeOrientation orientation)
{
	if (horizontalSlit_)
		horizontalSlit_->setFirstBlade(newControl, orientation);
}

void CLSJJSlits::setOutboardBlade(AMControl *newControl, AMSlit::BladeOrientation orientation)
{
	if (horizontalSlit_)
		horizontalSlit_->setSecondBlade(newControl, orientation);
}

void CLSJJSlits::setVerticalSlitOpenValues(double upperBladeOpenValue, double lowerBladeOpenValue)
{
	if (verticalSlit_)
		verticalSlit_->setOpenValues(upperBladeOpenValue, lowerBladeOpenValue);
}

void CLSJJSlits::removeVerticalSlitOpenValues()
{
	if (verticalSlit_)
		verticalSlit_->removeOpenValues();
}

void CLSJJSlits::setHorizontalSlitOpenValues(double inboardBladeOpenValue, double outboardBladeOpenValue)
{
	if (horizontalSlit_)
		horizontalSlit_->setOpenValues(inboardBladeOpenValue, outboardBladeOpenValue);
}

void CLSJJSlits::removeHorizontalSlitOpenValues()
{
	if (horizontalSlit_)
		horizontalSlit_->removeOpenValues();
}

void CLSJJSlits::setVerticalSlitClosedValues(double upperBladeClosedValue, double lowerBladeClosedValue)
{
	if (verticalSlit_)
		verticalSlit_->setClosedValues(upperBladeClosedValue, lowerBladeClosedValue);
}

void CLSJJSlits::removeVerticalSlitClosedValues()
{
	if (verticalSlit_)
		verticalSlit_->removeClosedValues();
}

void CLSJJSlits::setHorizontalSlitClosedValues(double inboardBladeClosedValue, double outboardBladeClosedValue)
{
	if (horizontalSlit_)
		horizontalSlit_->setClosedValues(inboardBladeClosedValue, outboardBladeClosedValue);
}

void CLSJJSlits::removeHorizontalSlitClosedValues()
{
	if (horizontalSlit_)
		horizontalSlit_->removeClosedValues();
}

