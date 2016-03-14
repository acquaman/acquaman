#include "BioXASSollerSlit.h"
#include "beamline/CLS/CLSMAXvMotor.h"

BioXASSollerSlit::BioXASSollerSlit(const QString &name, QObject *parent) :
	BioXASBeamlineComponent(name, parent)
{
	// Initialize beamline components.

	xMotor_ = 0;
	zMotor_ = 0;
}

BioXASSollerSlit::~BioXASSollerSlit()
{

}

double BioXASSollerSlit::xValue() const
{
	double result = -1;

	if (xMotor_ && xMotor_->canMeasure())
		result = xMotor_->value();

	return result;
}

double BioXASSollerSlit::zValue() const
{
	double result = -1;

	if (zMotor_ && zMotor_->canMeasure())
		result = zMotor_->value();

	return result;
}

void BioXASSollerSlit::setXMotor(CLSMAXvMotor *newControl)
{
	if (xMotor_ != newControl) {

		if (xMotor_)
			removeChildControl(xMotor_);

		xMotor_ = newControl;

		if (xMotor_) {
			addChildControl(xMotor_);

			connect( xMotor_, SIGNAL(valueChanged(double)), this, SIGNAL(xValueChanged(double)) );
		}

		emit xMotorChanged(xMotor_);
	}
}

void BioXASSollerSlit::setZMotor(CLSMAXvMotor *newControl)
{
	if (zMotor_ != newControl) {

		if (zMotor_)
			removeChildControl(zMotor_);

		zMotor_ = newControl;

		if (zMotor_) {
			addChildControl(zMotor_);

			connect( zMotor_, SIGNAL(valueChanged(double)), this, SIGNAL(zValueChanged(double)) );
		}

		emit zMotorChanged(zMotor_);
	}
}
