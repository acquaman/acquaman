#include "CLSDbUpgrade1Pt1.h"

#include <QStringBuilder>

#include "util/AMErrorMonitor.h"

CLSDbUpgrade1Pt1::CLSDbUpgrade1Pt1(const QString &facilityName, const QString &databaseNameToUpgrade, QObject *parent)
	: AMDbUpgrade(databaseNameToUpgrade, parent)
{
	targetFacilityName_ = facilityName;
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
	//   - the AMRun table has the "facilityId" column
	//   - The AMFacility table has more than one record, or
	//   - the name of the only facility doesn't match with the given beamline name, or

	bool hasFacilityId = databaseToUpgrade_->tableExists("AMRun_table") && databaseToUpgrade_->columnExists("AMRun_table", "facilityId");

	QVariantList tableFacilityNames = databaseToUpgrade_->retrieve("AMFacility_table", "name");
	bool singleFacility = (tableFacilityNames.count() == 1);

	bool correctFacilityName = false;
	if (singleFacility)
		correctFacilityName = (tableFacilityNames.at(0).toString() == targetFacilityName_);

	return hasFacilityId || !singleFacility || !correctFacilityName;
}

bool CLSDbUpgrade1Pt1::upgradeImplementation()
{
	int dbResult = -1;

	// Prologue.
	////////////////////////////////////////////////

	databaseToUpgrade_->startTransaction();

	// Stage 1:  update the AMFacility if required
	///////////////////////////////////////////////////
	QVariantList tableFacilityNames = databaseToUpgrade_->retrieve("AMFacility_table", "name");
	bool singleFacility = (tableFacilityNames.count() == 1);
	bool correctFacilityName = false;
	if (singleFacility)
		correctFacilityName = (tableFacilityNames.at(0).toString() == targetFacilityName_);

	if (!singleFacility || !correctFacilityName) {
		// remove all the facilites
		for (int rowId = tableFacilityNames.count(); rowId > 0; rowId--) {
			dbResult = databaseToUpgrade_->deleteRow(rowId, "AMFacility_table");
			if ( dbResult == 0 ){
				AMErrorMon::alert(this, CLSDBUPGRADE1PT1_COULD_NOT_DELETE_FACILITY, QString("Could not delete the facility (%1) in AMFacility_table").arg(rowId));
				databaseToUpgrade_->rollbackTransaction();
				return false;
			}
		}

		// add the new facility
		QStringList columns = QStringList() << "name" << "description" << "iconFileName";
		QVariantList values = QVariantList() << targetFacilityName_ <<  QString("CLS %1 Beamline").arg(targetFacilityName_) << ":/clsIcon.png";
		dbResult = databaseToUpgrade_->insertOrUpdate(0, "AMFacility_table", columns, values);
		if (dbResult == 0) {
			AMErrorMon::alert(this, CLSDBUPGRADE1PT1_COULD_NOT_INSERT_NEW_FACILITY, QString("Could not insert the facility (%1) in AMFacility_table").arg(targetFacilityName_));
			databaseToUpgrade_->rollbackTransaction();
			return false;
		}
	}

	// Stage 2:  update the AMRun table by deleting the facilityId column
	///////////////////////////////////////////////////
	bool updateAMRunTable = databaseToUpgrade_->tableExists("AMRun_table") && databaseToUpgrade_->columnExists("AMRun_table", "facilityId");
	if (updateAMRunTable) {
		if (!AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, "AMRun_table", "facilityId")) {
			AMErrorMon::alert(this, CLSDBUPGRADE1PT1_COULD_NOT_DELETE_AMRUN_COLUMN, "Could not remove the facilityId column from AMRun_table.");
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
	AMDbUpgrade *copy = new CLSDbUpgrade1Pt1(targetFacilityName_, databaseNameToUpgrade());

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
	return QString("Remove the CLS Beamline facility Ids for AMRun table and update AMFacility table.");
}
