#include "SGMDbUpgrade1Pt1.h"

SGMDbUpgrade1Pt1::SGMDbUpgrade1Pt1(QStringList databaseNamesToUpgrade, QObject *parent) :
	AMDbUpgrade(databaseNamesToUpgrade, parent)
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

	// Loop over all of the requested databases to check if any of these types are in the AMDbObjectTypes_table
	for(int x = 0; x < databasesToUpgrade_.count(); x++){
		matchingPGT = databasesToUpgrade_.at(x)->objectsMatching("AMDbObjectTypes_table", "AMDbObjectType", "PGTDetectorInfo");
		matchingOceanOptics = databasesToUpgrade_.at(x)->objectsMatching("AMDbObjectTypes_table", "AMDbObjectType", "OceanOptics65000DetectorInfo");
		matchingMCP = databasesToUpgrade_.at(x)->objectsMatching("AMDbObjectTypes_table", "AMDbObjectType", "MCPDetectorInfo");

		if(matchingPGT.count() > 0 || matchingOceanOptics.count() > 0 || matchingMCP.count() > 0)
			return true;
	}
	return false;
}

bool SGMDbUpgrade1Pt1::upgradeImplementation(){
	bool success = true;

	QMap<QString, QString> parentTablesToColumnsNames;
	QMap<QString, int> indexTablesToIndexSide;
	indexTablesToIndexSide.insert("AMDetectorInfoSet_table_detectorInfos", 2);
	// Loop over all of the requested databases to use dbObjectClassBecomes to upgrade each detectorInfo to its new counterpart
	for(int x = 0; x < databasesToUpgrade_.count(); x++){
		success &= AMDbUpgradeSupport::dbObjectClassBecomes(databasesToUpgrade_.at(x), "PGTDetectorInfo", "CLSPGTDetectorInfo", parentTablesToColumnsNames, indexTablesToIndexSide);
		success &= AMDbUpgradeSupport::dbObjectClassBecomes(databasesToUpgrade_.at(x), "OceanOptics65000DetectorInfo", "CLSOceanOptics65000DetectorInfo", parentTablesToColumnsNames, indexTablesToIndexSide);
		success &= AMDbUpgradeSupport::dbObjectClassBecomes(databasesToUpgrade_.at(x), "MCPDetectorInfo", "SGMMCPDetectorInfo", parentTablesToColumnsNames, indexTablesToIndexSide);
	}

	return success;
}

QString SGMDbUpgrade1Pt1::upgradeToTag() const{
	// Upgrade tag is:
	return QString("SGMDbUpgrade1.1");
}

QString SGMDbUpgrade1Pt1::description() const{
	return QString("Upgrade PGT, OceanOptics, and MCP Detector Infos to SGM or CLS");
}
