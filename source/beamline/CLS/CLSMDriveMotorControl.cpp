/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "CLSMDriveMotorControl.h"

#include <QStringBuilder>

CLSMDriveMotorControl::CLSMDriveMotorControl(const QString &name, const QString &baseName, const QString& units, double unitsPerRev, double offset, int microsteps, const QString &description, double tolerance, double moveStartTimeoutSeconds, QObject *parent)
	: AMPVwStatusAndUnitConversionControl(name, baseName % ":enc:fbk", baseName % ":step", baseName % ":status", baseName % ":stop", new AMScaleAndOffsetUnitConverter(units, unitsPerRev/4000.0, offset), new AMScaleAndOffsetUnitConverter(units, unitsPerRev/(200.0*microsteps), offset), parent, tolerance, moveStartTimeoutSeconds, new AMControlStatusCheckerDefault(1), 1, description) {

	// Unlike MaxV controllers, these motors can support move updates while moving:
	setAllowsMovesWhileMoving(true);
	// Because of the polled communication, it can take a while for these motors to send MOVE ACTIVE then MOVE DONE for null moves.  We use the moveStartThreshold to say that moves within 5 microsteps can be considered to already be in position.
	setMoveStartTolerance(5);	// in microsteps.

}
