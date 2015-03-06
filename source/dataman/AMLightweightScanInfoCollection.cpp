#include "AMLightweightScanInfoCollection.h"

#include "AMLightweightScanInfoFactory.h"
#include "database/AMDatabase.h"
#include "database/AMDbObjectSupport.h"

AMLightweightScanInfoCollection::AMLightweightScanInfoCollection(AMDatabase *database)
{
	database_ = database;
	scanInfos_ = AMLightweightScanInfoFactory::all(database_);
}

QUrl AMLightweightScanInfoCollection::getScanUrl(int id) const
{
	QString urlString =
			QString("amd://%1/%2/%3")
				.arg(database_->connectionName())
				.arg(AMDbObjectSupport::s()->tableNameForClass("AMScan"))
				.arg(id);
	QUrl returnUrl(urlString);

	return returnUrl;
}

AMLightweightScanInfo *AMLightweightScanInfoCollection::at(int index) const
{
	return scanInfos_.at(index);
}

int AMLightweightScanInfoCollection::count() const
{
	return scanInfos_.count();
}

int AMLightweightScanInfoCollection::indexOf(AMLightweightScanInfo *scan, int from) const
{
	return scanInfos_.indexOf(scan, from);
}

const QHash<int, QString> AMLightweightScanInfoCollection::runMap()
{
	// Todo: Figure out where this should be stored
	return QHash<int, QString>();
}

void AMLightweightScanInfoCollection::onDbItemAdded(const QString &tableName, int id)
{

}

void AMLightweightScanInfoCollection::onDbItemUpdated(const QString &tableName, int id)
{

}

void AMLightweightScanInfoCollection::onDbItemRemoved(const QString &tableName, int oldId)
{

}

