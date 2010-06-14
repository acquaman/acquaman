#ifndef AMDATABASEDEFINITION_H
#define AMDATABASEDEFINITION_H


#include <QString>
#include "dataman/AMDatabase.h"
#include "dataman/AMDbObject.h"

/// This class defines the schema (version 1) for the user database.  Tools for accessing and working with an actual, arbitrary database can be found in AMDatabase.
class AMDatabaseDefinition {
public:
	static int version() { return 1; }
	static QString objectTableName() { return "Objects"; }
	static QString objectTypeTableName() { return "ObjectTypes"; }
	static QString runTableName() { return "Runs"; }
	static QString experimentTableName() { return "Experiments"; }
	static QString experimentEntriesTableName() { return "ExperimentEntries"; }
	static QString databaseInformationTableName() { return "DatabaseInformation"; }

	static QString stringListSeparator() { return "|@^@|"; }

	static void initializeDatabaseTables(AMDatabase* db) {
		db->ensureTable(objectTypeTableName(), QString("type,columnNames").split(','), QString("TEXT,TEXT").split(','));
		db->ensureTable(objectTableName(), QString("type,dateTime,name,number,runId").split(','), QString("TEXT,TEXT,TEXT,INTEGER,INTEGER").split(','));
		db->ensureTable(runTableName(), QString("dateTime,name").split(','), QString("TEXT,TEXT").split(','));
		db->ensureTable(experimentTableName(), QString("dateTime,name").split(','), QString("TEXT,TEXT").split(','));
		db->ensureTable(experimentEntriesTableName(), QString("objectId,experimentId").split(','), QString("INTEGER,INTEGER").split(','));

		db->ensureTable(databaseInformationTableName(), QString("key,value").split(','), QString("TEXT,TEXT").split(','));


		// add userName and databaseVersion to DatabaseInformation
		QVariant v1, v2;
		QList<const QVariant*> vlist;
		vlist << &v1 << &v2;

		QStringList clist;
		clist << "key" << "value";

		v1.setValue(QString("userName"));
		v2.setValue(AMUserSettings::userName);

		db->insertOrUpdate(0, databaseInformationTableName(), clist, vlist);

		v1.setValue(QString("databaseVersion"));
		v2.setValue(version());
		db->insertOrUpdate(0, databaseInformationTableName(), clist, vlist);

	}

	static void registerType(const AMDbObject* prototype, AMDatabase* db) {

		QStringList columnNames;
		foreach(AMMetaMetaData col, prototype->metaDataAllKeys() ) {
			db->ensureColumn(objectTableName(), col.key, AMDatabase::metaType2DbType(col.type));
			columnNames << col.key;
		}

		QVariant v1, v2;
		QList<const QVariant*> vlist;
		vlist << &v1 << &v2;

		QStringList clist;
		clist << "type" << "columnNames";

		v1.setValue(prototype->type());
		v2.setValue(columnNames.join(stringListSeparator()));

		db->insertOrUpdate(0,objectTypeTableName(), clist, vlist);
	}


};

#endif // AMDATABASEDEFINITION_H
