#ifndef BIOXASCLSMAXVMOTOR_H
#define BIOXASCLSMAXVMOTOR_H

#include "beamline/AMPVControl.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/BioXAS/BioXASBeamlineDef.h"

class BioXASCLSMAXvMotor : public CLSMAXvMotor
{
	Q_OBJECT

public:
	BioXASCLSMAXvMotor(BioXASBeamlineDef::BioXASMotorType motorType, const QString& name, const QString& baseName, const QString& description, QString pvUnitFieldName, bool hasEncoder, double tolerance, double moveStartTimeoutSeconds = 2.0, QObject *parent = 0)
		: CLSMAXvMotor(name, baseName, description, hasEncoder, tolerance, moveStartTimeoutSeconds, parent, pvUnitFieldName)
	{
		type_ = motorType;
		pvBaseName_ = baseName;
		pvUnitFieldName_ = pvUnitFieldName;

		statusPVControl_ = new AMReadOnlyPVControl("Motor moving status", statusPVName(), this);
	}

	BioXASBeamlineDef::BioXASMotorType type() { return type_; }
	QString valuePVName() { return pvBaseName_ + pvUnitFieldName_; }
	QString feedbackPVName() { return pvBaseName_ + pvUnitFieldName_ + ":fbk"; }
	QString statusPVName() { return pvBaseName_ + ":status"; }
	QString CWPVName() { return pvBaseName_ + ":cw"; }
	QString CCWPVName() { return pvBaseName_ + ":ccw"; }

	/* The status PV control, which can be used as the statusTagControl for control editor*/
	AMReadOnlyPVControl *statusPVControl() { return statusPVControl_; }

private:
	BioXASBeamlineDef::BioXASMotorType type_;
	QString pvBaseName_;
	QString pvUnitFieldName_;

	AMReadOnlyPVControl* statusPVControl_;
};

#endif // BIOXASCLSMAXVMOTOR_H
