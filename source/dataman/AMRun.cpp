#include "AMRun.h"


#include <QList>


AMRun::AMRun(QObject *parent) :
		AMDbObject(parent)
{
	/// Initialize our unique pieces of meta-data. The values don't matter much, but we want to make sure that they're present in the metaData_ hash.
	metaData_["dateTime"] = QDateTime::currentDateTime();
	metaData_["notes"] = QString();
	metaData_["endDateTime"]=QDateTime::currentDateTime();
	metaData_["facilityId"]= int();

	dateRangeUpdateScheduled_ = false;
}

/// This constructor initializes a run with a given name and facility Id.
AMRun::AMRun(const QString& runName, const int& facilityId, QObject* parent)
	: AMDbObject(parent)
{
	/// Initialize our unique pieces of meta-data. The values don't matter much, but we want to make sure that they're present in the metaData_ hash.
	metaData_["dateTime"] = QDateTime::currentDateTime();
	metaData_["notes"] = QString();
	metaData_["endDateTime"]=QDateTime::currentDateTime();
	metaData_["facilityId"]=int();
	this->setName(runName);
	this->setFacilityId(facilityId);

	dateRangeUpdateScheduled_ = false;
}

/// This constructor immediately loads a stored run from the database.
AMRun::AMRun(int databaseId, AMDatabase* database, QObject* parent)
	: AMDbObject(parent)
{
	/// Initialize our unique pieces of meta-data. The values don't matter much, but we want to make sure that they're present in the metaData_ hash.
	metaData_["dateTime"] = QDateTime::currentDateTime();
	metaData_["notes"] = QString();
	metaData_["endDateTime"]=QDateTime::currentDateTime();
	metaData_["facilityId"]=1;

	loadFromDb(database, databaseId);

	dateRangeUpdateScheduled_ = false;
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
	if(alteredDateTime.isValid() && alteredDateTime < endDateTime() && alteredDateTime > dateTime())
		return;

	if(!dateRangeUpdateScheduled_) {
		dateRangeUpdateScheduled_ = true;
		QTimer::singleShot(0, this, SLOT(doDateRangeUpdate()));
	}
}

void AMRun::doDateRangeUpdate() {

	dateRangeUpdateScheduled_ = false;

	if(database() == 0 || id() < 1)
		return;

	QSqlQuery q = database()->query();
	q.prepare(QString("SELECT MAX(dateTime) FROM %1 WHERE runId = ?").arg(AMDatabaseDefinition::objectTableName()));
	q.bindValue(0, id());
	if(q.exec() && q.first())
		setEndDateTime(q.value(0).toDateTime());
	else
		setEndDateTime(QDateTime());

	q.prepare(QString("SELECT MIN(dateTime) FROM %1 WHERE runId = ?").arg(AMDatabaseDefinition::objectTableName()));
	q.bindValue(0, id());
	if(q.exec() && q.first())
		setDateTime(q.value(0).toDateTime());
	else
		setDateTime(QDateTime::currentDateTime());
}
