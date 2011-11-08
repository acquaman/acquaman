/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


AMRun::AMRun(QObject *parent) :
		AMDbObject(parent)
{
	/// Initialize our unique pieces of meta-data. The values don't matter much, but we want to make sure that they're present in the metaData_ hash.
	dateTime_ = QDateTime::currentDateTime();
	endDateTime_=QDateTime::currentDateTime();
	facilityId_= int();

	dateRangeUpdateScheduled_ = false;
	deleteWhenFinishedDateRangeUpdate_ = false;
}

/// This constructor initializes a run with a given name and facility Id.
AMRun::AMRun(const QString& runName, const int& facilityId, QObject* parent)
	: AMDbObject(parent)
{
	/// Initialize our unique pieces of meta-data. The values don't matter much, but we want to make sure that they're present in the metaData_ hash.
	dateTime_ = QDateTime::currentDateTime();
	endDateTime_=QDateTime::currentDateTime();

	setName(runName);
	setFacilityId(facilityId);

	dateRangeUpdateScheduled_ = false;
	deleteWhenFinishedDateRangeUpdate_ = false;
}

/// This constructor immediately loads a stored run from the database.
AMRun::AMRun(int databaseId, AMDatabase* database, QObject* parent)
	: AMDbObject(parent)
{

	loadFromDb(database, databaseId);

	dateRangeUpdateScheduled_ = false;
	deleteWhenFinishedDateRangeUpdate_ = false;
}



/// This function finds out how many scans are in this run.  (If this instance is not associated with any database yet, it returns 0)
int AMRun::scanCount() const {
	return scanIds().count();
}

/// Returns a list of the id's of all scans in this run
QList<int> AMRun::scanIds() const {

	if(database() == 0 || id() == 0)
		return QList<int>();

	return database()->objectsMatching("Objects", "runId", id());
}



#include <QTimer>
void AMRun::scheduleDateRangeUpdate(const QDateTime &alteredDateTime) {

	// if the altered dateTime is within the existing dateTime range of this run, nothing needs to happen; the limits won't be affected
	if(alteredDateTime.isValid() && alteredDateTime < endDateTime() && alteredDateTime > dateTime()) {
		return;
	}

	if(!dateRangeUpdateScheduled_) {
		dateRangeUpdateScheduled_ = true;
		// qDebug() << "AMRUN: Scheduling a date range update";
		QTimer::singleShot(0, this, SLOT(doDateRangeUpdate()));
	}
}

#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/AMScan.h"
void AMRun::doDateRangeUpdate() {

	dateRangeUpdateScheduled_ = false;

	if(database() == 0 || id() < 1)
		return;

	QSqlQuery q = database()->query();
	QString scanTableName = AMDbObjectSupport::s()->tableNameForClass<AMScan>();

	q.prepare(QString("SELECT MAX(dateTime) FROM %1 WHERE runId = ?").arg(scanTableName));
	q.bindValue(0, id());
	if(q.exec() && q.first())
		setEndDateTime(q.value(0).toDateTime());
	else
		setEndDateTime(QDateTime());

	q.prepare(QString("SELECT MIN(dateTime) FROM %1 WHERE runId = ?").arg(scanTableName));
	q.bindValue(0, id());
	if(q.exec() && q.first())
		setDateTime(q.value(0).toDateTime());
	else
		setDateTime(QDateTime::currentDateTime());

	storeToDb(database());

	// qDebug() << "AMRUN: finished a date range update" << dateTime() << endDateTime();

	// If this run is just hanging around for the purpose of finishing this update... Can delete it now.
	if(deleteWhenFinishedDateRangeUpdate_)
		deleteLater();
}
