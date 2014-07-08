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


#ifndef AMSAMPLEPLATEPRE2013_H
#define AMSAMPLEPLATEPRE2013_H

#include <QList>
#include <QDateTime>

#include "AMSamplePre2013.h"
#include "dataman/info/AMControlInfoList.h"
#include "dataman/database/AMDbObject.h"
#include "acquaman.h"

class AMConstDbObject;

/// This is a small helper class for AMSamplePlate, which stores a set of samples and their associated positions on the sample plate/manipulator
class AMSamplePositionPre2013 : public AMDbObject {
	Q_OBJECT

//	Q_PROPERTY(int sampleId READ sampleId WRITE setSampleId)
	Q_PROPERTY(AMConstDbObject* sample READ dbReadSample WRITE dbWriteSample)
	Q_PROPERTY(int facilityId READ facilityId WRITE setFacilityId)
	Q_PROPERTY(AMDbObject* position READ dbGetPosition WRITE dbLoadPosition)

	Q_PROPERTY(AMDbObject* topLeftPosition READ dbGetTopLeftPosition WRITE dbLoadTopLeftPosition)
	Q_PROPERTY(AMDbObject* bottomRightPosition READ dbGetBottomRightPosition WRITE dbLoadBottomRightPosition)

	Q_CLASSINFO("AMDbObject_Attributes", "description=Spacial Coordinates of Sample")

public:
	/// Constructor: can specify initial values for sampleId, position, and facilityId
	Q_INVOKABLE AMSamplePositionPre2013(int sampleId = 0, const AMControlInfoList& position = AMControlInfoList(), int facilityId = 0 );
	//Q_INVOKABLE AMSamplePosition(int sampleId = 0, const AMControlInfoList& position = AMControlInfoList(), int facilityId = 0 ) :
	//	sampleId_(sampleId), position_(position), facilityId_(facilityId), topLeftPosition_(AMControlInfoList()), bottomRightPosition_(AMControlInfoList()) {}

	/// Returns the database id of the sample associated with this marked position. This corresponds to the samples in the database's AMSamplePre2013 table.
	//int sampleId() const { return sampleId_; }
	int sampleId() const;
	const AMSamplePre2013* sample() const;
	/// Returns the facilityId that this sample position is relevant for.  (The motor positions from the SGM sample manipulator won't make sense to the REIXS positioner, etc.)  This corresponds to the facilities in the database's AMFacility table.
	int facilityId() const { return facilityId_; }
	/// Returns a non-modifiable reference to the sample position (stored as an AMControlInfoList). If you need a by-value copy of the position, just assign this to a local AMControlInfoList.
	const AMControlInfoList& position() const { return position_; }

	const AMControlInfoList& topLeftPosition() const { return topLeftPosition_;}
	const AMControlInfoList& bottomRightPosition() const { return bottomRightPosition_;}
	bool positionWithinBounds() const;
	bool overlaps(const AMSamplePositionPre2013 &other) const;
	bool matchesPosition(const AMControlInfoList &other) const;
	bool matchesPositionWithinTolerances(const AMControlInfoList &other, const QList<double> tolerances) const;
	double rms3SpaceDistance(const AMControlInfoList &other) const;


	/// Set the database id of the stored sample at this position. This corresponds to the id of a sample in the database's AMSamplePre2013 table.
	//void setSampleId(int id) { sampleId_ = id; setModified(true); }
	void setSampleId(int newSampleId);
	void setSample(const AMSamplePre2013 *sample);
	/// Set the database id of the facility (ie: beamline) to which this position is relevant. This corresponds to the id of a facilty in the database's AMFacility table.
	void setFacilityId(int id) { facilityId_ = id; setModified(true); }
	/// Set the position of this sample on the plate, as an AMControlInfoList.
	void setPosition(const AMControlInfoList& newPosition) { position_.setValuesFrom(newPosition); setModified(true); }

	void setTopLeftPosition(const AMControlInfoList& newPosition) { topLeftPosition_.setValuesFrom(newPosition); setModified(true);}
	void setBottomRightPosition(const AMControlInfoList& newPosition) { bottomRightPosition_.setValuesFrom(newPosition); setModified(true);}


