#include "AMDbUpgrade1Pt5.h"

#include "util/AMErrorMonitor.h"

AMDbUpgrade1Pt5::AMDbUpgrade1Pt5(const QString &databaseNameToUpgrade, QObject *parent)
	: AMDbUpgrade(databaseNameToUpgrade, parent)
{
}

QStringList AMDbUpgrade1Pt5::upgradeFromTags() const
{
	return QStringList() << "AMDbUpgrade1.4";
}

bool AMDbUpgrade1Pt5::upgradeNecessary() const
{
	return true;
}
#include <QDebug>
bool AMDbUpgrade1Pt5::upgradeImplementation()
{
	databaseToUpgrade_->startTransaction();

	// Doing AMRegionScanConfiguration.
	QSqlQuery query = databaseToUpgrade_->query();
	query.prepare("SELECT inheritance FROM AMDbObjectTypes_table WHERE inheritance LIKE '%AMRegionScanConfiguration%';");

	if (!databaseToUpgrade_->execQuery(query)){

		query.finish();
		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	QStringList scanConfigurations;

	if (query.first()){

		do {

			scanConfigurations << query.value(0).toString();
		} while (query.next());
	}

	query.finish();

	qDebug() << scanConfigurations;

	// Doing AM2DScanConfiguration.
	scanConfigurations = QStringList();
	query.prepare("SELECT inheritance FROM AMDbObjectTypes_table WHERE inheritance LIKE '%AM2DScanConfiguration%';");

	if (!databaseToUpgrade_->execQuery(query)){

		query.finish();
		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	if (query.first()){

		do {

			scanConfigurations << query.value(0).toString();
		} while (query.next());
	}

	query.finish();

	qDebug() << scanConfigurations;
	return false;
}

AMDbUpgrade* AMDbUpgrade1Pt5::createCopy() const
{
	AMDbUpgrade* retVal = new AMDbUpgrade1Pt5(this->databaseNameToUpgrade());

	if(this->databaseToUpgrade())
		retVal->loadDatabaseFromName();

	return retVal;
}

QString AMDbUpgrade1Pt5::upgradeToTag() const
{
	return QString("AMDbUpgrade1.5");
}

QString AMDbUpgrade1Pt5::description() const
{
	return QString("Upgrades all scan configurations that depend on AMRegionScanConfiguration and AM2DScanConfiguration.");
}
