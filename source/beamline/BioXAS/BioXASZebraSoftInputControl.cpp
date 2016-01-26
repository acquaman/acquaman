#include "BioXASZebraSoftInputControl.h"

BioXASZebraSoftInputControl::BioXASZebraSoftInputControl(const QString &name, const QString &PVname, QObject *parent, double tolerance, double completionTimeoutSeconds, const QString &description) :
	AMSinglePVControl(name, PVname, parent, tolerance, completionTimeoutSeconds, description)
{
	timeBeforeResetPreference_ = 0.01;

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
	return timeBeforeResetControl_->value();
}

void BioXASZebraSoftInputControl::setTimeBeforeReset(double newValue)
{
	if (timeBeforeResetControl_->canMove())
		timeBeforeResetControl_->move(newValue);
}

void BioXASZebraSoftInputControl::setTimeBeforeResetPreference(double newValue)
{
	if (timeBeforeResetPreference_ != newValue) {
		timeBeforeResetPreference_ = newValue;
		updateTimeBeforeResetControl();
	}
}

void BioXASZebraSoftInputControl::updateTimeBeforeResetControl()
{
	setTimeBeforeReset(timeBeforeResetPreference_);
}
