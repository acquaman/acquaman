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


#ifndef AMDETECTORTRIGGERACTIONINFO_H
#define AMDETECTORTRIGGERACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "dataman/info/AMDetectorInfo.h"

class AMDetectorTriggerActionInfo : public AMActionInfo3
{
Q_OBJECT
	Q_PROPERTY(AMDbObject* detectorInfo READ dbReadDetectorInfo WRITE dbLoadDetectorInfo)
	Q_PROPERTY(int readMode READ dbReadReadMode WRITE dbLoadReadMode)
public:
	/// Constructor
	virtual ~AMDetectorTriggerActionInfo();
	Q_INVOKABLE AMDetectorTriggerActionInfo(const AMDetectorInfo &detectorInfo = AMDetectorInfo(), AMDetectorDefinitions::ReadMode readMode = AMDetectorDefinitions::SingleRead,  QObject *parent = 0);

	/// Copy Constructor
	AMDetectorTriggerActionInfo(const AMDetectorTriggerActionInfo &other);

	/// This function is used as a virtual copy constructor
	virtual AMActionInfo3* createCopy() const;

	/// This should describe the type of the action
	virtual QString typeDescription() const { return "Detector Trigger"; }

	/// Returns a pointer to our detectorInfo
	const AMDetectorInfo* detectorInfo() const { return &detectorInfo_; }

	/// Returns the read mode for this acquisition
	AMDetectorDefinitions::ReadMode readMode() const { return readMode_; }

protected:

	/// For database storing only
	AMDetectorInfo* dbReadDetectorInfo() { return &detectorInfo_; }
	/// For database loading only. This function will never be called since dbReadDetectorInfo() always returns a valid pointer
	void dbLoadDetectorInfo(AMDbObject *newLoadedObject) { newLoadedObject->deleteLater(); }

	/// For storing the read mode to the database.
	int dbReadReadMode() { return int(readMode_); }

	/// For loading the read mode from the database.
	void dbLoadReadMode(int readMode) { readMode_ = AMDetectorDefinitions::ReadMode(readMode); }

	/// The AMDetectorInfo that specifies which detector to acquire
	AMDetectorInfo detectorInfo_;

	/// The read mode we want to acquire with
	AMDetectorDefinitions::ReadMode readMode_;
};

#endif // AMDETECTORTRIGGERACTIONINFO_H
