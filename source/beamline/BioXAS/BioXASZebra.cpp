#include "BioXASZebra.h"

BioXASZebra::BioXASZebra(const QString &baseName, QObject *parent)
	: QObject(parent)
{
	connected_ = false;

	pulseControls_ << new BioXASZebraPulseControl(baseName, 1, this);
	pulseControls_ << new BioXASZebraPulseControl(baseName, 2, this);
	pulseControls_ << new BioXASZebraPulseControl(baseName, 3, this);
	pulseControls_ << new BioXASZebraPulseControl(baseName, 4, this);

	foreach(BioXASZebraPulseControl *pulseControl, pulseControls_)
		connect(pulseControl, SIGNAL(connectedChanged(bool)), this, SLOT(onConnectedChanged()));

	softIn1Control_ = new AMSinglePVControl("SoftIn1", "TRG1607-601:SOFT_IN:B0", this, 0.5);
}

BioXASZebra::~BioXASZebra()
{

}

bool BioXASZebra::isConnected() const
{
	return connected_;
}

QList<BioXASZebraPulseControl *> BioXASZebra::pulseControls() const
{
	return pulseControls_;
}

AMSinglePVControl* BioXASZebra::softIn1Control() const
{
	return softIn1Control_;
}

void BioXASZebra::onConnectedChanged()
{
	bool connected = true;

	foreach(BioXASZebraPulseControl *pulseControl, pulseControls_)
		connected &= pulseControl->isConnected();

	if (connected_ != connected){

		connected_ = connected;
		emit connectedChanged(connected_);
	}
}
