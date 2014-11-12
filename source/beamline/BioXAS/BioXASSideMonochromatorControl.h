#ifndef BIOXASSIDEMONOCHROMATORCONTROL_H
#define BIOXASSIDEMONOCHROMATORCONTROL_H

#include "beamline/AMPVControl.h"

class BioXASSideMonochromatorControl : public AMPVwStatusControl
{
    Q_OBJECT
public:
    explicit BioXASSideMonochromatorControl(const QString& name,
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
    virtual ~BioXASSideMonochromatorControl();


public slots:
    AMControl::FailureExplanation move(double setpoint);

};

#endif // BIOXASSIDEMONOCHROMATORCONTROL_H
