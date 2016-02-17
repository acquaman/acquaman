#include "CLSDbUpgrade1Pt1.h"

#include "util/AMErrorMonitor.h"

CLSDbUpgrade1Pt1::CLSDbUpgrade1Pt1(CLSAppController::CLSBeamlineID targetFacilityId, const QString &databaseNameToUpgrade, QObject *parent)
	: AMDbUpgrade(databaseNameToUpgrade, parent)
{
	targetFacilityId_ = targetFacilityId;
}

CLSDbUpgrade1Pt1::~CLSDbUpgrade1Pt1()
{

}

QStringList CLSDbUpgrade1Pt1::upgradeFromTags() const
{
	return QStringList();
}

bool CLSDbUpgrade1Pt1::upgradeNecessary() const
{
	// run update when:
	//
	//   - the facilityId column doesn't exist for AMFacility or
	//   - the total number of facilities doesn't match with the existing max values

	bool hasFacilityId = databaseToUpgrade_->tableExists("AMFacility_table") && databaseToUpgrade_->columnExists("AMFacility_table", "facilityId");

	QVariantList tableFacilityNames = databaseToUpgrade_->retrieve("AMFacility_table", "name");
	bool matchFacilityCount = (tableFacilityNames.count() == CLSAppController::CLSMAXBeamlineId - 1);

	return !hasFacilityId || !matchFacilityCount;
}

bool CLSDbUpgrade1Pt1::upgradeImplementation()
{
	// Prologue.
	////////////////////////////////////////////////

	databaseToUpgrade_->startTransaction();

	// Stage 1:  Create facilityId column for AMFacility
	///////////////////////////////////////////////////

	if (!databaseToUpgrade_->ensureColumn("AMFacility_table", "facilityId", "INTEGER")){

		AMErrorMon::alert(this, CLSDBUPGRADE1PT1_COULD_NOT_CREATE_COLUMN, "Could not create the facilitId column in AMFacility_table");
		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	// Stage 2:  update the facility Ids for AMFacility and add the missing facility
	///////////////////////////////////////////////////
	QVariantList tableFacilityNames = databaseToUpgrade_->retrieve("AMFacility_table", "name");

	for (int id = 1; id < CLSAppController::CLSMAXBeamlineId; id++) {
		QString facilityName = CLSAppController::beamlineName(CLSAppController::CLSBeamlineID(id));

		if (tableFacilityNames.contains(facilityName)) {
			// update existing facility
			QString whereClause = QString("name=%1").arg(facilityName);
			bool updated = databaseToUpgrade_->update("AMFacility_table", whereClause, "facilityId", id);
			if (!updated) {
				AMErrorMon::alert(this, CLSDBUPGRADE1PT1_COULD_NOT_UPDATE_COLUMN, QString("Could not update the facilityId (%1) for %2 in AMFacility_table").arg(id).arg(facilityName));
				databaseToUpgrade_->rollbackTransaction();
				return false;
			}
		} else {
			// insert new/missing facility
			QStringList columnList = QStringList() << "name" << "facilityId" << "description" << "iconFileName";
			QVariantList valuesList = QVariantList() << facilityName << id << QString("CLS %1 beamline").arg(facilityName) << ":/clsIcon.png";
			int newId = databaseToUpgrade_->insertOrUpdate(0, "AMFacility_table", columnList, valuesList);
			if (newId == 0) {
				AMErrorMon::alert(this, CLSDBUPGRADE1PT1_COULD_NOT_INSERT_NEW_FACILITY, QString("Could not create the facilityId (%1) for %2 in AMFacility_table").arg(id).arg(facilityName));
				databaseToUpgrade_->rollbackTransaction();
				return false;
			}
		}
	}


	// Stage 3:  update the facility Ids for AMRun
	///////////////////////////////////////////////////
	if (targetFacilityId_ == 0) {
		AMErrorMon::alert(this, CLSDBUPGRADE1PT1_COULD_NOT_UPDATE_COLUMN, QString("Could not update the facilityId for AMRun_table, since no targetFacilityId matches"));
		databaseToUpgrade_->rollbackTransaction();
		return false;
	}

	QVariantList amRunNames = databaseToUpgrade_->retrieve("AMRun_table", "name");
	foreach (QVariant runName, amRunNames) {
		QString whereClause = QString("name=%1").arg(runName.toString());
		bool updated = databaseToUpgrade_->update("AMRun_table", whereClause, "facilityId", targetFacilityId_);
		if (!updated) {
			AMErrorMon::alert(this, CLSDBUPGRADE1PT1_COULD_NOT_UPDATE_COLUMN, QString("Could not update the facilityId (%1) for run %2 in AMRun_table").arg(targetFacilityId_).arg(runName.toString()));
			databaseToUpgrade_->rollbackTransaction();
			return false;
		}
	}

	// Epilogue.
	///////////////////////////////////////////////////////
	databaseToUpgrade_->commitTransaction();

	return true;
}

AMDbUpgrade * CLSDbUpgrade1Pt1::createCopy() const
{
	AMDbUpgrade *copy = new CLSDbUpgrade1Pt1(targetFacilityId_, databaseNameToUpgrade());

	if (databaseToUpgrade())
		copy->loadDatabaseFromName();

	return copy;
}

QString CLSDbUpgrade1Pt1::upgradeToTag() const
{
	return "CLSDbUpgrade1.1";
}

QString CLSDbUpgrade1Pt1::description() const
{
	return QString("Upgrades the CLS Beamline facility Ids for AMFacility and AMRun tables.");
}
