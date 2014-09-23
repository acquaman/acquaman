#ifndef BIOXASMAXVMOTOR_H
#define BIOXASMAXVMOTOR_H

#include "beamline/CLS/CLSMAXvMotor.h"

class BioXASMAXvMotor : public CLSMAXvMotor
{
public:
	BioXASMAXvMotor(const int categoryId, const QString &name, const QString &baseName, const QString &description, bool hasEncoder, double tolerance, double moveStartTimeoutSeconds = 2.0, QObject *parent = 0)
		: CLSMAXvMotor(name, baseName, description, hasEncoder, tolerance, moveStartTimeoutSeconds, parent)
	{
		category_ = categoryId;
		pvBaseName_ = baseName;
	}

	int category() { return category_; }
	QString valuePVName() { return pvBaseName_ + ":mm"; }
	QString feedbackPVName() { return pvBaseName_ + ":mm:fbk"; }
	QString statusPVName() { return pvBaseName_ + ":status"; }
	QString CWPVName() { return pvBaseName_ + ":cw"; }
	QString CCWPVName() { return pvBaseName_ + ":ccw"; }

private:
	int category_;
	QString pvBaseName_;
};

#endif // BIOXASMAXVMOTOR_H
