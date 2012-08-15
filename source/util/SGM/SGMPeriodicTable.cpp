/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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
			// Grab all the ids in this table (search for AMDbObjectType == SGMElementInfo should return everything)
			matchIDs = tempDatabase->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMElementInfo>(), "AMDbObjectType", "SGMElementInfo");
			for(int y = 0; y < matchIDs.count(); y++){
				elementInfo = new SGMElementInfo();
				if(elementInfo->loadFromDb(tempDatabase, matchIDs.at(y)))
					sgmPeriodicTableInfo_.append(elementInfo, elementInfo->element());
			}
		}
	}
}

SGMElementInfo* SGMPeriodicTable::elementInfoByName(const QString &elementName) const{
	if(sgmPeriodicTableInfo_.contains(AMPeriodicTable::table()->elementByName(elementName)))
		return sgmPeriodicTableInfo_.at(sgmPeriodicTableInfo_.indexOfKey(AMPeriodicTable::table()->elementByName(elementName)));
	else
		return 0; //NULL
}

QStringList SGMPeriodicTable::fastScanPresetsStrings() const{
	QStringList retVal;
	QString tmpStr;
	SGMElementInfo *tmpElementInfo;
	for(int x = 0; x < sgmPeriodicTableInfo_.count(); x++){
		tmpElementInfo = sgmPeriodicTableInfo_.at(x);
		for(int y = 0; y < tmpElementInfo->availableFastScanParameters().count(); y++)
			retVal << tmpElementInfo->availableFastScanParameters().at(y)->element() + " " + tmpElementInfo->availableFastScanParameters().at(y)->edge() + " " + tmpStr.setNum(tmpElementInfo->availableFastScanParameters().at(y)->runSeconds());
	}
	return retVal;
}

QList<SGMFastScanParameters*> SGMPeriodicTable::fastScanPresets() const{
	QList<SGMFastScanParameters*> retVal;
	SGMElementInfo *tmpElementInfo;
	for(int x = 0; x < sgmPeriodicTableInfo_.count(); x++){
		tmpElementInfo = sgmPeriodicTableInfo_.at(x);
		for(int y = 0; y < tmpElementInfo->availableFastScanParameters().count(); y++)
			retVal << tmpElementInfo->availableFastScanParameters().at(y);
	}
	return retVal;
}
