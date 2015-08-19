#include "BioXASDbUpgrade1Pt1.h"
#include "util/AMErrorMonitor.h"

BioXASDbUpgrade1Pt1::BioXASDbUpgrade1Pt1(const QString &databaseNameToUpgrade, QObject *parent) :
	AMDbUpgrade(databaseNameToUpgrade, parent)
{

}

BioXASDbUpgrade1Pt1::~BioXASDbUpgrade1Pt1()
{

}

QStringList BioXASDbUpgrade1Pt1::upgradeFromTags() const
{
	return QStringList();
}

bool BioXASDbUpgrade1Pt1::upgradeNecessary() const
{
	bool usingXRFDetector = databaseToUpgrade_->tableExists("BioXASScanConfigurationDbObject_table") && databaseToUpgrade_->columnExists("BioXASScanConfigurationDbObject_table", "usingXRFDetector");
	bool sideConfiguration = !databaseToUpgrade_->tableExists("BioXASXASScanConfiguration_table") && databaseToUpgrade_->tableExists("BioXASSideXASScanConfiguration_table");
	bool mainConfiguration = !databaseToUpgrade_->tableExists("BioXASXASScanConfiguration_table") && databaseToUpgrade_->tableExists("BioXASMainXASScanConfiguration_table");

	bool upgradeNeeded = (usingXRFDetector || sideConfiguration || mainConfiguration);
	return upgradeNeeded;
}

