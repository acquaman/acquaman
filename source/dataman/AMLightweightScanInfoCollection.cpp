#include "AMLightweightScanInfoCollection.h"

#include "AMLightweightScanInfoFactory.h"
#include "database/AMDatabase.h"
#include "database/AMDbObjectSupport.h"

AMLightweightScanInfoCollection::AMLightweightScanInfoCollection(AMDatabase *database) {
	database_ = database;
	connect(database_, SIGNAL(created(QString,int)), this, SLOT(onDbItemAdded(QString,int)));
	connect(database_, SIGNAL(updated(QString,int)), this, SLOT(onDbItemUpdated(QString,int)));
	connect(database_, SIGNAL(removed(QString,int)), this, SLOT(onDbItemRemoved(QString,int)));

	scanInfos_ = AMLightweightScanInfoFactory::all(database_);
}

QUrl AMLightweightScanInfoCollection::getScanUrl(int id) const {
	QString urlString =
			QString("amd://%1/%2/%3")
				.arg(database_->connectionName())
				.arg(AMDbObjectSupport::s()->tableNameForClass("AMScan"))
				.arg(id);
	QUrl returnUrl(urlString);

	return returnUrl;
}

AMLightweightScanInfo *AMLightweightScanInfoCollection::at(int index) const {
	return scanInfos_.at(index);
}

int AMLightweightScanInfoCollection::count() const {
	return scanInfos_.count();
}

int AMLightweightScanInfoCollection::indexOf(AMLightweightScanInfo *scan, int from) const {
	return scanInfos_.indexOf(scan, from);
}

const QHash<int, QString> AMLightweightScanInfoCollection::runMap() {
	// Todo: Figure out where this should be stored
	return QHash<int, QString>();
}

void AMLightweightScanInfoCollection::onDbItemAdded(const QString &tableName, int id) {
	if(tableName == AMDbObjectSupport::s()->tableNameForClass("AMScan")) {

		emit scanAboutToBeAdded(scanInfos_.count() - 2);
		AMLightweightScanInfo* addedScanInfo = AMLightweightScanInfoFactory::single(id, database_);

		if(addedScanInfo) {
			scanInfos_.append(addedScanInfo);
		}
		emit scanAdded();
	}
}

void AMLightweightScanInfoCollection::onDbItemUpdated(const QString &tableName, int id) {
	if(tableName == AMDbObjectSupport::s()->tableNameForClass("AMScan")) {
		int dirtyScanInfoIndex = indexOfScanWithId(id);
		if(dirtyScanInfoIndex >= 0) {
			AMLightweightScanInfo* dirtyScanInfo = scanInfos_.at(dirtyScanInfoIndex);
			if(dirtyScanInfo->thumbnailCount() > 0) {
				emit scanThumbnailAboutToBeRemoved(dirtyScanInfoIndex, 0, dirtyScanInfo->thumbnailCount()-1);
				emit scanThumbnailRemoved();
			}

			AMLightweightScanInfoFactory::updateSingle(dirtyScanInfo, database_);

			if(dirtyScanInfo->thumbnailCount() > 0) {
				emit scanThumbnailAboutToBeAdded();
				emit scanThumbnailAdded();
			}
			emit scanUpdated(dirtyScanInfoIndex);
		}
	}
}

void AMLightweightScanInfoCollection::onDbItemRemoved(const QString &tableName, int oldId) {
	if(tableName == AMDbObjectSupport::s()->tableNameForClass("AMScan")) {
		int scanInfoToBeRemovedIndex = indexOfScanWithId(oldId);
		if(scanInfoToBeRemovedIndex >= 0) {
			emit scanAboutToBeRemoved(scanInfoToBeRemovedIndex);
			scanInfos_.removeAt(scanInfoToBeRemovedIndex);
			emit scanRemoved();
		}
	}
}

int AMLightweightScanInfoCollection::indexOfScanWithId(int id) {
	for(int iScanInfo =0, scanInfoCount = scanInfos_.count();
		iScanInfo < scanInfoCount;
		++iScanInfo) {
		AMLightweightScanInfo* currentScanInfo = scanInfos_.at(iScanInfo);
		if(currentScanInfo->id() == id)
			return iScanInfo;
	}

	return -1;
}

