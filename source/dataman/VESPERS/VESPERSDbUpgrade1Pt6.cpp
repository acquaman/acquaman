#include "VESPERSDbUpgrade1Pt6.h"

#include "util/AMErrorMonitor.h"

VESPERSDbUpgrade1Pt6::VESPERSDbUpgrade1Pt6(const QString &databaseNameToUpgrade, QObject *parent)
	: AMDbUpgrade(databaseNameToUpgrade, parent)
{
}

QStringList VESPERSDbUpgrade1Pt6::upgradeFromTags() const
{
	return QStringList() << "VESPERSDbUpgrade1.5";
}

bool VESPERSDbUpgrade1Pt6::upgradeNecessary() const
{
	return true;
}

bool VESPERSDbUpgrade1Pt6::upgradeImplementation()
{
	return true;
}

AMDbUpgrade *VESPERSDbUpgrade1Pt6::createCopy() const
{
	AMDbUpgrade *copy = new VESPERSDbUpgrade1Pt6(databaseNameToUpgrade());

	if (databaseToUpgrade())
		copy->loadDatabaseFromName();

	return copy;
}

QString VESPERSDbUpgrade1Pt6::upgradeToTag() const
{
	return "VESPERSDbUpgrade1.6";
}

QString VESPERSDbUpgrade1Pt6::description() const
{
	return "Upgrades database by migrating away from old AMROIInfo and AMROIInfoLists to the new AMRegionOfInterest.";
}
