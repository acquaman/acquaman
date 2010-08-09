#include "AMRun.h"


#include <QList>


AMRun::AMRun(QObject *parent) :
	AMDbObject(parent)
{
	/// Initialize our unique pieces of meta-data. The values don't matter much, but we want to make sure that they're present in the metaData_ hash.
	metaData_["dateTime"] = QDateTime::currentDateTime();
	metaData_["notes"] = QString();
	metaData_["endDateTime"]=QDateTime();
	metaData_["facilityId"]= int();
}

/// This constructor initializes a run with a given name and facility Id.
AMRun::AMRun(const QString& runName, const int& facilityId, QObject* parent)
		: AMDbObject(parent)
{
		/// Initialize our unique pieces of meta-data. The values don't matter much, but we want to make sure that they're present in the metaData_ hash.
		metaData_["dateTime"] = QDateTime::currentDateTime();
		metaData_["notes"] = QString();
		metaData_["endDateTime"]=QDateTime();
		metaData_["facilityId"]=int();
		this->setName(runName);
		this->setFacilityId(facilityId);
}

/// This constructor immediately loads a stored run from the database.
AMRun::AMRun(int databaseId, AMDatabase* database, QObject* parent)
		: AMDbObject(parent)
{
		/// Initialize our unique pieces of meta-data. The values don't matter much, but we want to make sure that they're present in the metaData_ hash.
		metaData_["dateTime"] = QDateTime::currentDateTime();
		metaData_["notes"] = QString();
		metaData_["endDateTime"]=QDateTime();
		metaData_["facilityId"]=1;

		loadFromDb(database, databaseId);
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


