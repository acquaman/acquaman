#include "AMLightweightScanInfo.h"
#include "database/AMDbObject.h"
#include "database/AMDbObjectSupport.h"
AMLightweightScanInfo::AMLightweightScanInfo(int id,
		const QString &name,
		int number, const
		QDateTime &dateTime,
		const QString &scanType,
		const QString &runName,
		const QString &notes,
		const QString &sampleName,
		int thumbnailFirstId,
		int thumbnailCount)
{
	id_ = id;
	name_ = name;
	number_ = number;
	dateTime_ = dateTime;
	scanType_ = scanType;
	runName_ = runName;
	notes_ = notes;
	sampleName_ = sampleName;
	if(thumbnailCount == 0)
		return;
	for(int iThumbnailId = 0; iThumbnailId < thumbnailCount; iThumbnailId++)
	{
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

int AMLightweightScanInfo::number() const
{
	return number_;
}

QDateTime AMLightweightScanInfo::dateTime() const
{
	return dateTime_;
}

QString AMLightweightScanInfo::scanType() const
{
	return scanType_;
}

QString AMLightweightScanInfo::runName() const
{
	return runName_;
}

QString AMLightweightScanInfo::notes() const
{
	return notes_;
}

QString AMLightweightScanInfo::sampleName() const
{
	return sampleName_;
}

AMDbThumbnail *AMLightweightScanInfo::thumbnailAt(int index)
{
	/// Scan doesn't have this many thumbnails, return 0
	if(index >= thumbnailCount())
		return 0;

	/// Thumbnails hasn't been loaded yet, load
	if(!thumbnailsMap_.contains(thumbnailIds_.at(index)))
	{
		AMDatabase* db = AMDatabase::database("user");

		QSqlQuery query = db->select(AMDbObjectSupport::thumbnailTableName(), "type, title, subtitle, thumbnail", QString("id = %1").arg(thumbnailIds_.at(index)));
		if(query.exec())
		{
			while(query.next())
			{
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
		} else
		{
			query.finish();
		}

	}

	return thumbnailsMap_.value(thumbnailIds_.at(index));

}

int AMLightweightScanInfo::thumbnailCount()
{
	return thumbnailIds_.count();
}
