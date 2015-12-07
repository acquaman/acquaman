#include "BioXASSIS3820Scaler.h"
#include "beamline/AMPVControl.h"

BioXASSIS3820Scaler::BioXASSIS3820Scaler(const QString &baseName, QObject *parent) :
	CLSSIS3820Scaler(baseName, parent)
{
	inputsMode_ = new AMSinglePVControl("InputMode", baseName+":inputMode", this);
	triggerSourceMode_ = new AMSinglePVControl("TriggerSource", baseName+":triggerSource", this);
	clockSourceMode_ = new AMSinglePVControl("ClockSource", baseName+":source", this);
}

BioXASSIS3820Scaler::~BioXASSIS3820Scaler()
{

}

bool BioXASSIS3820Scaler::isConnected() const
{
	bool connected = (
				CLSSIS3820Scaler::isConnected() &&
				inputsMode_ && inputsMode_->isConnected() &&
				triggerSourceMode_ && triggerSourceMode_->isConnected() &&
				clockSourceMode_ && clockSourceMode_->isConnected()
				);

	return connected;
}

