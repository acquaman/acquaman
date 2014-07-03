/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#ifndef CLSPSEUDOMOTORGROUP_H
#define CLSPSEUDOMOTORGROUP_H

#include "beamline/AMMotorGroup.h"

/// This class extends the AMMotorGroupObject for the CLS pseudo motor application.  The main addition is adding the control which resets the pseudo motors since they don't have absolute references.
class CLSPseudoMotorGroupObject : public AMMotorGroupObject
{
	Q_OBJECT

public:
	/// Constructor.  Builds a new motor group object which mimics the AMMotorGroupObject with the addition of a reset control.
 	virtual ~CLSPseudoMotorGroupObject();
	CLSPseudoMotorGroupObject(const QString &name, const QString &prefix, const QString &units, AMControl *control, Orientation orientation, MotionType motionType, AMControl *resetControl, QObject *parent = 0);
	/// Constructor.  Builds a new motor group object which mimics the AMMotorGroupObject with up to three controls.
	CLSPseudoMotorGroupObject(const QString &name, const QStringList &prefixes, const QStringList &units, const QList<AMControl *> controls, QList<Orientation> orientations, QList<MotionType> motionTypes, AMControl *resetControl, QObject *parent = 0);

	/// Returns the reset control.
	AMControl *resetControl() const { return resetControl_; }

public slots:
	/// Reset this CLSPseudoMotorGroupObject.
	void resetPseudoMotor() const { resetControl_->move(1); }

protected:
	/// The control that handles the reset of the pseudo motor.
	AMControl *resetControl_;
};

/// This class extends the AMMotorGroup for the CLS pseudo motor application.
class CLSPseudoMotorGroup : public AMMotorGroup
{
	Q_OBJECT

public:
	/// Constructor.
	explicit CLSPseudoMotorGroup(QObject *parent = 0);
};

#endif // CLSPSEUDOMOTORGROUP_H
