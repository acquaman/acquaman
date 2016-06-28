#include "CLSDbUpgrade1Pt2.h"

#include "dataman/database/AMDbObjectSupport.h"
#include "util/AMErrorMonitor.h"

CLSDbUpgrade1Pt2::CLSDbUpgrade1Pt2(const QString &facilityName, const QString &databaseNameToUpgrade, QObject *parent)
	: AMDbUpgrade(databaseNameToUpgrade, parent)
{
	targetFacilityName_ = facilityName;

	thumbnailTableName_ = "AMDbObjectThumbnails_table";
	facilityTableName_ = "AMFacility_table";
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

	QMap<int, QString> facilityTableThumbnails = queryAMFacilityTableThumbnails();

	return facilityTableThumbnails.count() != 1 || facilityTableThumbnails.values().at(0) != targetFacilityName_;
}

bool CLSDbUpgrade1Pt2::upgradeImplementation()
{
	int dbResult = -1;

	// Prologue:
	//          - Start transation
	//          - Get the current thumbnails for AMFacility_table
	////////////////////////////////////////////////

	databaseToUpgrade_->startTransaction();

	QMap<int, QString> facilityTableThumbnails = queryAMFacilityTableThumbnails();
	if (facilityTableThumbnails.count() == 1 && facilityTableThumbnails.values().at(0) == targetFacilityName_) {
		databaseToUpgrade_->rollbackTransaction();
		AMErrorMon::alert(this, CLSDbUpgrade1Pt2_ALREADY_UPDATED, QString("The thumbnail table has the correct data. No need to updgrade."));
		return true;
	}

	// stage 1:
	//         - update the thumbnail table
	//           * remove the unnecessary thumbnail data for AMFacility table
	//
	foreach (int id, facilityTableThumbnails.keys()) {
		if (facilityTableThumbnails.value(id) != targetFacilityName_) {
			dbResult = databaseToUpgrade_->deleteRow(id, thumbnailTableName_);
			if ( dbResult == 0 ){
				AMErrorMon::alert(this, CLSDbUpgrade1Pt2_FAIL_TO_DELETE_FACILITY_THUMBNAIL, QString("Could not delete the thumbnail (%1) in %2").arg(id).arg(thumbnailTableName_));
				databaseToUpgrade_->rollbackTransaction();
				return false;
			}

			facilityTableThumbnails.remove(id);
		}
	}

	// stage 2:
	//         - update the thumbnail table
	//           *add new thumbnail data if there is no thumbnail for the target facility
	//
	if (facilityTableThumbnails.count() == 0) {

		// query the facility information
		int facilityId = -1;
		AMFacility *facility = 0;

		QSqlQuery queryFacility = databaseToUpgrade_->query();
		queryFacility.prepare(QString("SELECT id, iconFileName FROM %1 where name='%2'").arg(facilityTableName_).arg(targetFacilityName_));

		if (queryFacility.exec()) {

			if (queryFacility.first()) {
				facility = new AMFacility(targetFacilityName_, QString("CLS %1 Beamline").arg(targetFacilityName_), ":/clsIcon.png");

				facilityId = queryFacility.value(0).toInt();
				facility->setIconFileName(queryFacility.value(1).toString());
			}
		} else {
			AMErrorMon::alert(this, CLSDbUpgrade1Pt2_FAIL_TO_LOAD_FACILITY, QString("Failed to load the facility from db for %1.").arg(targetFacilityName_));
			databaseToUpgrade_->rollbackTransaction();
			return false;
		}

		// Find out how many thumbnails we're supposed to have:
		int thumbsCount = facility->thumbnailCount();
		if(thumbsCount != 1) {
			AMErrorMon::alert(this, CLSDbUpgrade1Pt2_INCORRECT_FACILITY_INFO, QString("There are %1 facility icons defined for %2").arg(thumbsCount).arg(facility->name()));
			databaseToUpgrade_->rollbackTransaction();
			return false;
		}

		// save the thumbnail for the facility

		const AMDbThumbnail& t = facility->thumbnail(0);

		QVariantList values;// list of values to store
		QStringList keys;	// list of keys (column names) to store

		keys << "objectId";
		values << facilityId;

		keys << "objectTableName";
		values << facilityTableName_;

		keys << "number";
		values << 0;

		keys << "type";
		values << t.typeString();

		keys << "title";
		values << t.title;

		keys << "subtitle";
		values << t.subtitle;

		keys << "thumbnail";
		values << t.thumbnail;

		int retVal = databaseToUpgrade_->insertOrUpdate(0, thumbnailTableName_, keys, values);
		if(retVal == 0) {
			AMErrorMon::alert(this, CLSDbUpgrade1Pt2_FAIL_TO_INSERT_THUMBNAIL, QString("Fail to save thumbnails for facility %1 in table '%2'.").arg(facility->name()).arg(thumbnailTableName_));
			databaseToUpgrade_->rollbackTransaction();
			return false;
		}

		// update the thumbnailFirstId for AMFacility table

		int firstThumbnailId = retVal; // the firstThumbnailId for Facility table

		if (!databaseToUpgrade_->update(facilityId, facilityTableName_,
										QStringList() << "thumbnailCount" << "thumbnailFirstId",
										QVariantList() << facility->thumbnailCount() << firstThumbnailId) ) {
			AMErrorMon::alert(this, CLSDbUpgrade1Pt2_FAIL_TO_UPDATE_FACILITY_TABLE, QString("Fail to store the updated thumbnail count and firstThumbnailId (%1) for table %2.").arg(firstThumbnailId).arg(facilityTableName_));
			databaseToUpgrade_->rollbackTransaction();
			return false;
		}

		facility->deleteLater();
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

QMap<int, QString> CLSDbUpgrade1Pt2::queryAMFacilityTableThumbnails() const
{
	QMap<int, QString> facilityTableThumbnails;

	QSqlQuery query = databaseToUpgrade_->query();
	query.prepare(QString("SELECT id, title FROM %1 where objectTableName='%2'").arg(thumbnailTableName_).arg(facilityTableName_));

	if (query.exec()) {
		while (query.next()) {
			facilityTableThumbnails.insert(query.value(0).toInt(), query.value(1).toString());
		}
	} else {

		AMErrorMon::alert(this, CLSDbUpgrade1Pt2_FAIL_TO_QUERY_FACILITY_THUMBNAIL, QString("%1").arg(query.lastQuery()));
		AMErrorMon::alert(this, CLSDbUpgrade1Pt2_FAIL_TO_QUERY_FACILITY_THUMBNAIL, QString("Query to find objectTableName (%1) in table %2 is failed.").arg(facilityTableName_).arg(thumbnailTableName_));
	}

//	query.finish();

	return facilityTableThumbnails;

}
