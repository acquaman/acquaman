#include "AMDbUpgrade1Pt7.h"

#include "util/AMErrorMonitor.h"

AMDbUpgrade1Pt7::AMDbUpgrade1Pt7(CLSFacilityID::Beamlines facilityId, const QString &databaseNameToUpgrade, QObject *parent)
	: AMDbUpgrade(databaseNameToUpgrade, parent)
{
	facilityId_ = facilityId;
}

AMDbUpgrade1Pt7::~AMDbUpgrade1Pt7()
{

}

QStringList AMDbUpgrade1Pt7::upgradeFromTags() const
{
	return QStringList();
}

bool AMDbUpgrade1Pt7::upgradeNecessary() const
{
	// if the facilityId doesn't match, we will do the upgrade
	return databaseToUpgrade_->objectsMatching("AMRun_table", "facilityId", facilityId_).isEmpty();
}

bool AMDbUpgrade1Pt7::upgradeImplementation()
{
	// Prologue.
	////////////////////////////////////////////////

	databaseToUpgrade_->startTransaction();

	// Stage 1:  update the facility Id
	///////////////////////////////////////////////////

	QString whereClause = QString("name=%1").arg(CLSFacilityID::beamlineName(facilityId_));
	bool updated = databaseToUpgrade_->update("AMRun_table", whereClause, "facilityId", facilityId_);
	if (!updated) {
		AMErrorMon::alert(this, AMDBUPGRADE1PT7_COULD_NOT_UPDATE_COLUMN, QString("Could not update the facilityId (%1) for %2 in AMRun_table").arg(facilityId_).arg(CLSFacilityID::beamlineName(facilityId_)));
		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	// Epilogue.
	///////////////////////////////////////////////////////
	databaseToUpgrade_->commitTransaction();

	return true;
}

AMDbUpgrade * AMDbUpgrade1Pt7::createCopy() const
{
	AMDbUpgrade *copy = new AMDbUpgrade1Pt7(facilityId_, databaseNameToUpgrade());

	if (databaseToUpgrade())
		copy->loadDatabaseFromName();

	return copy;
}

QString AMDbUpgrade1Pt7::upgradeToTag() const
{
	return "AMDbUpgrade1.7";
}

QString AMDbUpgrade1Pt7::description() const
{
	return QString("Upgrades the Beamline facility ID of %1to %2.").arg(CLSFacilityID::beamlineName(facilityId_)).arg(facilityId_);
}
