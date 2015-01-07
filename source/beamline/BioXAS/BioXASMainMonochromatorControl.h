#ifndef BIOXASMAINMONOCHROMATORCONTROL_H
#define BIOXASMAINMONOCHROMATORCONTROL_H

#include "beamline/AMPVControl.h"

class BioXASMainMonochromatorControl : public AMPVwStatusControl
{
    Q_OBJECT
public:
    explicit BioXASMainMonochromatorControl(const QString& name,
                                   const QString& readPVname,
                                   const QString& writePVname,
                                   const QString& movingPVname,
                                   const QString& stopPVname = QString(),
                                   QObject* parent = 0,
                                   double tolerance = AMCONTROL_TOLERANCE_DONT_CARE,
                                   double moveStartTimeoutSeconds = 2.0,
                                   AMAbstractControlStatusChecker* statusChecker = new AMControlStatusCheckerDefault(1),
                                   int stopValue = 1,
                                   const QString &description = "");
    virtual ~BioXASMainMonochromatorControl();


public slots:
    AMControl::FailureExplanation move(double setpoint);

};

#endif // BIOXASMAINMONOCHROMATORCONTROL_H
