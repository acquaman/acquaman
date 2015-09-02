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


#ifndef AMCONTROLWAITACTIONINFO_H
#define AMCONTROLWAITACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "dataman/info/AMControlInfo.h"

/// This AMActionInfo-subclass specifies the information for AMControlWaitAction -- an action that waits for a control to reach a value.  This info specifies the value we are waiting for as well as how we want to match (equal, greater than, less than), and the timeout limit.
class AMControlWaitActionInfo : public AMActionInfo3
{
	Q_OBJECT
	Q_PROPERTY(AMDbObject* controlInfo READ dbReadControlInfo WRITE dbLoadControlInfo)
	Q_PROPERTY(double timeout READ timeout WRITE setTimeout)
	Q_PROPERTY(int matchType READ matchTypeAsInt WRITE setMatchTypeAsInt)

public:
	enum MatchType {
		MatchEqual = 0,         // matches when the value is the same
		MatchGreaterThan = 1,           // matches when the current value of the control is greater than the setpoint.
		MatchLessThan = 2,           // matches when the current value of the control is less than the setpoint.
		MatchWithinTolerance = 3,	// matches when the current values lies within tolerance of the setpoint.
		MatchInvalid
	};

	/// Constructor. You should always specify a valid \c setpoint, but we provide the default argument because we need a default constructor for database loading.
	Q_INVOKABLE AMControlWaitActionInfo(const AMControlInfo &setpoint = AMControlInfo(), double timeout = -1, AMControlWaitActionInfo::MatchType matchType = AMControlWaitActionInfo::MatchInvalid, QObject *parent = 0);
	virtual ~AMControlWaitActionInfo();

	/// Copy Constructor
	AMControlWaitActionInfo(const AMControlWaitActionInfo &other);

	/// This function is used as a virtual copy constructor
	virtual AMActionInfo3* createCopy() const;

	// Re-implemented public functions
	/////////////////////////////////

	/// This should describe the type of the action
	virtual QString typeDescription() const { return "Control Wait"; }

	// New public functions
	//////////////////////////

	/// Returns a pointer to our move destination setpoint
	const AMControlInfo& controlInfo() const { return controlInfo_; }

	/// Returns our timeout.
	double timeout() const;

	/// Return our matchtype.
	AMControlWaitActionInfo::MatchType matchType() const;

	/// Set the move destination setpoint, including the control name, value, and description.
	/*! \note We make a copy of \c controlInfo's values, and do not retain any reference to it afterward. */
	void setControlInfo(const AMControlInfo& controlInfo);
	/// Set the move destination setpoint (value only).
	void setSetpoint(double setpoint);

	void setTimeout(double newTimeout);
	void setMatchType(AMControlWaitActionInfo::MatchType newMatchType);

public slots:

signals:

protected:
	// Database loading/storing
	////////////////////////////

	/// For database storing only.
	AMControlInfo* dbReadControlInfo() { return &controlInfo_; }
	/// For database loading only. This function will never be called since dbReadControlInfo() always returns a valid setpoint, but it needs to be here.
	void dbLoadControlInfo(AMDbObject* newLoadedObject) { newLoadedObject->deleteLater(); }

	int matchTypeAsInt() const;
	void setMatchTypeAsInt(int newMatchType);

protected:
	/// The AMControlInfo that specifies where to move to
	AMControlInfo controlInfo_;

	/// The max time to wait for the control to reach a setpoint. (seconds)
	double timeout_;

	/// The desired match type.
	AMControlWaitActionInfo::MatchType matchType_;

	/// A short helper function to update the action's description
	void updateDescriptionText();

};

#endif // AMCONTROLWAITACTIONINFO_H
