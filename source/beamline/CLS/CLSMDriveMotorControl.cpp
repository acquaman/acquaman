#include "CLSMDriveMotorControl.h"

#include <QStringBuilder>

CLSMDriveMotorControl::CLSMDriveMotorControl(const QString &name, const QString &baseName, const QString& units, double unitsPerRev, double offset, int microsteps, const QString &description, double tolerance, double moveStartTimeoutSeconds, QObject *parent)
	: AMPVwStatusAndUnitConversionControl(name, baseName % ":step:fbk", baseName % ":step", baseName % ":status", baseName % ":stop", new AMScaleAndOffsetUnitConverter(units, unitsPerRev/4000.0, offset), new AMScaleAndOffsetUnitConverter(units, unitsPerRev/(200.0*microsteps), offset), parent, tolerance, moveStartTimeoutSeconds, new AMControlStatusCheckerDefault(1), 1, description) {

}
