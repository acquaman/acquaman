#include "CLSDbUpgrade1Pt2.h"

#include <QStringBuilder>

#include "dataman/database/AMDbObjectSupport.h"
#include "util/AMErrorMonitor.h"

CLSDbUpgrade1Pt2::CLSDbUpgrade1Pt2(const QString &facilityName, const QString &databaseNameToUpgrade, QObject *parent)
	: AMDbUpgrade(databaseNameToUpgrade, parent)
{
	targetFacilityName_ = facilityName;
}

CLSDbUpgrade1Pt2::~CLSDbUpgrade1Pt2()
{

}

QStringList CLSDbUpgrade1Pt2::upgradeFromTags() const
{
	return QStringList();
}

bool CLSDbUpgrade1Pt2::upgradeNecessary() const
{
	// run update when:
	//
	//   - The AMDbObjectThumbnails_table doesn't have thumbnail for the given beamline name or has more than one thumbnails for AMFacilityTable

	QString thumbnailTableName = AMDbObjectSupport::thumbnailTableName();
	QMap<int, QString> facilityTableThumbnails;

	QSqlQuery thumbnailTableQuery = databaseToUpgrade_->select(thumbnailTableName, "id, title", QString("objectTableName=%1").arg("AMFacility_table") );
	if (thumbnailTableQuery.exec()) {
		while (thumbnailTableQuery.next()) {
			facilityTableThumbnails.insert(thumbnailTableQuery.value(0).toInt(), thumbnailTableQuery.value(1).toString()); //id, title
//			qDebug() << "==== " << thumbnailTableQuery.value(0).toInt() << thumbnailTableQuery.value(1).toString();
		}
	}
	thumbnailTableQuery.finish();

	bool correctThumbnailTableData = (facilityTableThumbnails.count() == 1) && (facilityTableThumbnails.values().at(0) == targetFacilityName_);
//	qDebug() << "==== " << correctThumbnailTableData;

	return !correctThumbnailTableData;
}

bool CLSDbUpgrade1Pt2::upgradeImplementation()
{
	int dbResult = -1;

	// Prologue.
	////////////////////////////////////////////////

	databaseToUpgrade_->startTransaction();

	QString thumbnailTableName = AMDbObjectSupport::thumbnailTableName();

	// Stage 1: update the thumbnail table if required
	//   - The AMDbObjectThumbnails_table doesn't have thumbnail for the given beamline name
	QMap<int, QString> facilityTableThumbnails;
	QSqlQuery thumbnailTableQuery = databaseToUpgrade_->select(thumbnailTableName, "id, title", QString("objectTableName=%1").arg("AMFacility_table") );
	if (thumbnailTableQuery.exec()) {
		while (thumbnailTableQuery.next()) {
			facilityTableThumbnails.insert(thumbnailTableQuery.value(0).toInt(), thumbnailTableQuery.value(1).toString()); //id, title
		}
	}
	thumbnailTableQuery.finish();

	bool correctThumbnailTableData = (facilityTableThumbnails.count() == 1) && (facilityTableThumbnails.values().at(0) == targetFacilityName_);
	if (!correctThumbnailTableData) {
		foreach (int id, facilityTableThumbnails.keys()) {
			if (facilityTableThumbnails.value(id) != targetFacilityName_) {
				dbResult = databaseToUpgrade_->deleteRow(id, thumbnailTableName);
				if ( dbResult == 0 ){
					AMErrorMon::alert(this, CLSDbUpgrade1Pt2_COULD_NOT_DELETE_FACILITY, QString("Could not delete the thumbnail (%1) in %2").arg(id).arg(thumbnailTableName));
					databaseToUpgrade_->rollbackTransaction();
					return false;
				}

				facilityTableThumbnails.remove(id);
			}
		}

		// if we still have one facility table thumbnail left, it is the one we want to keep. Otherwise, we will create a new one
		if (facilityTableThumbnails.count() == 0) {
			// sth is wrong with our previous updagrades, we need to redo the facility
			// delete the existing facility and re-add it along with thumbnail
			dbResult = databaseToUpgrade_->deleteRow(1, "AMFacility_table");
			if ( dbResult == 0 ){
				AMErrorMon::alert(this, CLSDbUpgrade1Pt2_COULD_NOT_DELETE_FACILITY, QString("Could not delete the facility in %1").arg("AMFacility_table"));
							databaseToUpgrade_->rollbackTransaction();
							return false;
			}

			// add the new facility and create a new thumbnail
			AMFacility *facility = new AMFacility(targetFacilityName_, QString("CLS %1 Beamline").arg(targetFacilityName_), ":/clsIcon.png");
			if (!facility->storeToDb(databaseToUpgrade_)) { // this will create Thumbnail as welld
				AMErrorMon::alert(this, CLSDbUpgrade1Pt2_COULD_NOT_INSERT_NEW_FACILITY, QString("Could not insert the facility (%1) in %2").arg(targetFacilityName_).arg("AMFacility_table"));
				databaseToUpgrade_->rollbackTransaction();
				facility->deleteLater();
				return false;
			}
			facility->deleteLater();
		}
	}

	// Epilogue.
	///////////////////////////////////////////////////////
	databaseToUpgrade_->commitTransaction();

	return true;
}

AMDbUpgrade * CLSDbUpgrade1Pt2::createCopy() const
{
	AMDbUpgrade *copy = new CLSDbUpgrade1Pt2(targetFacilityName_, databaseNameToUpgrade());

	if (databaseToUpgrade())
		copy->loadDatabaseFromName();

	return copy;
}

QString CLSDbUpgrade1Pt2::upgradeToTag() const
{
	return "CLSDbUpgrade1.2";
}

QString CLSDbUpgrade1Pt2::description() const
{
	return QString("Check the AMDBThumbnail table on the thumbnails of AMFacility table.");
}
