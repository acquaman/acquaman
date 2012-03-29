#include "SGMDbUpgrade1Pt1.h"

SGMDbUpgrade1Pt1::SGMDbUpgrade1Pt1(QString databaseNameToUpgrade, QObject *parent) :
	AMDbUpgrade(databaseNameToUpgrade, parent)
{
}

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
	bool success = true;

	QMap<QString, QString> parentTablesToColumnsNames;
	QMap<QString, int> indexTablesToIndexSide;
	indexTablesToIndexSide.insert("AMDetectorInfoSet_table_detectorInfos", 2);

	// Use dbObjectClassBecomes to upgrade each detectorInfo to its new counterpart
	success &= AMDbUpgradeSupport::dbObjectClassBecomes(databaseToUpgrade_, "PGTDetectorInfo", "CLSPGTDetectorInfo", parentTablesToColumnsNames, indexTablesToIndexSide);
	success &= AMDbUpgradeSupport::dbObjectClassBecomes(databaseToUpgrade_, "OceanOptics65000DetectorInfo", "CLSOceanOptics65000DetectorInfo", parentTablesToColumnsNames, indexTablesToIndexSide);
	success &= AMDbUpgradeSupport::dbObjectClassBecomes(databaseToUpgrade_, "MCPDetectorInfo", "SGMMCPDetectorInfo", parentTablesToColumnsNames, indexTablesToIndexSide);

	return success;
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
