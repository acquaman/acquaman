#include "AMSample.h"

AMSample::AMSample(QObject *parent) :
	AMDbObject(parent)
{

}

/// This constructor initializes a sample with a given name.
AMSample::AMSample(const QString& sampleName, QObject* parent)
	: AMDbObject(parent)
{
	dateTime_ = QDateTime::currentDateTime();
	setName(sampleName);
}

/// This constructor immediately loads a stored sample from the database.
AMSample::AMSample(int databaseId, AMDatabase* database, QObject* parent)
	: AMDbObject(parent)
{

	loadFromDb(database, databaseId);
}

#include "dataman/AMDbObjectSupport.h"
void AMSample::destroySample(AMDatabase* db, int id) {
	if(db == 0)
		return;
	db->deleteRow(id, AMDbObjectSupport::tableNameForClass<AMSample>());
}


///
