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


#ifndef AMCONTROLSTOPACTIONINFO_H
#define AMCONTROLSTOPACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "dataman/info/AMControlInfo.h"

/// This action info specifies the very limited amount of information needed to stop a control.
class AMControlStopActionInfo : public AMActionInfo3
{
	Q_OBJECT

	Q_PROPERTY(AMDbObject* controlInfo READ dbReadControlInfo WRITE dbLoadControlInfo)

public:
	/// Constructor.  Builds the action info required to stop a control.
	virtual ~AMControlStopActionInfo();
	Q_INVOKABLE AMControlStopActionInfo(const AMControlInfo &setpoint = AMControlInfo(), QObject *parent = 0);
	/// Copy contructor.
	AMControlStopActionInfo(const AMControlStopActionInfo &other);

	/// This function is used as a virtual copy constructor.
	virtual AMActionInfo3 *createCopy() const;
	/// Short description of the type of action this is.
	virtual QString typeDescription() const { return "Control Stop"; }

	/// Returns a pointer to our move destination setpoint
	const AMControlInfo* controlInfo() const { return &controlInfo_; }
	/// Set the move destination setpoint, including the control name, value, and description.
	/*! \note We make a copy of \c controlInfo's values, and do not retain any reference to it afterward. */
	void setControlInfo(const AMControlInfo& controlInfo);

	// Database loading/storing
	////////////////////////////

	/// For database storing only.
	AMControlInfo* dbReadControlInfo() { return &controlInfo_; }
	/// For database loading only. This function will never be called since dbReadControlInfo() always returns a valid setpoint, but it needs to be here.
	void dbLoadControlInfo(AMDbObject* newLoadedObject) { newLoadedObject->deleteLater(); }

protected:
	/// The AMControlInfo that specifies where to move to
	AMControlInfo controlInfo_;
};

#endif // AMCONTROLSTOPACTIONINFO_H
