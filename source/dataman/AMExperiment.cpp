#include "AMExperiment.h"

AMExperiment::AMExperiment(QObject *parent) :
		AMDbObject(parent)
{

}
/// initializing experiment with a given name.
AMExperiment::AMExperiment(const QString& experimentName, QObject* parent)
	: AMDbObject(parent)
{

	this->setName(experimentName);
}

/// This constructor immediately loads a stored experiment from the database.
AMExperiment::AMExperiment(int databaseId, AMDatabase* database, QObject* parent)
		: AMDbObject(parent)
{
		loadFromDb(database, databaseId);
}




///
