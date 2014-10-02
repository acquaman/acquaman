#ifndef BIOXASCLSMAXVMOTOR_H
#define BIOXASCLSMAXVMOTOR_H

#include "beamline/AMPVControl.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/BioXAS/BioXASBeamlineDef.h"

class BioXASCLSMAXvMotor : public CLSMAXvMotor
{
	Q_OBJECT

public:
	BioXASCLSMAXvMotor(BioXASBeamlineDef::BioXASMotorType motorType, const QString& name, const QString& baseName, const QString& description, QString pvUnitFieldName, bool hasEncoder, double tolerance, double moveStartTimeoutSeconds = 2.0, QObject *parent = 0);

	BioXASBeamlineDef::BioXASMotorType type();
	QString valuePVName();
	QString feedbackPVName();
	QString statusPVName();
	QString CWPVName();
	QString CCWPVName();

	/* The status PV control, which can be used as the statusTagControl for control editor*/
	AMReadOnlyPVControl *statusPVControl();

public slots:
	/// Start a move to the value setpoint (reimplemented)
	virtual FailureExplanation move(double setpoint);

private:
	BioXASBeamlineDef::BioXASMotorType type_;
	QString pvBaseName_;
	QString pvUnitFieldName_;

	AMReadOnlyPVControl* statusPVControl_;
};

#endif // BIOXASCLSMAXVMOTOR_H
