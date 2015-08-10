#include "IDEASDbUpgrade1Pt1.h"

#include "util/AMErrorMonitor.h"

IDEASDbUpgrade1Pt1::IDEASDbUpgrade1Pt1(const QString &databaseNameToUpgrade, QObject *parent)
	: AMDbUpgrade(databaseNameToUpgrade, parent)
{
}

IDEASDbUpgrade1Pt1::~IDEASDbUpgrade1Pt1()
{

}

QStringList IDEASDbUpgrade1Pt1::upgradeFromTags() const
{
	return QStringList();
}

bool IDEASDbUpgrade1Pt1::upgradeNecessary() const
{
	return true;
}

bool IDEASDbUpgrade1Pt1::upgradeImplementation()
{
	return false;
}

AMDbUpgrade *IDEASDbUpgrade1Pt1::createCopy() const
{
	AMDbUpgrade *copy = new IDEASDbUpgrade1Pt1(databaseNameToUpgrade());

	if (databaseToUpgrade())
		copy->loadDatabaseFromName();

	return copy;
}

QString IDEASDbUpgrade1Pt1::upgradeToTag() const
{
	return "IDEASDbUpgrade1.1";
}

QString IDEASDbUpgrade1Pt1::description() const
{
	return "Gives IDEAS an IDEASScanConfigurationDbObject and does all necessary data management to ensure that old scans can load with the new table scheme.";
}
