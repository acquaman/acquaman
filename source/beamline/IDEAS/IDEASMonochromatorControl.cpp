#include "IDEASMonochromatorControl.h"

IDEASMonochromatorControl::IDEASMonochromatorControl(QObject *parent)
    : AMPVwStatusControl("Energy", "BL08B2-1:Energy:EV:fbk", "BL08B2-1:Energy:EV", "BL08B2-1:Energy:status", "BL08B2-1:Energy:stop", parent, 25.0, 2.0, new AMControlStatusCheckerDefault(1), 1, "Mono Energy")
{
    connect(this, SIGNAL(connected(bool)), this, SLOT(onAllControlsConnected()));
}

void IDEASMonochromatorControl::onAllControlsConnected()
{
    setUnits("eV");
}
