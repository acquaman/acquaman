#include "CLSSplitIonChamber.h"

CLSSplitIonChamber::CLSSplitIonChamber(const QString &name, const QString &description, const QString &countsAPV, const QString &countsBPV, const QString &voltageAPV, const QString &voltageBPV, const QString &sensitivityNameA, const QString &sensitivityNameB, const QString &sensitivityUnitsA, const QString &sensitivityUnitsB, QObject *parent)
	: AMSplitIonChamber(name, description, parent)
{
	// Do stuff.
}

void CLSSplitIonChamber::onConnectedChanged()
{
	bool connected = (countsA_->isConnected() && countsB_->isConnected()
					  && voltageA_->isConnected() && voltageB_->isConnected()
					  && sensitivityA_->isConnected() && sensitivityB_->isConnected());

	if (connected != connected_)
		emit connected(connected_ = connected);
}
