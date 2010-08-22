#ifndef AMDATABASEDEFINITION_H
#define AMDATABASEDEFINITION_H


#include <QString>
#include "dataman/AMDatabase.h"
#include "dataman/AMDbObject.h"
#include "AMErrorMonitor.h"

/// This class defines the schema (version 1) for the user database.  Tools for accessing and working with an actual, arbitrary database can be found in AMDatabase.
class AMDatabaseDefinition {
public:
	static int version() { return 1; }
	static QString objectTableName() { return "Objects"; }
	static QString objectTypeTableName() { return "ObjectTypes"; }
	static QString runTableName() { return "Runs"; }
	static QString experimentTableName() { return "Experiments"; }
	static QString experimentEntriesTableName() { return "ObjectExperimentEntries"; }
	static QString databaseInformationTableName() { return "DatabaseInformation"; }
	static QString elementTableName() { return "Elements"; }
	static QString sampleTableName() { return "Samples"; }
	static QString sampleElementEntriesTableName() { return "SampleElementEntries"; }
	static QString thumbnailTableName() { return "Thumbnails"; }
	static QString detectorTableName() { return "DetectorInfo"; }
	static QString controlSetTableName() { return "ControlSetInfo"; }
	static QString samplePlateTableName() { return "Samples"; }
	static QString facilityTableName() { return "Facilities"; }

	static QString stringListSeparator() { return "|@^@|"; }

	static void initializeDatabaseTables(AMDatabase* db);

	/// Database support function: registers a subclass of AMDbObject so that the database is ready to store and access objects of this type. This only needs to be called once for each object type that you want to store in the database.
	static void registerType(const AMDbObject* prototype, AMDatabase* db);

	/// Return the type (ie: class name) of an object stored at 'id' in the main object table. (Returns empty string if not found.)
	static QString objectType(AMDatabase* db, int id);
};




#endif // AMDATABASEDEFINITION_H
