#ifndef BIOXASSSRLMONOCHROMATORENERGYCONTROL_H
#define BIOXASSSRLMONOCHROMATORENERGYCONTROL_H

#include <QObject>

#include "beamline/AMPVControl.h"

class BioXASSSRLMonochromatorEnergyControl : public AMPVwStatusControl
{
	Q_OBJECT
    public:
	explicit BioXASSSRLMonochromatorEnergyControl(const QString& name,
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
	virtual ~BioXASSSRLMonochromatorEnergyControl();


    public slots:
	AMControl::FailureExplanation move(double setpoint);

};

#endif // BIOXASSSRLMONOCHROMATORENERGYCONTROL_H