	/// Copy all the values from \c other (including sampleId(), facilityId(), and position()), but retain our old database identity.  (Unlike the default assignment operator and copy constructor, our database() and id() will remain the same after calling this function.)
	void setValuesFrom(const AMSamplePositionPre2013& other) {
		//sampleId_ = other.sampleId_;
		setSample(other.sample());
		facilityId_ = other.facilityId_;
		position_.setValuesFrom(other.position_);

		topLeftPosition_.setValuesFrom(other.topLeftPosition());
		bottomRightPosition_.setValuesFrom(other.bottomRightPosition());

		setModified(true);
	}

protected:
	/// The database id of the stored sample at this position
	//int sampleId_;
	AMConstDbObject *sample_;
	/// The position (ie: of the sample manipulator, whatever you want) that this sample is found at, expressed as an AMControlInfoList.
	AMControlInfoList position_;
	/// The facility (ie: beamline) to which this position is relevant. The motor positions from the SGM sample manipulator won't make sense to the REIXS sample manipulator, etc.
	int facilityId_;

	AMControlInfoList topLeftPosition_, bottomRightPosition_;

	/// Store the position to the database
	AMDbObject* dbGetPosition() { return &position_; }
	/// Load the position from the database: Never called, because dbGetPosition() is always valid.
	void dbLoadPosition(AMDbObject*) {} // never called

	AMDbObject* dbGetTopLeftPosition() { return &topLeftPosition_;}
	void dbLoadTopLeftPosition(AMDbObject*) {}
	AMDbObject* dbGetBottomRightPosition() { return &bottomRightPosition_;}
	void dbLoadBottomRightPosition(AMDbObject*) {}

	AMConstDbObject* dbReadSample() const;
	void dbWriteSample(AMConstDbObject *newSample);
};



/// An AMSamplePlate is a database object that can be associated with users and their scans.  A sample plate contains one or (most often) more samples. We also remember the position for each sample (ie: from a set of positioning motors, etc.) necessary to reposition the sample in the same spot later.
class AMSamplePlatePre2013 : public AMDbObject, public AMOrderedList<AMSamplePositionPre2013>
{
Q_OBJECT
	Q_PROPERTY(QDateTime dateTime READ dateTime WRITE dbLoadDateTime)
	Q_PROPERTY(AMDbObjectList positions READ dbGetPositions WRITE dbLoadPositions)

	Q_CLASSINFO("AMDbObject_Attributes", "description=List of Samples and their Positions on a Sample Plate")
	Q_CLASSINFO("createTime", "createIndex=true")

public:
	/// Default constructor: an empty sample plate with no samples.
	Q_INVOKABLE explicit AMSamplePlatePre2013(QObject *parent = 0);
	/// Copy constructor
	AMSamplePlatePre2013(const AMSamplePlatePre2013& other);


	/// Re-implemented from AMDbObject::loadFromDb().  If the number of sample positions stays the same, the new positions will be re-loaded into the existing sample position objects, and no sample positions will be added or removed. We'll need to signal that the sample plate data changed.
	bool loadFromDb(AMDatabase *db, int id);

	// Meta-data access
	////////////////////////

	/// When this sample plate was first created
	QDateTime dateTime() const { return dateTime_; }

	/// Returns the sample id at the given position. If no sample is at the given position, -1 is returned. Can optionally take a list of alternate tolerance (rather than the motion tolerances) for the controls.
	int sampleIdAtPosition(const AMControlInfoList &position, const QList<double> tolerances = QList<double>()) const;

	bool sampleAtIndexOverlaps(int index) const;

signals:
	// The following signals are forwarded from our signalSource().
	void samplePositionChanged(int index);
	void samplePositionAdded(int index);
	void samplePositionRemoved(int index);
	void samplePositionAboutToBeAdded(int index);
	void samplePositionAboutToBeRemoved(int index);

public slots:

protected slots:


protected:

	/// Time when this sample plate was first created
	QDateTime dateTime_;

	/// Set the createTime(); used when restoring a sample plate from the database.
	void dbLoadDateTime(const QDateTime& createTime) { dateTime_ = createTime; }

	/// Export the current positions to the database
	AMDbObjectList dbGetPositions();
	/// Load the positions for an existing sample plate from the database
	void dbLoadPositions(const AMDbObjectList& newPositions);


};


#endif // AMSAMPLEPLATEPRE2013_H