bool BioXASDbUpgrade1Pt1::upgradeImplementation()
{
	// Begin upgrade.

	databaseToUpgrade_->startTransaction();

	// Remove usingXRFDetector column from BioXASScanConfigurationDbObject_table, if it exists. This column is no longer used.

	if (databaseToUpgrade_->tableExists("BioXASScanConfigurationDbObject_table")) {
		if (databaseToUpgrade_->columnExists("BioXASXASScanConfigurationDbObject_table", "usingXRFDetector")) {
			if (!AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, "BioXASScanConfigurationDbObject_table", "usingXRFDetector")) {
				AMErrorMon::alert(this, BIOXASDBUPGRADE1PT1_COULD_NOT_REMOVE_COLUMN, "Could not remove the usingXRFDetector column from BioXASScanConfigurationDbObject_table.");
				databaseToUpgrade_->rollbackTransaction();
				return false;
			}
		}
	}

	// Create and populate BioXASXASScanConfiguration_table, if it doesn't already exist.

	if (!databaseToUpgrade_->tableExists("BioXASXASScanConfiguration_table")) {
		QStringList dbObjectNames = QStringList() << "AMDbObjectType" << "thumbnailCount" << "thumbnailFirstId" << "name" << "detectorConfigurations" << "axisControlInfos" << "configurationDbObject" << "header";
		QStringList dbObjectTypes = QStringList() << "TEXT" << "INTEGER" << "INTEGER" << "TEXT" << "TEXT" << "TEXT" << "TEXT" << "TEXT";

		if (!AMDbUpgradeSupport::addTable(databaseToUpgrade_, "BioXASXASScanConfiguration", dbObjectNames, dbObjectTypes, true, "BioXAS XAS Scan Configuration", "BioXASXASScanConfiguration;AMGenericStepScanConfiguration;AMStepScanConfiguration;AMScanConfiguration;AMDbObject;QObject")) {
			databaseToUpgrade_->rollbackTransaction();
			AMErrorMon::alert(this, BIOXASDBUPGRADE1PT1_COULD_NOT_CREATE_TABLE, "Could not create BioXASXASScanConfiguration_table.");
			return false;
		}

		// Populate BioXASXASScanConfiguration_table with items from BioXASSideXASScanConfiguration_table, if it exists.

		if (databaseToUpgrade_->tableExists("BioXASSideXASScanConfiguration_table")) {

			QVariantList ids = databaseToUpgrade_->retrieve("BioXASSideXASScanConfiguration_table", "id");
			QVariantList names = databaseToUpgrade_->retrieve("BioXASSideXASScanConfiguration_table", "name");
			QVariantList detectors = databaseToUpgrade_->retrieve("BioXASSideXASScanConfiguration_table", "detectorConfigurations");
			QVariantList axisControls = databaseToUpgrade_->retrieve("BioXASSideXASScanConfiguration_table", "axisControlInfos");
			QVariantList dbObjects = databaseToUpgrade_->retrieve("BioXASSideXASScanConfiguration_table", "configurationDbObject");
			QVariantList headers = databaseToUpgrade_->retrieve("BioXASSideXASScanConfiguration_table", "header");

			QList<int> updateIds;

			foreach (QVariant id, ids) {
				updateIds << id.toString().split(";").last().toInt();
			}

			foreach (int idIndex, updateIds) {

				int newId = databaseToUpgrade_->insertOrUpdate(0, "BioXASXASScanConfiguration_table",
															   QStringList()
															   << "id"
															   << "name"
															   << "detectorConfigurations"
															   << "axisControlInfos"
															   << "configurationDbObject"
															   << "header",
															   QVariantList()
															   << "BioXASXASScanConfiguration"
															   << names.at(idIndex)
															   << detectors.at(idIndex)
															   << axisControls.at(idIndex)
															   << dbObjects.at(idIndex)
															   << headers.at(idIndex)
															   );

				if (newId == 0) {
					databaseToUpgrade_->rollbackTransaction();
					AMErrorMon::alert(this, BIOXASDBUPGRADE1PT1_COULD_NOT_INSERT_OR_UPDATE_TABLE, "Could not populate BioXASXASScanConfiguration_table with old configurations from BioXASSideXASScanConfiguration_table.");
					return false;
				}
			}
		}

		// Populate BioXASXASScanConfiguration_table with items from BioXASMainXASScanConfiguration_table, if it exists.

		if (databaseToUpgrade_->tableExists("BioXASMainXASScanConfiguration_table")) {

			QVariantList ids = databaseToUpgrade_->retrieve("BioXASMainXASScanConfiguration_table", "id");
			QVariantList names = databaseToUpgrade_->retrieve("BioXASMainXASScanConfiguration_table", "name");
			QVariantList detectors = databaseToUpgrade_->retrieve("BioXASMainXASScanConfiguration_table", "detectorConfigurations");
			QVariantList axisControls = databaseToUpgrade_->retrieve("BioXASMainXASScanConfiguration_table", "axisControlInfos");
			QVariantList dbObjects = databaseToUpgrade_->retrieve("BioXASMainXASScanConfiguration_table", "configurationDbObject");
			QVariantList headers = databaseToUpgrade_->retrieve("BioXASMainXASScanConfiguration_table", "header");

			QList<int> updateIds;

			foreach (QVariant id, ids) {
				updateIds << id.toString().split(";").last().toInt();
			}

			foreach (int idIndex, updateIds) {

				int newId = databaseToUpgrade_->insertOrUpdate(0, "BioXASXASScanConfiguration_table",
															   QStringList()
															   << "id"
															   << "name"
															   << "detectorConfigurations"
															   << "axisControlInfos"
															   << "configurationDbObject"
															   << "header",
															   QVariantList()
															   << "BioXASXASScanConfiguration"
															   << names.at(idIndex)
															   << detectors.at(idIndex)
															   << axisControls.at(idIndex)
															   << dbObjects.at(idIndex)
															   << headers.at(idIndex)
															   );

				if (newId == 0) {
					databaseToUpgrade_->rollbackTransaction();
					AMErrorMon::alert(this, BIOXASDBUPGRADE1PT1_COULD_NOT_INSERT_OR_UPDATE_TABLE, "Could not populate BioXASXASScanConfiguration_table with old configurations from BioXASMainXASScanConfiguration_table.");
					return false;
				}
			}
		}
	}

	// Upgrade complete.

	databaseToUpgrade_->commitTransaction();

	return true;
}

AMDbUpgrade* BioXASDbUpgrade1Pt1::createCopy() const
{
	AMDbUpgrade *copy = new BioXASDbUpgrade1Pt1(databaseNameToUpgrade());

	if (databaseToUpgrade())
		copy->loadDatabaseFromName();

	return copy;
}

QString BioXASDbUpgrade1Pt1::upgradeToTag() const
{
	return "BioXASDbUpgrade1.1";
}

QString BioXASDbUpgrade1Pt1::description() const
{
	return "Upgrades database to account for new BioXASXASScanConfiguration, which functionally replaces existing BioXASSideXASScanConfiguration and BioXASMainXASScanConfiguration.";
}
