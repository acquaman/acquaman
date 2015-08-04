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


#include "SGMDbUpgrade1Pt1.h"

SGMDbUpgrade1Pt1::SGMDbUpgrade1Pt1(QString databaseNameToUpgrade, QObject *parent) :
	AMDbUpgrade(databaseNameToUpgrade, parent)
{
}

SGMDbUpgrade1Pt1::~SGMDbUpgrade1Pt1(){}

QStringList SGMDbUpgrade1Pt1::upgradeFromTags() const{
	// No dependecies for this upgrade
	return QStringList();
}

bool SGMDbUpgrade1Pt1::upgradeNecessary() const{
	QList<int> matchingPGT;
	QList<int> matchingOceanOptics;
	QList<int> matchingMCP;

	// Check if any of these types are in the AMDbObjectTypes_table
	matchingPGT = databaseToUpgrade_->objectsMatching("AMDbObjectTypes_table", "AMDbObjectType", "PGTDetectorInfo");
	matchingOceanOptics = databaseToUpgrade_->objectsMatching("AMDbObjectTypes_table", "AMDbObjectType", "OceanOptics65000DetectorInfo");
	matchingMCP = databaseToUpgrade_->objectsMatching("AMDbObjectTypes_table", "AMDbObjectType", "MCPDetectorInfo");

	if(matchingPGT.count() > 0 || matchingOceanOptics.count() > 0 || matchingMCP.count() > 0)
		return true;
	return false;
}

bool SGMDbUpgrade1Pt1::upgradeImplementation(){

	QMap<QString, QString> parentTablesToColumnsNames;
	QMap<QString, int> indexTablesToIndexSide;
	indexTablesToIndexSide.insert("AMDetectorInfoSet_table_detectorInfos", 2);

	databaseToUpgrade_->startTransaction();

	// Use dbObjectClassBecomes to upgrade each detectorInfo to its new counterpart

	if (!AMDbUpgradeSupport::dbObjectClassBecomes(databaseToUpgrade_, "PGTDetectorInfo", "CLSPGTDetectorInfo", parentTablesToColumnsNames, indexTablesToIndexSide)){

		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	if (!AMDbUpgradeSupport::dbObjectClassBecomes(databaseToUpgrade_, "OceanOptics65000DetectorInfo", "CLSOceanOptics65000DetectorInfo", parentTablesToColumnsNames, indexTablesToIndexSide)){

		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	if (!AMDbUpgradeSupport::dbObjectClassBecomes(databaseToUpgrade_, "MCPDetectorInfo", "SGMMCPDetectorInfo", parentTablesToColumnsNames, indexTablesToIndexSide)){

		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	databaseToUpgrade_->commitTransaction();

	return true;
}

AMDbUpgrade* SGMDbUpgrade1Pt1::createCopy() const{
	AMDbUpgrade* retVal = new SGMDbUpgrade1Pt1(this->databaseNameToUpgrade());
	if(this->databaseToUpgrade())
		retVal->loadDatabaseFromName();
	return retVal;
}

QString SGMDbUpgrade1Pt1::upgradeToTag() const{
	// Upgrade tag is:
	return QString("SGMDbUpgrade1.1");
}

QString SGMDbUpgrade1Pt1::description() const{
	return QString("Upgrade PGT, OceanOptics, and MCP Detector Infos to SGM or CLS");
}
