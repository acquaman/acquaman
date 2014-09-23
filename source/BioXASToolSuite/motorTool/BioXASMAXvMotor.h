#ifndef BIOXASMAXVMOTOR_H
#define BIOXASMAXVMOTOR_H

#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/AMPVControl.h"

class BioXASMAXvMotor : public CLSMAXvMotor
{
public:
	BioXASMAXvMotor(const int categoryId, const QString &name, const QString &baseName, const QString &description, bool hasEncoder, double tolerance, double moveStartTimeoutSeconds = 2.0, QObject *parent = 0)
		: CLSMAXvMotor(name, baseName, description, hasEncoder, tolerance, moveStartTimeoutSeconds, parent)
	{
		category_ = categoryId;
		pvBaseName_ = baseName;

		statusPVControl_ = new AMReadOnlyPVControl("Motor moving status", statusPVName(), this);
	}

	int category() { return category_; }
	QString valuePVName() { return pvBaseName_ + ":mm"; }
	QString feedbackPVName() { return pvBaseName_ + ":mm:fbk"; }
	QString statusPVName() { return pvBaseName_ + ":status"; }
	QString CWPVName() { return pvBaseName_ + ":cw"; }
	QString CCWPVName() { return pvBaseName_ + ":ccw"; }

	AMReadOnlyPVControl *statusPVControl() { return statusPVControl_; }

private:
	int category_;
	QString pvBaseName_;

		AMReadOnlyPVControl* statusPVControl_;
};

#endif // BIOXASMAXVMOTOR_H
