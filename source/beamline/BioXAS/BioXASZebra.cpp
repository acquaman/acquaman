#include "BioXASZebra.h"

BioXASZebra::BioXASZebra(const QString &baseName, QObject *parent)
	: QObject(parent)
{
	connected_ = false;

	pulseControls_ << new BioXASZebraPulseControl(baseName, 1, this);
	pulseControls_ << new BioXASZebraPulseControl(baseName, 2, this);
	pulseControls_ << new BioXASZebraPulseControl(baseName, 3, this);
	pulseControls_ << new BioXASZebraPulseControl(baseName, 4, this);

	softInputControls_ << new AMSinglePVControl("SoftIn1", QString("%1:SOFT_IN:B0").arg(baseName), this, 0.5);
	softInputControls_ << new AMSinglePVControl("SoftIn2", QString("%1:SOFT_IN:B1").arg(baseName), this, 0.5);
	softInputControls_ << new AMSinglePVControl("SoftIn3", QString("%1:SOFT_IN:B2").arg(baseName), this, 0.5);
	softInputControls_ << new AMSinglePVControl("SoftIn4", QString("%1:SOFT_IN:B3").arg(baseName), this, 0.5);

	foreach(BioXASZebraPulseControl *pulseControl, pulseControls_)
		connect(pulseControl, SIGNAL(connectedChanged(bool)), this, SLOT(onConnectedChanged()));

	foreach(AMControl *control, softInputControls_)
		connect(control, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()));
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

BioXASZebraPulseControl *BioXASZebra::pulseControlAt(int index) const
{
	if (index >= 0 && index < 4)
		return pulseControls_.at(index);

	return 0;
}

QList<AMPVControl *> BioXASZebra::softInputControls() const
{
	return softInputControls_;
}

AMPVControl *BioXASZebra::softInputControlAt(int index) const
{
	if (index >= 0 && index < 4)
		return softInputControls_.at(index);

	return 0;
}

void BioXASZebra::onConnectedChanged()
{
	bool connected = true;

	foreach(BioXASZebraPulseControl *pulseControl, pulseControls_)
		connected &= pulseControl->isConnected();

	foreach (AMControl *control, softInputControls_)
		connected &= control->isConnected();

	if (connected_ != connected){

		connected_ = connected;
		emit connectedChanged(connected_);
	}
}
