#include "BioXASMainZebra.h"

BioXASMainZebra::BioXASMainZebra(const QString &baseName, QObject *parent) :
	BioXASZebra(baseName, parent)
{
	// Add the Main-specific output controls.

	outputControls_ << new BioXASZebraOutputControl(QString("OUT1 TTL"), QString("%1:OUT1_TTL").arg(baseName), this);
	outputControls_ << new BioXASZebraOutputControl(QString("OUT1 NIM"), QString("%1:OUT1_NIM").arg(baseName), this);
	outputControls_ << new BioXASZebraOutputControl(QString("OUT2 TTL"), QString("%1:OUT2_TTL").arg(baseName), this);
	outputControls_ << new BioXASZebraOutputControl(QString("OUT3 TTL"), QString("%1:OUT3_TTL").arg(baseName), this);

	foreach (AMControl *outputControl, outputControls_)
		connect( outputControl, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );

	onConnectedChanged();
}

BioXASMainZebra::~BioXASMainZebra()
{

}

