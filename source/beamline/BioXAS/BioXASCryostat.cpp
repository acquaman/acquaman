#include "BioXASCryostat.h"

BioXASCryostat::BioXASCryostat(const QString &name, QObject *parent) :
	BioXASBeamlineComponent(name, parent)
{
	// Initialize class variables.

	pressure_ = 0;
	temperature_ = 0;
}

BioXASCryostat::~BioXASCryostat()
{

}

double BioXASCryostat::pressureValue() const
{
	double result = -1;

	if (pressure_ && pressure_->canMeasure())
		result = pressure_->value();

	return result;
}

double BioXASCryostat::temperatureValue() const
{
	double result = -1;

	if (temperature_ && temperature_->canMeasure())
		result = temperature_->value();

	return result;
}

void BioXASCryostat::setPressure(AMControl *newControl)
{
	if (pressure_ != newControl) {

                removeChildControl(pressure_);

		pressure_ = newControl;

                if (addChildControl(pressure_))
			connect( pressure_, SIGNAL(valueChanged(double)), this, SIGNAL(pressureValueChanged(double)) );

		emit pressureChanged(pressure_);
	}
}

void BioXASCryostat::setTemperature(AMControl *newControl)
{
	if (temperature_ != newControl) {

                removeChildControl(temperature_);

		temperature_ = newControl;

                if (addChildControl(temperature_))
			connect( temperature_, SIGNAL(valueChanged(double)), this, SIGNAL(temperatureValueChanged(double)) );

		emit temperatureChanged(temperature_);
	}
}

