/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "SGMPeriodicTable.h"
#include "dataman/database/AMDbObjectSupport.h"

//Singleton instance
SGMPeriodicTable* SGMPeriodicTable::instance_ = 0;
 SGMPeriodicTable::~SGMPeriodicTable(){}
QString SGMPeriodicTable::SGMPeriodicTableAllDatabasesConnectionName_ = "^%^AllDatabases^%^";

SGMPeriodicTable::SGMPeriodicTable(QObject *parent) :
		QObject(parent)
{
	// Grab all the databases available but remove the "actions" database ... there shouldn't be anything in there
	QStringList allDatabases = AMDatabase::registeredDatabases();
	allDatabases.removeAll("actions");

	// Loop over all databases and load them up, if successful grab the elementInfos and load them
	AMDatabase *tempDatabase;
	SGMElementInfo *elementInfo;
	QList<int> matchIDs;
	for(int x = 0; x < allDatabases.count(); x++){
		tempDatabase = AMDatabase::database(allDatabases.at(x));
		if(tempDatabase){
			AMOrderedSet<AMElement*, SGMElementInfo*> *thisDatabaseMapping = new AMOrderedSet<AMElement*, SGMElementInfo*>();
			// Grab all the ids in this table (search for AMDbObjectType == SGMElementInfo should return everything)
			matchIDs = tempDatabase->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMElementInfo>(), "AMDbObjectType", "SGMElementInfo");
			for(int y = 0; y < matchIDs.count(); y++){
				elementInfo = new SGMElementInfo();
				if(elementInfo->loadFromDb(tempDatabase, matchIDs.at(y)))
					thisDatabaseMapping->append(elementInfo, elementInfo->element());
			}
			sgmPeriodicTableInfo_.append(thisDatabaseMapping, tempDatabase);
		}
	}
}

SGMElementInfo* SGMPeriodicTable::elementInfoByName(const QString &elementName, const QString &databaseConnectionName) const{
	AMDatabase *requestedDatabase = AMDatabase::database(databaseConnectionName);
	if(!requestedDatabase || !sgmPeriodicTableInfo_.contains(requestedDatabase))
		return 0; //NULL
	if(sgmPeriodicTableInfo_.at(sgmPeriodicTableInfo_.indexOfKey(requestedDatabase))->contains(AMPeriodicTable::table()->elementByName(elementName)))
		return sgmPeriodicTableInfo_.at(sgmPeriodicTableInfo_.indexOfKey(requestedDatabase))->at(sgmPeriodicTableInfo_.at(sgmPeriodicTableInfo_.indexOfKey(requestedDatabase))->indexOfKey(AMPeriodicTable::table()->elementByName(elementName)));
	else
		return 0; //NULL
}

QStringList SGMPeriodicTable::fastScanPresetsStrings(const QString &databaseConnectionName) const{
	QStringList retVal;
	QString tmpStr;
	SGMElementInfo *tmpElementInfo;
	if(databaseConnectionName != SGMPeriodicTableAllDatabasesConnectionName()){
		AMDatabase *requestedDatabase = AMDatabase::database(databaseConnectionName);
		if(!requestedDatabase || !sgmPeriodicTableInfo_.contains(requestedDatabase))
			return retVal;
		for(int x = 0; x < sgmPeriodicTableInfo_.at(sgmPeriodicTableInfo_.indexOfKey(requestedDatabase))->count(); x++){
			tmpElementInfo = sgmPeriodicTableInfo_.at(sgmPeriodicTableInfo_.indexOfKey(requestedDatabase))->at(x);
			for(int y = 0; y < tmpElementInfo->availableFastScanParameters().count(); y++)
				retVal << tmpElementInfo->availableFastScanParameters().at(y)->element() + " " + tmpElementInfo->availableFastScanParameters().at(y)->edge() + " " + tmpStr.setNum(tmpElementInfo->availableFastScanParameters().at(y)->runSeconds());
		}
	}
	else{
		QStringList allDatabaseConnectionNames = AMDatabase::registeredDatabases();
		AMDatabase *requestedDatabase;
		for(int z = 0; z < allDatabaseConnectionNames.count(); z++){
			requestedDatabase = AMDatabase::database(allDatabaseConnectionNames.at(z));
			if(requestedDatabase && sgmPeriodicTableInfo_.indexOfKey(requestedDatabase) >= 0 ){
				for(int x = 0; x < sgmPeriodicTableInfo_.at(sgmPeriodicTableInfo_.indexOfKey(requestedDatabase))->count(); x++){
					tmpElementInfo = sgmPeriodicTableInfo_.at(sgmPeriodicTableInfo_.indexOfKey(requestedDatabase))->at(x);
					for(int y = 0; y < tmpElementInfo->availableFastScanParameters().count(); y++)
						retVal << tmpElementInfo->availableFastScanParameters().at(y)->element() + " " + tmpElementInfo->availableFastScanParameters().at(y)->edge() + " " + tmpStr.setNum(tmpElementInfo->availableFastScanParameters().at(y)->runSeconds());
				}
			}
		}
	}
	return retVal;
}

QList<SGMFastScanParameters*> SGMPeriodicTable::fastScanPresets(const QString &databaseConnectionName) const{
	QList<SGMFastScanParameters*> retVal;
	if(databaseConnectionName != SGMPeriodicTableAllDatabasesConnectionName()){
		AMDatabase *requestedDatabase = AMDatabase::database(databaseConnectionName);
		if(!requestedDatabase || !sgmPeriodicTableInfo_.contains(requestedDatabase))
			return retVal;
		SGMElementInfo *tmpElementInfo;
		for(int x = 0; x < sgmPeriodicTableInfo_.at(sgmPeriodicTableInfo_.indexOfKey(requestedDatabase))->count(); x++){
			tmpElementInfo = sgmPeriodicTableInfo_.at(sgmPeriodicTableInfo_.indexOfKey(requestedDatabase))->at(x);
			for(int y = 0; y < tmpElementInfo->availableFastScanParameters().count(); y++)
				retVal << tmpElementInfo->availableFastScanParameters().at(y);
		}
	}
	else{
		QStringList allDatabaseConnectionNames = AMDatabase::registeredDatabases();
		AMDatabase *requestedDatabase;
		for(int z = 0; z < allDatabaseConnectionNames.count(); z++){
			requestedDatabase = AMDatabase::database(allDatabaseConnectionNames.at(z));
			if(requestedDatabase && sgmPeriodicTableInfo_.indexOfKey(requestedDatabase) >= 0 ){
				SGMElementInfo *tmpElementInfo;
				for(int x = 0; x < sgmPeriodicTableInfo_.at(sgmPeriodicTableInfo_.indexOfKey(requestedDatabase))->count(); x++){
					tmpElementInfo = sgmPeriodicTableInfo_.at(sgmPeriodicTableInfo_.indexOfKey(requestedDatabase))->at(x);
					for(int y = 0; y < tmpElementInfo->availableFastScanParameters().count(); y++)
						retVal << tmpElementInfo->availableFastScanParameters().at(y);
				}
			}
		}
	}
	return retVal;
}
