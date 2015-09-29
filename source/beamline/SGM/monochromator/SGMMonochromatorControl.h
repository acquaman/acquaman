#ifndef SGMMONOCHROMATORCONTROL_H
#define SGMMONOCHROMATORCONTROL_H

#include <QObject>


#include "beamline/AMPseudoMotorControl.h"
#include "SGMMonochromatorInfo.h"

class AMAction3;
class SGMMonochromatorControl : public AMPseudoMotorControl
{
    Q_OBJECT
public:
    SGMMonochromatorControl(QObject* parent = 0);

    bool shouldMeasure() const;

    bool shouldMove() const;

    bool shouldStop() const;

    bool canMeasure() const;

    bool canMove() const;

    bool canStop() const;

    bool validValue(double value) const;

    bool validSetpoint(double value) const;

    AMControl* undulatorControl() const;

    AMControl* gratingAngleControl() const;

    AMControl* gratingTranslationControl() const;

    AMControl* undulatorPositionControl() const;

    AMControl* exitSlitControl() const;
protected:

    AMAction3* createMoveAction(double setpoint);
};

#endif // SGMMONOCHROMATORCONTROL_H
