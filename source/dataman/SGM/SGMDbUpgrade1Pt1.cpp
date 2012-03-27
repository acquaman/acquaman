#include "SGMDbUpgrade1Pt1.h"

SGMDbUpgrade1Pt1::SGMDbUpgrade1Pt1(QList<AMDatabase *> databasesToUpgrade, QObject *parent) :
	AMDbUpgrade(databasesToUpgrade, parent)
{
}

QStringList SGMDbUpgrade1Pt1::upgradeFromTags(){
	return QStringList();
}

bool SGMDbUpgrade1Pt1::upgradeNecessary(){
	AMDatabase *userDb = AMDatabase::database("user");

	QList<int> matchingPGT;
	QList<int> matchingOceanOptics;
	QList<int> matchingMCP;

	// Find the number of objects matching the original and new class names in the AMDbObjectTypes table
	matchingPGT = userDb->objectsMatching("AMDbObjectTypes_table", "AMDbObjectType", "PGTDetectorInfo");
	matchingOceanOptics = userDb->objectsMatching("AMDbObjectTypes_table", "AMDbObjectType", "OceanOptics65000DetectorInfo");
	matchingMCP = userDb->objectsMatching("AMDbObjectTypes_table", "AMDbObjectType", "MCPDetectorInfo");

	if(matchingPGT.count() > 0 || matchingOceanOptics.count() > 0 || matchingMCP.count() > 0)
		return true;
	return false;
}

bool SGMDbUpgrade1Pt1::upgradeImplementation(){
	bool success = true;

	QMap<QString, QString> parentTablesToColumnsNames;
	QMap<QString, int> indexTablesToIndexSide;
	indexTablesToIndexSide.insert("AMDetectorInfoSet_table_detectorInfos", 2);
	success &= AMDbUpgradeSupport::dbObjectClassBecomes("PGTDetectorInfo", "CLSPGTDetectorInfo", parentTablesToColumnsNames, indexTablesToIndexSide);
	success &= AMDbUpgradeSupport::dbObjectClassBecomes("OceanOptics65000DetectorInfo", "CLSOceanOptics65000DetectorInfo", parentTablesToColumnsNames, indexTablesToIndexSide);
	success &= AMDbUpgradeSupport::dbObjectClassBecomes("MCPDetectorInfo", "SGMMCPDetectorInfo", parentTablesToColumnsNames, indexTablesToIndexSide);

	return success;
}

QString SGMDbUpgrade1Pt1::upgradeToTag(){
	return QString("SGMDbUpgrade1.1");
}

QString SGMDbUpgrade1Pt1::description(){
	return QString("Upgrade PGT, OceanOptics, and MCP Detector Infos to SGM or CLS");
}
