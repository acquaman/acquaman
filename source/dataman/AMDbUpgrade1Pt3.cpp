#include "AMDbUpgrade1Pt3.h"

#include "util/AMErrorMonitor.h"

#include <QProcess>

AMDbUpgrade1Pt3::AMDbUpgrade1Pt3(const QString &databaseNameToUpgrade, QObject *parent)
	: AMDbUpgrade(databaseNameToUpgrade, parent)
{
}

QStringList AMDbUpgrade1Pt3::upgradeFromTags() const
{
	return QStringList();
}

bool AMDbUpgrade1Pt3::upgradeNecessary() const
{
	QList<int> hasOldTags = databaseToUpgrade_->objectsMatching("AMScan_table", "fileFormat", "sgm2013XAS");
	return !hasOldTags.isEmpty();
}

bool AMDbUpgrade1Pt3::upgradeImplementation()
{
	databaseToUpgrade_->startTransaction();

	if (!databaseToUpgrade_->update("AMScan_table", "fileFormat='sgm2013XAS'", "fileFormat", "amRegionAscii2013")){

		AMErrorMon::alert(this, AMDBUPGRADE1PT3_COULD_NOT_UPDATE_DB, "Failed to update the fileFormat column.");
		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	databaseToUpgrade_->commitTransaction();

	QString pathToData = databaseToUpgrade_->dbAccessString();
	pathToData.replace("//", "/");
	pathToData.remove("/userdata.db");

	QProcess changingFiles;
	changingFiles.start("bash", QStringList() << "-c" << QString("grep -lr -e 'Version: SGM Generic 0.1' %1/* | xargs sed -i 's/Version: SGM Generic 0.1/Acquaman Generic Linear Step 0.1/g'").arg(pathToData));
	AMErrorMon::information(this, AMDBUPGRADE1PT3_BEGINNING_MESSAGE, "Finding and updating data files.");
	changingFiles.waitForFinished(-1);
	AMErrorMon::information(this, AMDBUPGRADE1PT3_END_MESSAGE, "Finished updating data files.");

	return true;
}

AMDbUpgrade* AMDbUpgrade1Pt3::createCopy() const
{
	AMDbUpgrade* retVal = new AMDbUpgrade1Pt3(this->databaseNameToUpgrade());

	if(this->databaseToUpgrade())
		retVal->loadDatabaseFromName();

	return retVal;
}

QString AMDbUpgrade1Pt3::upgradeToTag() const
{
	return QString("AMDbUpgrade1.3");
}

QString AMDbUpgrade1Pt3::description() const
{
	return QString("Changes the file format from sgm2013XAS to amRegionAscii2013 to remove legacy code from alpha and beta phases of the new scan controller.");
}
