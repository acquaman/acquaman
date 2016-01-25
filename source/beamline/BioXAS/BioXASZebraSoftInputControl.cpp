#include "BioXASZebraSoftInputControl.h"

BioXASZebraSoftInputControl::BioXASZebraSoftInputControl(const QString &name, const QString &PVname, QObject *parent, double tolerance, double completionTimeoutSeconds, const QString &description) :
	AMSinglePVControl(name, PVname, parent, tolerance, completionTimeoutSeconds, description)
{
	timeBeforeResetValue_ = 0.01;

	timeBeforeResetControl_ = new AMSinglePVControl(name+".HIGH", PVname+".HIGH", this);
	connect( timeBeforeResetControl_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)) );
	connect( timeBeforeResetControl_, SIGNAL(connected(bool)), this, SLOT(updateTimeBeforeResetControl()) );
	connect( timeBeforeResetControl_, SIGNAL(valueChanged(double)), this, SIGNAL(timeBeforeResetChanged(double)) );
}

BioXASZebraSoftInputControl::~BioXASZebraSoftInputControl()
{

}

bool BioXASZebraSoftInputControl::isConnected() const
{
	return (AMSinglePVControl::isConnected() && timeBeforeResetControl_->isConnected());
}

double BioXASZebraSoftInputControl::timeBeforeReset() const
{
	double result = -1;

	if (timeBeforeResetControl_->canMeasure())
		result = timeBeforeResetControl_->value();

	return result;
}

void BioXASZebraSoftInputControl::setTimeBeforeReset(double newValue)
{
	if (timeBeforeResetValue_ != newValue) {
		timeBeforeResetValue_ = newValue;
		updateTimeBeforeResetControl();
	}
}

void BioXASZebraSoftInputControl::updateTimeBeforeResetControl()
{
	if (timeBeforeResetControl_->canMove() && !timeBeforeResetControl_->withinTolerance(timeBeforeResetValue_))
		timeBeforeResetControl_->move(timeBeforeResetValue_);
}
