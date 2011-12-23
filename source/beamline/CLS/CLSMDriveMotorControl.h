/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef CLSMDRIVEMOTORCONTROL_H
#define CLSMDRIVEMOTORCONTROL_H

#include "beamline/AMControl.h"

/// This class is a subclass of AMPVwStatusControl that is useful for IMS MDrive motors at the CLS using the initial version of the driver that doesn't support unit conversion. (Future versions of that driver will have unit conversion built-in, and this class can switch to using AMPVwStatusControl instead of AMPVwStatusAndUnitConversionControl.
class CLSMDriveMotorControl : public AMPVwStatusAndUnitConversionControl
{
    Q_OBJECT
public:
	/// Constructor
	/*!
	  \param name A unique representative name for this motor
	  \param basePVName The base of the PV name (ie: if the motor status was "SMTR16114I1022:status" then the base is "SMTR16114I1022")
	  \param units The name of the units that describe positions for this motor
	  \param unitsPerRev The scale factor used to convert from motor revolutions to desired units (ie: units/rev).  value() = step position / (200*microsteps steps/rev) * unitsPerRev + offset.  value() = encoder position / (4000 lines/rev) * unitsPerRev + offset.
	  \param offset The offset used in the conversion from motor revolutions to desired units (in desired units).  value() = step position / (200*microsteps steps/rev) * unitsPerRev + offset.  value() = encoder position / (4000 lines/rev) * unitsPerRev + offset.
	  \param microsteps The microstep setting for the motor. We need to know this because the step position depends on the microstep setting, but the encoder feedback is always in 4000 lines/rev.
	  \param description A human readable description for this motor
	  \param tolerance The tolerance that will be used to determine if moves succeed in getting close enough to be considered a success. It should be specified in our desired units.
	  \param moveStartTimeout How long the motor has to start moving before something is considered wrong
	  \param parent QObject parent class

	  Implementation note: We need to use separate readUnitConverters and writeUnitConverters, since the feedback PV (readPV) is in encoder counts at 4000lines/rev, and the move PV (writePV) is in steps (ie: 200*microsteps/rev).
	  */
	CLSMDriveMotorControl(const QString &name, const QString &basePVName, const QString& units, double unitsPerRev, double offset, int microsteps, const QString &description, double tolerance, double moveStartTimeoutSeconds = 2.0, QObject *parent = 0);

signals:

public slots:

};

#endif // CLSMDRIVEMOTORCONTROL_H
