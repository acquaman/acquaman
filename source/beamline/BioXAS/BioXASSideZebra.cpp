#include "BioXASSideZebra.h"

BioXASSideZebra::BioXASSideZebra(const QString &baseName, QObject *parent) :
	BioXASZebra(baseName, parent)
{
	// Add the Side-specific output controls.

	outputControls_ << new BioXASZebraOutputControl(QString("OUT1 TTL"), QString("%1:OUT1_TTL"), this);
	outputControls_ << new BioXASZebraOutputControl(QString("OUT1 NIM"), QString("%1:OUT1_NIM"), this);
	outputControls_ << new BioXASZebraOutputControl(QString("OUT2 TTL"), QString("%1:OUT2_TTL"), this);

	foreach (AMControl *outputControl, outputControls_)
		connect( outputControl, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );

	onConnectedChanged();
}

BioXASSideZebra::~BioXASSideZebra()
{

}

