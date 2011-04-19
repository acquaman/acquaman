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


#ifndef AMRUN_H
#define AMRUN_H


#include "dataman/AMDbObject.h"
#include <QImage>

#include <QBuffer>
#include <QByteArray>
#include <QPixmap>

/// This class represents instances of objects that map to Runs in the database of user information. It provides a simple example of how to subclass AMDbObject to create your own C++ object that can be easily stored in the database, and exploit the general meta-data management system.

/// AMRun's happen at AMFacility's... This is a lightweight class to store facility information in the database.
class AMFacility : public AMDbObject
{
	Q_OBJECT
	Q_PROPERTY(QString description READ description WRITE setDescription)
	Q_PROPERTY(QString iconFileName READ iconFileName WRITE setIconFileName)

	Q_CLASSINFO("AMDbObject_Attributes", "description=Facility")

public:
	explicit AMFacility(const QString& shortDescription, const QString& longDescription, const QString& iconFileName = QString(), QObject* parent = 0)
		: AMDbObject(parent) {
		setName(shortDescription);	// ex: "SGM"
		description_ = longDescription;	// ex: "Canadian Light Source SGM Beamline"
		iconFileName_ = iconFileName;
	}


	/// Long description of the facility (ex: "Canadian Light Source SGM Beamline")
	QString description() const {
		return description_;
	}

	void setDescription(const QString& description) {
		description_ = description;
		setModified(true);
	}

	QString iconFileName() const {
		return iconFileName_;
	}

	int thumbnailCount() const {
		if(iconFileName().isEmpty())
			return 0;
		else
			return 1;
	}

	AMDbThumbnail thumbnail(int index) const {
		Q_UNUSED(index)

		if(iconFileName().isEmpty())
			return AMDbThumbnail(name(), description());
		else
			return AMDbThumbnail(name(), description(), QPixmap(iconFileName()));
	}





protected:

	void setIconFileName(const QString& fileName) { iconFileName_ = fileName; setModified(true); }

	QString description_;
	QString iconFileName_;

};

class AMRun : public AMDbObject
{
	Q_OBJECT
	Q_PROPERTY(QString notes READ notes WRITE setNotes)
	Q_PROPERTY(QDateTime dateTime READ dateTime WRITE setDateTime)
	Q_PROPERTY(QDateTime endDateTime READ endDateTime WRITE setEndDateTime)
	Q_PROPERTY(int facilityId READ facilityId WRITE setFacilityId)

	Q_CLASSINFO("AMDbObject_Attributes", "description=Data Run")
public:
	/// Default constructor. In it we initialize the extra fields we want to store inside metaData_.
	explicit AMRun(QObject *parent = 0);

	/// This constructor initializes a run with a given name and facility id.
	AMRun(const QString& runName, const int& facilityId=1, QObject* parent = 0);

	/// This constructor immediately loads a stored run from the database.
	AMRun(int databaseId, AMDatabase* database, QObject* parent = 0);


	// Convenient access methods for our meta-data:
	/////////////////////////

	/// Although we can access everything through metaData() and setMetaData(), we can also write convenience functions for direct acces. This returns the date/time of the earliest scan in this run.
	QDateTime dateTime() const {
		return dateTime_;
	}

	/// This returns a string of notes/comments about this run.
	QString notes() const {
		return notes_;
	}

	/// This returns a string regarding the location of the run.
	int facilityId() const{
		return facilityId_;
	}

	/// This returns the date/time of the last scan in this run.
	QDateTime endDateTime() const{
		return endDateTime_;
	}

	/// This function finds out how many scans are in this run.  (If this instance is not associated with any database yet, it returns 0)
	int scanCount() const;

	/// Returns a list of the id's of all scans in this run
	QList<int> scanIds() const;


	// Thumbnail System
	/////////////////////////////////

	/// Runs don't have thumbnails...
	int thumbnailCount() const {
		return 0;
	}



	/// This class-wide function is the most efficient way to notify a run that it needs to update its date range.
	static void scheduleDateRangeUpdate(int runid, AMDatabase* db, const QDateTime& alteredDateTime = QDateTime()) {

		AMRun* updateRun = new AMRun(runid, db);
		updateRun->deleteWhenFinishedDateRangeUpdate_ = true;

		updateRun->scheduleDateRangeUpdate(alteredDateTime);
		if(!updateRun->dateRangeUpdateScheduled_)	// if the date range update wasn't necessary, we can delete now...
			delete updateRun;
		// otherwise this run will hang around until the update is completed

	}


signals:

public slots:

	/// Convenience function to set the notes/comments on this run
	void setNotes(const QString& notes) {
		notes_ = notes;
		setModified(true);
	}

	/// Convenience function to set the location for this run
	void setFacilityId(int facilityId){
		facilityId_ = facilityId;
		setModified(true);
	}

	/// Call this to schedule an update of the start and end dates, based on looking at the scans in this run. For optimization, you can provide the \c alteredDateTime of the scan that was just added or removed from this run, or changed within a scan that was part of this run. (If this \c alteredDateTime is in the middle of this run's dateTime range, nothing needs to happen.)
	/*! The update search happens when control returns to the event loop, so multiple calls to scheduleDateRangeUpdate() will only result in one search. */
	void scheduleDateRangeUpdate(const QDateTime& alteredDateTime = QDateTime());

protected slots:
	/// Convenience function to set the end date for this run. This is only called internally, because the dateTime and endDateTime are determined automatically from the scans inside this run.  We call setMetaData(), since this takes care of emitting the metaDataChanged() signal for us.
	void setEndDateTime(const QDateTime& endDateTime) {
		endDateTime_ = endDateTime;
		setModified(true);
	}

	/// Convenience function to set the creation date/time associated with this run.   This is only called internally, because the dateTime and endDateTime are determined automatically from the scans inside this run.  We call setMetaData(), since this takes care of emitting the metaDataChanged() signal for us.
	void setDateTime(const QDateTime& dateTime) {
		dateTime_ = dateTime;
		setModified(true);
	}

	/// Conduct a search through all scans in this run, and determine the dateTime range of this run
	void doDateRangeUpdate();



protected:
	bool dateRangeUpdateScheduled_;
	bool deleteWhenFinishedDateRangeUpdate_;

	QString notes_;
	int facilityId_;
	QDateTime dateTime_;
	QDateTime endDateTime_;

};



#endif // AMRUN_H
