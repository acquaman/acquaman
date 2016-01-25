#include "BioXASZebraSoftInputControl.h"

BioXASZebraSoftInputControl::BioXASZebraSoftInputControl(const QString &name, const QString &PVname, QObject *parent, double tolerance, double completionTimeoutSeconds, const QString &description) :
	AMSinglePVControl(name, PVname, parent, tolerance, completionTimeoutSeconds, description)
{
	highValue_ = new AMSinglePVControl(name+".HIGH", PVname+".HIGH", this);
	connect( highValue_, SIGNAL(valueChanged(double)), this, SIGNAL(highValueChanged(double)) );
}

BioXASZebraSoftInputControl::~BioXASZebraSoftInputControl()
{

}

double BioXASZebraSoftInputControl::highValue() const
{
	double result = -1;

	if (highValue_->canMeasure())
		result = highValue_->value();

	return result;
}

void BioXASZebraSoftInputControl::setHighValue(double newValue)
{
	if (highValue_->canMove() && !highValue_->withinTolerance(newValue))
		highValue_->move(newValue);
}
