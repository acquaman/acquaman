#include "BioXASZebra.h"

BioXASZebra::BioXASZebra(const QString &name, const QString &baseName, QObject *parent)
	: AMConnectedControl(name, "", parent)
{
	pulseControls_ << new BioXASZebraPulseControl(QString("%1%2").arg(baseName).arg("Pulse1"), baseName, 1, this);
	pulseControls_ << new BioXASZebraPulseControl(QString("%1%2").arg(baseName).arg("Pulse2"), baseName, 2, this);
	pulseControls_ << new BioXASZebraPulseControl(QString("%1%2").arg(baseName).arg("Pulse3"), baseName, 3, this);
	pulseControls_ << new BioXASZebraPulseControl(QString("%1%2").arg(baseName).arg("Pulse4"), baseName, 4, this);

	softInputControls_ << new BioXASZebraSoftInputControl("SoftIn1", QString("%1:SOFT_IN:B0").arg(baseName), this, 0.5);
	softInputControls_ << new BioXASZebraSoftInputControl("SoftIn2", QString("%1:SOFT_IN:B1").arg(baseName), this, 0.5);
	softInputControls_ << new BioXASZebraSoftInputControl("SoftIn3", QString("%1:SOFT_IN:B2").arg(baseName), this, 0.5);
	softInputControls_ << new BioXASZebraSoftInputControl("SoftIn4", QString("%1:SOFT_IN:B3").arg(baseName), this, 0.5);

	andBlocks_ << new BioXASZebraLogicBlock("AND1", QString("%1:AND1").arg(baseName), this);
	andBlocks_ << new BioXASZebraLogicBlock("AND2", QString("%1:AND2").arg(baseName), this);
	andBlocks_ << new BioXASZebraLogicBlock("AND3", QString("%1:AND3").arg(baseName), this);
	andBlocks_ << new BioXASZebraLogicBlock("AND4", QString("%1:AND4").arg(baseName), this);

	orBlocks_ << new BioXASZebraLogicBlock("OR1", QString("%1:OR1").arg(baseName), this);
	orBlocks_ << new BioXASZebraLogicBlock("OR2", QString("%1:OR2").arg(baseName), this);
	orBlocks_ << new BioXASZebraLogicBlock("OR3", QString("%1:OR3").arg(baseName), this);
	orBlocks_ << new BioXASZebraLogicBlock("OR4", QString("%1:OR4").arg(baseName), this);

	foreach(BioXASZebraPulseControl *pulseControl, pulseControls_)
		connect(pulseControl, SIGNAL(connected(bool)), this, SLOT(updateConnected()));

	foreach(AMControl *control, softInputControls_)
		connect(control, SIGNAL(connected(bool)), this, SLOT(updateConnected()));

	foreach (AMControl *andBlock, andBlocks_)
		connect( andBlock, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	foreach (AMControl *orBlock, orBlocks_)
		connect( orBlock, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
}

BioXASZebra::~BioXASZebra()
{

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

QList<BioXASZebraSoftInputControl *> BioXASZebra::softInputControls() const
{
	return softInputControls_;
}

BioXASZebraSoftInputControl *BioXASZebra::softInputControlAt(int index) const
{
	if (index >= 0 && index < 4)
		return softInputControls_.at(index);

	return 0;
}

QList<BioXASZebraLogicBlock*> BioXASZebra::andBlocks() const
{
	return andBlocks_;
}

BioXASZebraLogicBlock* BioXASZebra::andBlockAt(int index) const
{
	BioXASZebraLogicBlock *result = 0;

	if (index >= 0 && index < andBlocks_.count())
		result = andBlocks_.at(index);

	return result;
}

QList<BioXASZebraLogicBlock*> BioXASZebra::orBlocks() const
{
	return orBlocks_;
}

BioXASZebraLogicBlock* BioXASZebra::orBlockAt(int index) const
{
	BioXASZebraLogicBlock *result = 0;

	if (index >= 0 && index < orBlocks_.count())
		result = orBlocks_.at(index);

	return result;
}

void BioXASZebra::updateConnected()
{
	setConnected( controlsConnected() );
}

bool BioXASZebra::controlsConnected() const
{
	bool connected = true;

	foreach(BioXASZebraPulseControl *pulseControl, pulseControls_)
		connected &= pulseControl->isConnected();

	foreach (AMControl *control, softInputControls_)
		connected &= control->isConnected();

	foreach (AMControl *andBlock, andBlocks_)
		connected &= andBlock->isConnected();

	foreach (AMControl *orBlock, orBlocks_)
		connected &= orBlock->isConnected();

	return connected;
}
