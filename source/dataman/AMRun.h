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


#ifndef AMRUN_H
#define AMRUN_H


#include "dataman/database/AMDbObject.h"
#include <QImage>

#include <QBuffer>
#include <QByteArray>
#include <QPixmap>

/// AMRun's happen at AMFacility's... This is a lightweight class to store facility information in the database.
class AMFacility : public AMDbObject
{
	Q_OBJECT
	Q_PROPERTY(QString description READ description WRITE setDescription)
	Q_PROPERTY(QString iconFileName READ iconFileName WRITE setIconFileName)

	Q_CLASSINFO("AMDbObject_Attributes", "description=Facility")

public:
 	virtual ~AMFacility();
	Q_INVOKABLE explicit AMFacility(const QString& shortDescription = "[Other]", const QString& longDescription = "Unknown Facility", const QString& iconFileName = QString(), QObject* parent = 0);


	/// Long description of the facility (ex: "Canadian Light Source SGM Beamline")
	QString description() const;
	void setDescription(const QString& description);

	/// display icons for the facility
	QString iconFileName() const ;
	void setIconFileName(const QString& fileName) ;

	int thumbnailCount() const ;

	AMDbThumbnail thumbnail(int index) const;

protected:
	QString description_;
	QString iconFileName_;

};

/// This class represents instances of Experimental Runs in the database of user information. It provides a simple example of how to subclass AMDbObject to create your own C++ object that can be easily stored in the database.
class AMRun : public AMDbObject
{
	Q_OBJECT
	Q_PROPERTY(QString notes READ notes WRITE setNotes)
	Q_PROPERTY(QDateTime dateTime READ dateTime WRITE setDateTime)

	Q_CLASSINFO("AMDbObject_Attributes", "description=Experimental Run")
public:
	/// Default constructor. In it we initialize the extra fields we want to store inside metaData_.
 	virtual ~AMRun();
	Q_INVOKABLE explicit AMRun(QObject *parent = 0);

	/// This constructor initializes a run with a given name.
	AMRun(const QString& runName, QObject* parent = 0);

	/// This constructor immediately loads a stored run from the database.
	AMRun(int databaseId, AMDatabase* database, QObject* parent = 0);


	// Convenient access methods for our meta-data:
	/////////////////////////

	/// This returns the date/time that this run was started.
	QDateTime dateTime() const { return dateTime_; }

	/// This returns a string of notes/comments about this run.
	QString notes() const { return notes_; }

	/// This function finds out how many scans are in this run.  (If this instance is not associated with any database yet, it returns 0)
	int scanCount() const;

	/// Returns a list of the id's of all scans in this run
	QList<int> scanIds() const;


	// Thumbnail System
	/////////////////////////////////

	/// Runs don't have thumbnails...
	int thumbnailCount() const { return 0;}


public slots:

	/// Convenience function to set the notes/comments on this run
	void setNotes(const QString& notes) ;

protected slots:

	/// Set the start date of this run
	void setDateTime(const QDateTime& dateTime);

protected:

	QString notes_;
	QDateTime dateTime_;

};



#endif // AMRUN_H
