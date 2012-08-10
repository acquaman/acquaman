#include "VESPERSDbUpgrade1Pt1.h"

VESPERSDbUpgrade1Pt1::VESPERSDbUpgrade1Pt1(const QString &databaseNameToUpgrade, QObject *parent)
	: AMDbUpgrade(databaseNameToUpgrade, parent)
{
}

QStringList VESPERSDbUpgrade1Pt1::upgradeFromTags() const
{
	return QStringList();
}

bool VESPERSDbUpgrade1Pt1::upgradeNecessary() const
{
	return !databaseToUpgrade_->objectsMatching("AMScan_table", "AMDbObjectType", "AM2DScan").isEmpty();
}

bool VESPERSDbUpgrade1Pt1::upgradeImplementation()
{
	QList<int> indicesToUpgrade = databaseToUpgrade_->objectsMatching("AMScan_table", "AMDbObjectType", "AM2DScan");
	QList<int> measurements;
	QVariant detectorChoice;
	QVariant numberOfElements;

	for (int i = 0, size = indexesToUpgrade.size(); i < size; i++){

		detectorChoice = databaseToUpgrade_->retrieve(indicesToUpgrade.at(i), "AMScan_table", "scanConfiguration");


	}
	return false;
}

bool VESPERSDbUpgrade1Pt1::createCopy() const
{
	AMDbUpgrade *copy = new VESPERSDbUpgrade1Pt1(this->databaseNameToUpgrade());

	if (this->databaseNameToUpgrade())
		copy->loadDatabaseFromName();

	retur copy;
}

QString VESPERSDbUpgrade1Pt1::upgradeToTag() const
{
	return "VESPERSDbUpgrade1.1";
}

QString VESPERSDbUpgrade1Pt1::description() const
{
	return "Upgrade all 2D scans to have spectra raw data sources.  Scans using the single element get one extra raw data source and the four element get five extra.";
}
