#include "AMSample.h"

AMSample::AMSample(QObject *parent) :
	AMDbObject(parent)
{

}

/// This constructor initializes a sample with a given name.
AMSample::AMSample(const QString& sampleName, QObject* parent)
	: AMDbObject(parent)
{
	/// Initialize our unique pieces of meta-data. The values don't matter much, but we want to make sure that they're present in the metaData_ hash.
	metaData_["dateTime"] = QDateTime::currentDateTime();
	metaData_["notes"] = QString();
	metaData_["image"] = QVariant();

	setName(sampleName);
}

/// This constructor immediately loads a stored sample from the database.
AMSample::AMSample(int databaseId, AMDatabase* database, QObject* parent)
	: AMDbObject(parent)
{
	/// Initialize our unique pieces of meta-data. The values don't matter much, but we want to make sure that they're present in the metaData_ hash.
	metaData_["dateTime"] = QDateTime::currentDateTime();
	metaData_["notes"] = QString();
	metaData_["image"] = QVariant();

	loadFromDb(database, databaseId);
}


///
