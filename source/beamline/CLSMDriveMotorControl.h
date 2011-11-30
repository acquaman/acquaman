#ifndef CLSMDRIVEMOTORCONTROL_H
#define CLSMDRIVEMOTORCONTROL_H

#include "beamline/AMControl.h"

/// This class is a subclass of AMPVwStatusControl that is useful for IMS MDrive motors at the CLS using the initial version of the driver that doesn't support unit conversion. (Future versions of that driver will have unit conversion built-in, and this class can switch to using AMPVwStatusControl instead of AMPvwStatusAndUnitConversionControl.
class CLSMDriveMotorControl : public AMPVwStatusAndUnitConversionControl
{
    Q_OBJECT
public:
	/// Constructor
	/*!
	  \param name A unique representative name for this motor
	  \param basePVName The base of the PV name (ie: if the motor status was "SMTR16114I1022:status" then the base is "SMTR16114I1022")
	  \param units The name of the units that describe positions for this motor
	  \param scaleFactor The scale factor used to convert from raw steps to units.  value() = step value * scaleFactor + offset.
	  \param offset The offset used in the conversion from raw steps to units.  value() = step value * scaleFactor + offset.
	  \param description A human readable description for this motor
	  \param tolerance The tolerance that will be used to determine if moves succeed in getting close enough to be considered a success
	  \param moveStartTimeout How long the motor has to start moving before something is considered wrong
	  \param parent QObject parent class
	  */
	CLSMDriveMotorControl(const QString &name, const QString &basePVName, const QString& units, double scaleFactor, double offset, const QString &description, double tolerance, double moveStartTimeoutSeconds = 2.0, QObject *parent = 0);

signals:

public slots:

};

#endif // CLSMDRIVEMOTORCONTROL_H
