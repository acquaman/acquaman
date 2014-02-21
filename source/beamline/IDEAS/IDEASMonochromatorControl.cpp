#include "IDEASMonochromatorControl.h"

IDEASMonochromatorControl::IDEASMonochromatorControl(QObject *parent)
    : AMPVwStatusControl("Energy", "BL08B2-1:Energy:EV:fbk", "BL08B2-1:Energy:EV", "SMTR1608-9-B20-05:brag.DMOV", "BL08B2-1:Energy:stop", parent, AMCONTROL_TOLERANCE_DONT_CARE, 2.0, new AMControlStatusCheckerDefault(0), 1, "Mono Energy")
    //: AMPVwStatusControl("Energy", "BL08B2-1:Energy:EV:fbk", "BL08B2-1:Energy:EV", "BL08B2-1:Energy:status", "BL08B2-1:Energy:stop", parent, AMCONTROL_TOLERANCE_DONT_CARE, 2.0, new AMControlStatusCheckerDefault(1), 1, "Mono Energy")
{
    connect(this, SIGNAL(connected(bool)), this, SLOT(onAllControlsConnected()));
}

void IDEASMonochromatorControl::onAllControlsConnected()
{
    setUnits("eV");
}
#include <QDebug>
AMControl::FailureExplanation IDEASMonochromatorControl::move(double setpoint)
{

    //qDebug() << "setpoint: "<< setpoint <<", setpoint_: " <<setpoint_;
    if (qAbs(setpoint-setpoint_) < 0.1){

        //qDebug() << "Faked it";
                emit moveSucceeded();
                return AMControl::NoFailure;
        }
    //qDebug() << "Commanded move to: " << setpoint;
        return AMPVwStatusControl::move(setpoint);
}
