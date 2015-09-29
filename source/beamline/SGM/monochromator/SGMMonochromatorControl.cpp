#include "SGMMonochromatorControl.h"
#include "actions3/AMAction3.h"

SGMMonochromatorControl::SGMMonochromatorControl(QObject *parent) :
    AMPseudoMotorControl("SGM Monochromator", "eV", parent, "SGM Monochromator Energy")
{

}

bool SGMMonochromatorControl::shouldMeasure() const
{

}

bool SGMMonochromatorControl::shouldMove() const
{

}

bool SGMMonochromatorControl::shouldStop() const
{

}

bool SGMMonochromatorControl::canMeasure() const
{

}

bool SGMMonochromatorControl::canMove() const
{

}

bool SGMMonochromatorControl::canStop() const
{

}

bool SGMMonochromatorControl::validValue(double value) const
{

}

bool SGMMonochromatorControl::validSetpoint(double value) const
{

}

AMControl *SGMMonochromatorControl::undulatorControl() const
{

}

AMControl *SGMMonochromatorControl::gratingAngleControl() const
{

}

AMControl *SGMMonochromatorControl::gratingTranslationControl() const
{

}

AMControl *SGMMonochromatorControl::undulatorPositionControl() const
{

}

AMControl *SGMMonochromatorControl::exitSlitControl() const
{

}

AMAction3 *SGMMonochromatorControl::createMoveAction(double setpoint)
{
    AMAction3* energyMoveAction = 0;

    if(canMove()) {


    }
}

