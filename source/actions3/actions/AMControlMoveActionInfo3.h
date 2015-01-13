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


#ifndef AMCONTROLMOVEACTIONINFO3_H
#define AMCONTROLMOVEACTIONINFO3_H

#include "actions3/AMActionInfo3.h"
#include "dataman/info/AMControlInfo.h"

/// This AMActionInfo-subclass specifies the information for AMControlMoveAction -- an action that moves a control to a setpoint.  This info specifies the setpoint (the name of the control and where to move it to), in the form of an AMControlInfo.
class AMControlMoveActionInfo3 : public AMActionInfo3
{
	Q_OBJECT
	Q_PROPERTY(AMDbObject* controlInfo READ dbReadControlInfo WRITE dbLoadControlInfo)
	Q_PROPERTY(bool isRelativeMove READ isRelativeMove WRITE setIsRelativeMove)
	Q_PROPERTY(bool isRelativeFromSetpoint READ isRelativeFromSetpoint WRITE setIsRelativeFromSetpoint)

public:
	/// Constructor. You should always specify a valid \c setpoint, but we provide the default argument because we need a default constructor for database loading.
	Q_INVOKABLE AMControlMoveActionInfo3(const AMControlInfo &setpoint = AMControlInfo(), QObject *parent = 0);
	/// Destructor.
	virtual ~AMControlMoveActionInfo3();

	/// Copy Constructor
	AMControlMoveActionInfo3(const AMControlMoveActionInfo3 &other);

	/// This function is used as a virtual copy constructor
	virtual AMActionInfo3* createCopy() const;

	// Re-implemented public functions
	/////////////////////////////////

	/// This should describe the type of the action
	virtual QString typeDescription() const { return "Control Move"; }

	// New public functions
	//////////////////////////

	/// Returns a pointer to our move destination setpoint
//	const AMControlInfo* controlInfo() const { return &controlInfo_; }
	AMControlInfo controlInfo() const { return controlInfo_; }
	/// Returns true if this is to be a relative move (otherwise returns false for absolute).
	bool isRelativeMove() const { return isRelative_; }
	/// Returns true if this is to be a relative move from the setpoint as opposed to the feedback value
	bool isRelativeFromSetpoint() const { return isRelativeFromSetpoint_; }

	/// Set the move destination setpoint, including the control name, value, and description.
	/*! \note We make a copy of \c controlInfo's values, and do not retain any reference to it afterward. */
	void setControlInfo(const AMControlInfo& controlInfo);
	/// Set the move destination setpoint (value only).
	void setSetpoint(double setpoint);
	/// Sets whether this should be a relative (rather than absolute) move.  Absolute is the default.
	void setIsRelativeMove(bool isRelative);
	/// Sets whether this should be a relative move from the setpoint as opposed to the feedback value.
	void setIsRelativeFromSetpoint(bool isRelativeFromSetpoint);

public slots:

signals:

protected:
	// Database loading/storing
	////////////////////////////

	/// For database storing only.
	AMControlInfo* dbReadControlInfo() { return &controlInfo_; }
	/// For database loading only. This function will never be called since dbReadControlInfo() always returns a valid setpoint, but it needs to be here.
	void dbLoadControlInfo(AMDbObject* newLoadedObject) { newLoadedObject->deleteLater(); }

protected:
	/// The AMControlInfo that specifies where to move to
	AMControlInfo controlInfo_;
	/// A flag to indicate that this should be a relative (rather than absolute) move
	bool isRelative_;
	/// A flag to indicate that a relative move should be from the setpoint (rather than the feedback)
	bool isRelativeFromSetpoint_;

	/// A short helper function to update the action's description
	void updateDescriptionText();

};

#endif // AMCONTROLMOVEACTIONINFO3_H
