#include "AMExperiment.h"

AMExperiment::AMExperiment(QObject *parent) :
		AMDbObject(parent)
{

}
/// initializing experiment with a given name.
AMExperiment::AMExperiment(const QString& experimentName, QObject* parent)
	: AMDbObject(parent)
{
	/// Initialize unique pieces of meta-data.

	metaData_["notes"] = QString();


	this->setName(experimentName);
}

/// This constructor immediately loads a stored experiment from the database.
AMExperiment::AMExperiment(int databaseId, AMDatabase* database, QObject* parent)
		: AMDbObject(parent)
{
		/// Initialize our unique pieces of meta-data. The values don't matter much, but we want to make sure that they're present in the metaData_ hash.

		metaData_["notes"] = QString();

		loadFromDb(database, databaseId);
}




///
