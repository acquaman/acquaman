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

/*!
  * A class representing a motor group object for a CLS pseudo motor. The main
  * addition is a control which resets the pseudo motors since they don't have
  * absolute references.
  */
class CLSPseudoMotorGroupObject : public AMMotorGroupObject
{
	Q_OBJECT

public:
	/*!
	  * Creates an instance of a pseudo motor group object with the provided name,
	  * and the provided reset control. This object will contain no axes.
	  * \param name ~ A human readable name with which to associate this pseudo
	  * motor group object.
	  * \param resetControl ~ The control which handles resetting the pseudo
	  * motor.
	  */
	CLSPseudoMotorGroupObject(const QString &name,
							  AMControl* resetControl);

	/*!
	  * Creates an instance of a pseudo motor group object with the provided name,
	  * axes and reset control.
	  * \param name ~ A human readable name with which to associate this pseudo
	  * motor group object.
	  * \param axes ~ A collection of the axes which comprise this motor group.
	  * \param resetControl ~ The control which handles resetting the pseudo
	  * motor.
	  */
	CLSPseudoMotorGroupObject(const QString &name,
							  const QMap<MotionDirection, AMMotorGroupAxis*>& axes,
							  AMControl* resetControl);

	virtual ~CLSPseudoMotorGroupObject() {}

	/*!
	  * The motor groups reset control.
	  */
	AMControl *resetControl() const { return resetControl_; }

public slots:
	/*!
	  * Performs a reset on this pseudo motor group object.
	  */
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
