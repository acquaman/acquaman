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


#ifndef AMDETECTORDWELLTIMEACTIONINFO_H
#define AMDETECTORDWELLTIMEACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "dataman/info/AMDetectorInfo.h"

class AMDetectorDwellTimeActionInfo : public AMActionInfo3
{
Q_OBJECT
	Q_PROPERTY(AMDbObject* detectorInfo READ dbReadDetectorInfo WRITE dbLoadDetectorInfo)
	Q_PROPERTY(double dwellSeconds READ dwellSeconds WRITE setDwellSeconds)
public:
	/// Constructor
	virtual ~AMDetectorDwellTimeActionInfo();
	Q_INVOKABLE AMDetectorDwellTimeActionInfo(const AMDetectorInfo &detectorInfo = AMDetectorInfo(), double dwellSeconds = 1,  QObject *parent = 0);

	/// Copy Constructor
	AMDetectorDwellTimeActionInfo(const AMDetectorDwellTimeActionInfo &other);

	/// This function is used as a virtual copy constructor
	virtual AMActionInfo3* createCopy() const;

	/// This should describe the type of the action
	virtual QString typeDescription() const { return "Detector Dwell Time"; }

	/// Returns a pointer to our detectorInfo
	const AMDetectorInfo* detectorInfo() const { return &detectorInfo_; }

	/// Returns the requested dwell time
	double dwellSeconds() const { return dwellSeconds_; }


protected:
	/// For database storing only
	AMDetectorInfo* dbReadDetectorInfo() { return &detectorInfo_; }
	/// For database loading only. This function will never be called since dbReadDetectorInfo() always returns a valid pointer
	void dbLoadDetectorInfo(AMDbObject *newLoadedObject) { newLoadedObject->deleteLater(); }

	/// Sets the dwell seconds to the provided value. Used in database loading.
	void setDwellSeconds(double dwellSeconds);

	/// The AMDetectorInfo that specifies which detector to acquire
	AMDetectorInfo detectorInfo_;

	/// The requested dwell time
	double dwellSeconds_;
};

#endif // AMDETECTORDWELLTIMEACTIONINFO_H
