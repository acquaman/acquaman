#include "AMRun.h"

AMRun::AMRun(QObject *parent) :
    AMDbObject(parent)
{

}

/// This constructor initializes a run with a given name.
AMRun::AMRun(const QString& runName, QObject* parent)
        : AMDbObject(parent)
{
        /// Initialize our unique pieces of meta-data. The values don't matter much, but we want to make sure that they're present in the metaData_ hash.
        metaData_["dateTime"] = QDateTime::currentDateTime();
        metaData_["notes"] = QString();
        metaData_["image"] = QVariant();

        this->setName(runName);
}

/// This constructor immediately loads a stored run from the database.
AMRun::AMRun(int databaseId, AMDatabase* database, QObject* parent)
        : AMDbObject(parent)
{
        /// Initialize our unique pieces of meta-data. The values don't matter much, but we want to make sure that they're present in the metaData_ hash.
        metaData_["dateTime"] = QDateTime::currentDateTime();
        metaData_["notes"] = QString();
        metaData_["image"] = QVariant();

        loadFromDb(database, databaseId);
}


///

