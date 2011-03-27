/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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


#ifndef AMSAMPLEPLATE_H
#define AMSAMPLEPLATE_H

#include <QList>
#include <QDateTime>

#include "AMSample.h"
#include "AMControlInfoList.h"
#include "AMDbObject.h"
#include "acquaman.h"

/// This is a small helper class for AMSamplePlate, which stores a set of samples and their associated positions on the sample plate/manipulator
class AMSamplePosition : public AMDbObject {
	Q_OBJECT

	Q_PROPERTY(int sampleId READ sampleId WRITE setSampleId)
	Q_PROPERTY(int facilityId READ facilityId WRITE setFacilityId)
	Q_PROPERTY(AMDbObject* position READ dbGetPosition WRITE dbLoadPosition)

public:
	/// Constructor: can specify initial values for sampleId, position, and facilityId
	Q_INVOKABLE AMSamplePosition(int sampleId = 0, const AMControlInfoList& position = AMControlInfoList(), int facilityId = 0 ) :
		sampleId_(sampleId), position_(position), facilityId_(facilityId) {}

	/// Returns the database id of the sample associated with this marked position. This corresponds to the samples in the database's AMSample table.
	int sampleId() const { return sampleId_; }
	/// Returns the facilityId that this sample position is relevant for.  (The motor positions from the SGM sample manipulator won't make sense to the REIXS positioner, etc.)  This corresponds to the facilities in the database's AMFacility table.
	int facilityId() const { return facilityId_; }
	/// Returns a non-modifiable reference to the sample position (stored as an AMControlInfoList). If you need a by-value copy of the position, just assign this to a local AMControlInfoList.
	const AMControlInfoList& position() const { return position_; }


	/// Set the database id of the stored sample at this position. This corresponds to the id of a sample in the database's AMSample table.
	void setSampleId(int id) { sampleId_ = id; setModified(true); }
	/// Set the database id of the facility (ie: beamline) to which this position is relevant. This corresponds to the id of a facilty in the database's AMFacility table.
	void setFacilityId(int id) { facilityId_ = id; setModified(true); }
	/// Set the position of this sample on the plate, as an AMControlInfoList.
	void setPosition(const AMControlInfoList& newPosition) { position_ = newPosition; setModified(true); }

protected:
	/// The database id of the stored sample at this position
	int sampleId_;
	/// The position (ie: of the sample manipulator, whatever you want) that this sample is found at, expressed as an AMControlInfoList.
	AMControlInfoList position_;
	/// The facility (ie: beamline) to which this position is relevant. The motor positions from the SGM sample manipulator won't make sense to the REIXS sample manipulator, etc.
	int facilityId_;

	/// Store the position to the database
	AMDbObject* dbGetPosition() { return &position_; }
	/// Load the position from the database: Never called, because dbGetPosition() is always valid.
	void dbLoadPosition(AMDbObject*) {} // never called
};



/// An AMSamplePlate is a database object that can be associated with users and their scans.  A sample plate contains one or (most often) more samples. We also remember the position for each sample (ie: from a set of positioning motors, etc.) necessary to reposition the sample in the same spot later.
class AMSamplePlate : public AMDbObject, public AMOrderedList<AMSamplePosition>
{
Q_OBJECT
	Q_PROPERTY(QDateTime dateTime READ dateTime WRITE dbLoadDateTime)
	Q_PROPERTY(AMDbObjectList positions READ dbGetPositions WRITE dbLoadPositions)

	Q_CLASSINFO("AMDbObject_Attributes", "description=List of Samples and their Positions on a Sample Plate")
	Q_CLASSINFO("createTime", "createIndex=true")

public:
	/// Default constructor: an empty sample plate with no samples.
	Q_INVOKABLE explicit AMSamplePlate(QObject *parent = 0);
	/// Copy constructor
	AMSamplePlate(const AMSamplePlate& other);


	// Meta-data access
	////////////////////////

	/// When this sample plate was first created
	QDateTime dateTime() const { return dateTime_; }



// Required?
//	/// Sample plates are valid after being loaded or saved. You can also manually mark a sample plate as invalid by calling setInvalid(). (For example, at the beginning of a sample transfer.)
//	bool valid() const { return valid_; }
//	/// Mark a sample plate as 'invalid' (ie: non-existent, or not trustworthy. It won't become valid until successfully saved to the db, or loaded from the db.)
//	void setInvalid() { valid_ = false; }
//	/// This is a convenience function that tells a sample plate object to "become another sample plate" that already exists in the user's database.  It's equivalent to "loadFromDb(AMDatabase::userdb(), newSamplePlateId);"
//	void changeSamplePlate(int newSamplePlateId) { loadFromDb(AMDatabase::userdb(), newSamplePlateId); }




signals:
	// The following signals are forwarded from our signalSource().
	void samplePositionChanged(int index);
	void samplePositionAdded(int index);
	void samplePositionRemoved(int index);
	void samplePositionAboutToBeAdded(int index);
	void samplePositionAboutToBeRemoved(int index);

//	/// This signal is emitted when the sample plate is "changed out", or re-loaded. It may now contain completely different samples, at completely different positions. \todo Re-implement if required?
//	void samplePlateChanged(bool isValid);

public slots:

protected slots:


protected:

//	/// Sample plates are valid when they've been successfully stored to or loaded from the Db.
// necessary? bool valid_;

	/// Time when this sample plate was first created
	QDateTime dateTime_;

	/// Set the createTime(); used when restoring a sample plate from the database.
	void dbLoadDateTime(const QDateTime& createTime) { dateTime_ = createTime; }

	/// Export the current positions to the database
	AMDbObjectList dbGetPositions();
	/// Load the positions for an existing sample plate from the database
	void dbLoadPositions(const AMDbObjectList& newPositions);


};


#endif // AMSAMPLEPLATE_H
