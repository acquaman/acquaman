#include "AMDbLoader.h"

#include "dataman/AMScan.h"
#include "dataman/AMXASScan.h"
#include "dataman/AMDatabaseDefinition.h"

#include <QDebug>

AMDbObject* AMDbLoader::createAndLoad(AMDatabase* db, int id, QObject* parent, QString* type) {
	// Get the type of the object that lives here:
	QString realType = AMDatabaseDefinition::objectType(db, id);

	// If the user asked for the return type name, provide it:
	if(type)
		*type = realType;

	AMDbObject* returnObject = 0;

	// List of type possiblities starts here:
	if(realType == "AMDbObject") {
		returnObject = new AMDbObject(parent);
		returnObject->loadFromDb(db, id);
	}
	else if(realType == "AMScan") {
		returnObject = new AMScan(parent);
		returnObject->loadFromDb(db, id);
	}
	else if (realType == "AMXASScan") {
		returnObject = new AMXASScan(QList<AMDetectorInfo*>(), parent);
		returnObject->loadFromDb(db, id);
	}

	/// \note Add additional subclasses here:

	return returnObject;
}
