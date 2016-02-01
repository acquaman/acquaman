#include "AMLightweightScanInfo.h"
#include "database/AMDbObject.h"
#include "database/AMDbObjectSupport.h"
#include "util/AMErrorMonitor.h"

AMLightweightScanInfo::AMLightweightScanInfo(int id,
											 const QString &name,
											 int number,
											 const QDateTime &dateTime,
											 const QString &scanType,
											 int runId,
											 const QString &runName,
											 const QString &notes,
											 const QString &sampleName,
											 int thumbnailFirstId,
											 int thumbnailCount,
											 AMDatabase *database,
											 QObject *parent)
	: QObject(parent)
{
	id_ = id;
	name_ = name;
	number_ = number;
	dateTime_ = dateTime;
	scanType_ = scanType;
	runId_ = runId;
	runName_ = runName;
	notes_ = notes;
	sampleName_ = sampleName;
	database_ = database;
	if(thumbnailCount == 0)
		return;
	for(int iThumbnailId = 0; iThumbnailId < thumbnailCount; iThumbnailId++) {
		thumbnailIds_.append(thumbnailFirstId);
		thumbnailFirstId++;
	}
}

int AMLightweightScanInfo::id() const
{
	return id_;
}

QString AMLightweightScanInfo::name() const
{
	return name_;
}

void AMLightweightScanInfo::setName(const QString &name)
{
	name_ = name;
}

int AMLightweightScanInfo::number() const
{
	return number_;
}

void AMLightweightScanInfo::setNumber(int number)
{
	number_ = number;
}

QDateTime AMLightweightScanInfo::dateTime() const
{
	return dateTime_;
}

void AMLightweightScanInfo::setDateTime(const QDateTime &dateTime)
{
	dateTime_ = dateTime;
}

QString AMLightweightScanInfo::scanType() const
{
	return scanType_;
}

void AMLightweightScanInfo::setScanType(const QString &scanType)
{
	scanType_ = scanType;
}

int AMLightweightScanInfo::runId() const
{
	return runId_;
}

void AMLightweightScanInfo::setRunId(int runId)
{
	runId_ = runId;
}

QString AMLightweightScanInfo::runName() const
{
	return runName_;
}

void AMLightweightScanInfo::setRunName(const QString &runName)
{
	runName_ = runName;
}

QString AMLightweightScanInfo::notes() const
{
	return notes_;
}

void AMLightweightScanInfo::setNotes(const QString &notes)
{
	notes_ = notes;
}

QString AMLightweightScanInfo::sampleName() const
{
	return sampleName_;
}

void AMLightweightScanInfo::setSampleName(const QString &sampleName)
{
	sampleName_ = sampleName;
}

AMDbThumbnail *AMLightweightScanInfo::thumbnailAt(int index) const
{
	/// Scan doesn't have this many thumbnails, return 0
	if(index >= thumbnailCount())
		return 0;

	/// Thumbnails hasn't been loaded yet, load
	if(!thumbnailsMap_.contains(thumbnailIds_.at(index)) && database_) {
		AMDatabase* db = database_;

		QSqlQuery query = db->select(AMDbObjectSupport::thumbnailTableName(), "type, title, subtitle, thumbnail", QString("id = %1").arg(thumbnailIds_.at(index)));
		if(query.exec()) {
			while(query.next()) {
				AMDbThumbnail::ThumbnailType type;
				if(query.value(0).toString() == "PNG")
					type = AMDbThumbnail::PNGType;
				else
					type = AMDbThumbnail::InvalidType;

				QString title = query.value(1).toString();
				QString subtitle = query.value(2).toString();
				QByteArray thumbnail = query.value(3).toByteArray();

				thumbnailsMap_.insert(thumbnailIds_.at(index), new AMDbThumbnail(title, subtitle, type, thumbnail));
			}
		} else {
			AMErrorMon::alert(this, AMLIGHTWEIGHTSCANINFO_SQL_ERROR, QString("Could not complete query with error: %1").arg(query.lastError().text()));
		}
		query.finish();
	}

	return thumbnailsMap_.value(thumbnailIds_.at(index));
}

int AMLightweightScanInfo::thumbnailCount() const
{
	return thumbnailIds_.count();
}

void AMLightweightScanInfo::refreshThumbnails(int thumbnailFirstId, int thumbnailCount)
{
	clearThumbnails();
	if(thumbnailCount == 0)
		return;
	for(int iThumbnailId = 0; iThumbnailId < thumbnailCount; iThumbnailId++) {
		thumbnailIds_.append(thumbnailFirstId);
		thumbnailFirstId++;
	}
}

int AMLightweightScanInfo::thumbnailFirstId() const
{
	if(thumbnailIds_.isEmpty())
		return -1;

	return thumbnailIds_.at(0);
}

void AMLightweightScanInfo::addExperimentId(int experimentId)
{
	experimentIds_.append(experimentId);
}

void AMLightweightScanInfo::clearThumbnails()
{
	thumbnailIds_.clear();
	thumbnailsMap_.clear();
}

const QList<int> AMLightweightScanInfo::experimentIds() const
{
	return experimentIds_;
}
