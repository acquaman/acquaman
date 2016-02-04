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


#include "AMDbUpgrade1Pt1.h"

AMDbUpgrade1Pt1::AMDbUpgrade1Pt1(QString databaseNameToUpgrade, QObject *parent) :
	AMDbUpgrade(databaseNameToUpgrade, parent)
{
}

AMDbUpgrade1Pt1::~AMDbUpgrade1Pt1(){}

QStringList AMDbUpgrade1Pt1::upgradeFromTags() const{
	// No dependecies for this upgrade
	return QStringList();
}

bool AMDbUpgrade1Pt1::upgradeNecessary() const{
	QList<int> matchingAMDetectorInfoType;
	QList<int> matchingAMDetectorInfoDescription;

	// Check to make sure that this AMDbObjectType is in the AMDbObjectTypes_table (and that it's the original type with description = "Generic Detector")
	matchingAMDetectorInfoType = databaseToUpgrade_->objectsMatching("AMDbObjectTypes_table", "AMDbObjectType", "AMDetectorInfo");
	matchingAMDetectorInfoDescription = databaseToUpgrade_->objectsMatching("AMDbObjectTypes_table", "description", "Generic Detector");

	// So we found that, make sure that the columns in the actual table are correct and this actually needs to be upgraded
	if(matchingAMDetectorInfoType.count() > 0 && matchingAMDetectorInfoDescription.count() > 0 && (matchingAMDetectorInfoType.at(0) == matchingAMDetectorInfoDescription.at(0)) ){

		QSqlQuery query = databaseToUpgrade_->query();
		query.prepare(QString("PRAGMA table_info(%1);").arg("AMDetectorInfo_table"));
		databaseToUpgrade_->execQuery(query);
		QStringList columnNames;
		if (query.first()){

			do {
				columnNames << query.value(1).toString();
			}while(query.next());
		}
		query.finish();

		if(columnNames.count() == 9 && columnNames.contains("id") && columnNames.contains("AMDbObjectType") && columnNames.contains("thumbnailCount")
				&& columnNames.contains("thumbnailFirstId") && columnNames.contains("name") && columnNames.contains("description")
				&& columnNames.contains("units") && columnNames.contains("rank") && columnNames.contains("size")){
			return true;
		}
	}

	return false;
}

bool AMDbUpgrade1Pt1::upgradeImplementation(){

	QMap<QString, QString> parentTablesToColumnsNames;
	QMap<QString, int> indexTablesToIndexSide;
	indexTablesToIndexSide.insert("AMDetectorInfoSet_table_detectorInfos", 2);

	databaseToUpgrade_->startTransaction();
	// Use dbObjectClassBecomes to upgrade each detectorInfo to its new counterpart
	if (!AMDbUpgradeSupport::dbObjectClassBecomes(databaseToUpgrade_, "AMDetectorInfo", "AMOldDetectorInfo", parentTablesToColumnsNames, indexTablesToIndexSide)){

		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	databaseToUpgrade_->commitTransaction();
	return true;
}

AMDbUpgrade* AMDbUpgrade1Pt1::createCopy() const{
	AMDbUpgrade* retVal = new AMDbUpgrade1Pt1(this->databaseNameToUpgrade());
	if(this->databaseToUpgrade())
		retVal->loadDatabaseFromName();
	return retVal;
}

QString AMDbUpgrade1Pt1::upgradeToTag() const{
	// Upgrade tag is:
	return QString("AMDbUpgrade1.1");
}

QString AMDbUpgrade1Pt1::description() const{
	return QString("Upgrade AMDetectorInfo to AMOldDetectorInfo to prepare for the new AMDetector suite.");
}
