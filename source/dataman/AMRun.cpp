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


#include "AMRun.h"


#include <QList>


/// ======================= Implementation of AMFacility ================================
AMFacility::~AMFacility(){}

AMFacility::AMFacility(const QString& shortDescription, const QString& longDescription, const QString& iconFileName, QObject* parent)
	: AMDbObject(parent)
{
	setName(shortDescription); // ex: "SGM"
	description_ = longDescription; // ex: "Canadian Light Source SGM Beamline"
	iconFileName_ = iconFileName;
}

QString AMFacility::description() const
{
	return description_;
}

void AMFacility::setDescription(const QString& description)
{
	description_ = description;
	setModified(true);
}

QString AMFacility::iconFileName() const
{
	return iconFileName_;
}

void AMFacility::setIconFileName(const QString& fileName)
{
	iconFileName_ = fileName;
	setModified(true);
}

int AMFacility::thumbnailCount() const
{
	if(iconFileName().isEmpty())
		return 0;
	else
		return 1;
}

AMDbThumbnail AMFacility::thumbnail(int index) const
{
	Q_UNUSED(index)
	if(iconFileName().isEmpty())
		return AMDbThumbnail(name(), description());
	else
		return AMDbThumbnail(name(), description(), QImage(iconFileName()));
}

/// ======================= Implementation of AMRun ================================
AMRun::~AMRun(){}

AMRun::AMRun(QObject *parent) :
	AMDbObject(parent)
{
	dateTime_ = QDateTime::currentDateTime();
}

/// This constructor initializes a run with a given name
AMRun::AMRun(const QString& runName, QObject* parent)
	: AMDbObject(parent)
{
	dateTime_ = QDateTime::currentDateTime();
	setName(runName);
}

/// This constructor immediately loads a stored run from the database.
AMRun::AMRun(int databaseId, AMDatabase* database, QObject* parent)
: AMDbObject(parent)
{
	loadFromDb(database, databaseId);
}

/// This function finds out how many scans are in this run. (If this instance is not associated with any database yet, it returns 0)
int AMRun::scanCount() const {
	return scanIds().count();
}

/// Returns a list of the id's of all scans in this run
QList<int> AMRun::scanIds() const {
	if(database() == 0 || id() == 0)
		return QList<int>();
	return database()->objectsMatching("Objects", "runId", id());
}

/// Convenience function to set the notes/comments on this run
void AMRun::setNotes(const QString& notes) {
	notes_ = notes;
	setModified(true);
}

/// Set the start date of this run
void AMRun::setDateTime(const QDateTime& dateTime) {
	dateTime_ = dateTime;
	setModified(true);
}
