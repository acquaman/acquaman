#include "CLSMDriveMotorControl.h"

#include <QStringBuilder>

CLSMDriveMotorControl::CLSMDriveMotorControl(const QString &name, const QString &baseName, const QString& units, double scaleFactor, double offset, const QString &description, double tolerance, double moveStartTimeoutSeconds, QObject *parent)
	: AMPVwStatusAndUnitConversionControl(name, baseName % ":step:fbk", baseName % ":step", baseName % ":status", baseName % ":stop", new AMScaleAndOffsetUnitConverter(units, scaleFactor, offset), parent, tolerance, moveStartTimeoutSeconds, new AMControlStatusCheckerDefault(1), 1, description) {

}
