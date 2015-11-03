#include "SGMOldEnergyPVSet.h"

#include "beamline/AMControlSet.h"
#include "beamline/AMPVControl.h"
SGMOldEnergyPVSet::SGMOldEnergyPVSet(QObject *parent) :
    QObject(parent)
{

	controlSet_ = new AMControlSet(this);
	connect(controlSet_, SIGNAL(connected(bool)), this, SIGNAL(connected(bool)));

    controlSet_->addControl(new AMSinglePVControl("UndulatorHarmonic",
                                                  "BL1611-ID-1:harmonic",
                                                  this,
                                                  0.5));

}

AMControl * SGMOldEnergyPVSet::undulatorHarmonic() const
{
	return controlSet_->controlNamed("UndulatorHarmonic");
}

bool SGMOldEnergyPVSet::isConnected() const
{
	return controlSet_->isConnected();
}
