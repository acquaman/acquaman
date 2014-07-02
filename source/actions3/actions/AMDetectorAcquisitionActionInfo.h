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


#ifndef AMDETECTORACQUISITIONACTIONINFO_H
#define AMDETECTORACQUISITIONACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "dataman/info/AMDetectorInfo.h"

class AMDetectorAcquisitionActionInfo : public AMActionInfo3
{
Q_OBJECT
public:
	/// Constructor
 	virtual ~AMDetectorAcquisitionActionInfo();
	Q_INVOKABLE AMDetectorAcquisitionActionInfo(const AMDetectorInfo &detectorInfo = AMDetectorInfo(), AMDetectorDefinitions::ReadMode readMode = AMDetectorDefinitions::SingleRead,  QObject *parent = 0);

	/// Copy Constructor
	AMDetectorAcquisitionActionInfo(const AMDetectorAcquisitionActionInfo &other);

	/// This function is used as a virtual copy constructor
	virtual AMDetectorAcquisitionActionInfo* createCopy() const { return new AMDetectorAcquisitionActionInfo(*this); }

	/// This should describe the type of the action
	virtual QString typeDescription() const { return "Detector Acquisition"; }

	/// Returns a pointer to our detectorInfo
	const AMDetectorInfo* detectorInfo() const { return &detectorInfo_; }

	/// Returns the read mode for this acquisition
	AMDetectorDefinitions::ReadMode readMode() const { return readMode_; }

	/// For database storing only
	AMDetectorInfo* dbReadDetectorInfo() { return &detectorInfo_; }
	/// For database loading only. This function will never be called since dbReadDetectorInfo() always returns a valid pointer
	void dbLoadDetectorInfo(AMDbObject *newLoadedObject) { delete newLoadedObject; }

protected:
	/// The AMDetectorInfo that specifies which detector to acquire
	AMDetectorInfo detectorInfo_;

	/// The read mode we want to acquire with
	AMDetectorDefinitions::ReadMode readMode_;
};

#endif // AMDETECTORACQUISITIONACTIONINFO_H
