#include "AMLightweightScanInfoCollection.h"
#include "database/AMDatabase.h"
#include "database/AMDbObjectSupport.h"
#include "util/AMErrorMonitor.h"
AMLightweightScanInfoCollection::AMLightweightScanInfoCollection(AMDatabase *database)
{
	lastUpdatedScanId_ = -1;
	database_ = database;
	populateSampleNames();
	populateRuns();
	populateObjectTypes();
	populateExperimentIds();
	populateCollection();
	connect(database_, SIGNAL(created(QString,int)), this, SLOT(onDbItemAdded(QString,int)));
	connect(database_, SIGNAL(removed(QString,int)), this, SLOT(onDbItemRemoved(QString,int)));
	connect(database_, SIGNAL(updated(QString,int)), this, SLOT(onDbItemUpdated(QString,int)));
}

QUrl AMLightweightScanInfoCollection::getScanUrl(int id) const
{

	QString urlString = QString("amd://%1/%2/%3").arg(database_->connectionName()).arg(AMDbObjectSupport::s()->tableNameForClass("AMScan")).arg(id);

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

void AMLightweightScanInfoCollection::populateExperimentIds()
{
	QSqlQuery selectQuery = database_->select("ObjectExperimentEntries", "objectId, ExperimentId");

	if(!selectQuery.exec())
	{
		selectQuery.finish();
		return;
	}

	while(selectQuery.next())
	{
		int objectId = selectQuery.value(0).toInt();
		int experimentId = selectQuery.value(1).toInt();

		experimentIdMap_.insert(objectId, experimentId);
	}
}

void AMLightweightScanInfoCollection::populateSampleNames()
{
	// Building up the first section of the map, such that after this first part is done sampleNameMap_
	// will contain a key for each table name mentioned in the sample field of AMScan_table mapped to
	// an empty Hash (which in the second part will be filled with the sample id mapped to the sample name
	QSqlQuery selectQuery = database_->select(AMDbObjectSupport::s()->tableNameForClass("AMScan"), "sample", "sample IS NOT NULL");

	if(!selectQuery.exec())
	{
		selectQuery.finish();
		return;
	}

	while(selectQuery.next())
	{
		// Sample results are stored in format table_name;id
		QString sampleResults = selectQuery.value(0).toString();


		QStringList splitResults = sampleResults.split(";");

		if(!sampleNameMap_.contains(splitResults.at(0)))
			sampleNameMap_.insert(splitResults.at(0), QHash<int, QString>());


	}

	selectQuery.finish();

	// Second part. For each table mentioned in the sampleNameMap so far, we'll fill the Hash keyed
	// against it with all the sample ids and sample names it contains
	QList<QString> tables = sampleNameMap_.keys();

	for (int iSampleTable = 0, size = tables.count(); iSampleTable < size; iSampleTable++)
	{
		QString tableName = tables.at(iSampleTable);

		QSqlQuery sampleSelectQuery = database_->select(tableName, "id, name");

		if(sampleSelectQuery.exec())
		{
			while(sampleSelectQuery.next())
			{
				int sampleId = sampleSelectQuery.value(0).toInt();
				QString sampleName = sampleSelectQuery.value(1).toString();

				sampleNameMap_[tableName].insert(sampleId, sampleName);
			}

		}
		else
		{
			AMErrorMon::alert(this, AMLIGHTWEIGHTSCANINFOCOLLECTION_SAMPLES_SQL_ERROR, QString("Could not complete query to load samples, with error: %1").arg(sampleSelectQuery.lastError().text()));
		}
		sampleSelectQuery.finish();
	}

}

void AMLightweightScanInfoCollection::populateSingleSample(const QString& tableName, int id)
{
	QSqlQuery selectQuery = database_->select(tableName, "id, name", QString("id = %1").arg(id));

	if(selectQuery.exec())
	{			
		while(selectQuery.next())
		{
			int sampleId = selectQuery.value(0).toInt();
			QString sampleName = selectQuery.value(1).toString();

			sampleNameMap_[tableName].insert(sampleId, sampleName);
		}
	}
	else
	{
		AMErrorMon::alert(this, AMLIGHTWEIGHTSCANINFOCOLLECTION_SAMPLES_SQL_ERROR, QString("Could not complete query to load single sample, with error: %1").arg(selectQuery.lastError().text()));
	}
	selectQuery.finish();
}

void AMLightweightScanInfoCollection::populateObjectTypes()
{
	QSqlQuery selectQuery = database_->select("AMDbObjectTypes_table", "AMDbObjectType, description");

	if(selectQuery.exec())
	{
		while(selectQuery.next())
		{
			QString dbObjectName = selectQuery.value(0).toString();
			QString dbObjectDescription = selectQuery.value(1).toString();

			objectTypesMap_.insert(dbObjectName, dbObjectDescription);
		}
	}
	else
	{
		AMErrorMon::alert(this, AMLIGHTWEIGHTSCANINFOCOLLECTION_OBJECT_TYPES_SQL_ERROR, QString("Could not complete query to load object types, with error: %1").arg(selectQuery.lastError().text()));
	}
	selectQuery.finish();
}

void AMLightweightScanInfoCollection::populateCollection()
{
	QSqlQuery selectQuery = database_->select(AMDbObjectSupport::s()->tableNameForClass("AMScan"), "*");

	if(selectQuery.exec())
	{		
		while(selectQuery.next())
		{
			QSqlRecord currentRecord = selectQuery.record();
			int id = selectQuery.value(currentRecord.indexOf("id")).toInt();
			QString name = selectQuery.value(currentRecord.indexOf("name")).toString();
			int number = selectQuery.value(currentRecord.indexOf("number")).toInt();
			int thumbnailFirstId = selectQuery.value(currentRecord.indexOf("thumbnailFirstId")).toInt();
			int thumbnailCount = selectQuery.value(currentRecord.indexOf("thumbnailCount")).toInt();
			QDateTime dateTime = selectQuery.value(currentRecord.indexOf("dateTime")).toDateTime();
			int runId = selectQuery.value(currentRecord.indexOf("runId")).toInt();
			int experimentId = getExperimentId(id);
			QString runName = getRunName(runId);
			QString scanType = getScanType(selectQuery.value(currentRecord.indexOf("AMDbObjectType")).toString());
			QString notes = selectQuery.value(currentRecord.indexOf("notes")).toString();
			QString sampleName = getSampleName(selectQuery.value(currentRecord.indexOf("Sample")).toString());

			scanInfos_.append(new AMLightweightScanInfo(id, name, number, dateTime, scanType, runId, runName, notes, sampleName, thumbnailFirstId, thumbnailCount, database_, experimentId, this));
		}

	}
	else
	{
		AMErrorMon::alert(this, AMLIGHTWEIGHTSCANINFOCOLLECTION_SCANS_SQL_ERROR, QString("Could not complete query to load scans, with error: %1").arg(selectQuery.lastError().text()));
	}
	selectQuery.finish();
}

void AMLightweightScanInfoCollection::populateSingleScanInfo(int id)
{
	QSqlQuery selectQuery = database_->select(AMDbObjectSupport::s()->tableNameForClass("AMScan"), "*", QString("id = %1").arg(id));

	if(selectQuery.exec())
	{
		while(selectQuery.next())
		{
			QSqlRecord currentRecord = selectQuery.record();
			int id = selectQuery.value(currentRecord.indexOf("id")).toInt();
			QString name = selectQuery.value(currentRecord.indexOf("name")).toString();
			int number = selectQuery.value(currentRecord.indexOf("number")).toInt();
			int thumbnailFirstId = selectQuery.value(currentRecord.indexOf("thumbnailFirstId")).toInt();
			int thumbnailCount = selectQuery.value(currentRecord.indexOf("thumbnailCount")).toInt();
			QDateTime dateTime = selectQuery.value(currentRecord.indexOf("dateTime")).toDateTime();
			int runId = selectQuery.value(currentRecord.indexOf("runId")).toInt();
			int experimentId = getExperimentId(id);
			QString runName = getRunName(runId);
			QString scanType = getScanType(selectQuery.value(currentRecord.indexOf("AMDbObjectType")).toString());
			QString notes = selectQuery.value(currentRecord.indexOf("notes")).toString();
			QString sampleName = getSampleName(selectQuery.value(currentRecord.indexOf("Sample")).toString());

			scanInfos_.append(new AMLightweightScanInfo(id, name, number, dateTime, scanType, runId, runName, notes, sampleName, thumbnailFirstId, thumbnailCount, database_, experimentId, this));
		}
	}
	else
	{
		AMErrorMon::alert(this, AMLIGHTWEIGHTSCANINFOCOLLECTION_SCANS_SQL_ERROR, QString("Could not complete query to load single scan, with error: %1").arg(selectQuery.lastError().text()));
	}
	selectQuery.finish();
}

void AMLightweightScanInfoCollection::updateSingleScanInfo(AMLightweightScanInfo *scanInfo)
{
	QSqlQuery selectQuery = database_->select(AMDbObjectSupport::s()->tableNameForClass("AMScan"), "*", QString("id = %1").arg(scanInfo->id()));

	if(selectQuery.exec())
	{			
		while(selectQuery.next())
		{
			QSqlRecord currentRecord = selectQuery.record();
			QString name = selectQuery.value(currentRecord.indexOf("name")).toString();
			int number = selectQuery.value(currentRecord.indexOf("number")).toInt();
			QDateTime dateTime = selectQuery.value(currentRecord.indexOf("dateTime")).toDateTime();
			int runId = selectQuery.value(currentRecord.indexOf("runId")).toInt();
			QString runName = getRunName(runId);
			QString scanType = getScanType(selectQuery.value(currentRecord.indexOf("AMDbObjectType")).toString());
			QString notes = selectQuery.value(currentRecord.indexOf("notes")).toString();
			QString sampleName = getSampleName(selectQuery.value(currentRecord.indexOf("Sample")).toString());

			scanInfo->setName(name);
			scanInfo->setDateTime(dateTime);
			scanInfo->setNotes(notes);
			scanInfo->setNumber(number);
			scanInfo->setRunId(runId);
			scanInfo->setRunName(runName);
			scanInfo->setSampleName(sampleName);
			scanInfo->setScanType(scanType);
		}
	}
	else
	{
		AMErrorMon::alert(this, AMLIGHTWEIGHTSCANINFOCOLLECTION_SCANS_SQL_ERROR, QString("Could not complete query to load single scan, with error: %1").arg(selectQuery.lastError().text()));
	}
	selectQuery.finish();
}

void AMLightweightScanInfoCollection::populateRuns()
{
	QSqlQuery selectQuery = database_->select(AMDbObjectSupport::s()->tableNameForClass("AMRun"), "id, name");

	if(selectQuery.exec())
	{

		while(selectQuery.next())
		{
			int currentRunId = selectQuery.value(0).toInt();
			QString currentRunName = selectQuery.value(1).toString();

			runMap_.insert(currentRunId, currentRunName);
		}
	}
	else
	{
		AMErrorMon::alert(this, AMLIGHTWEIGHTSCANINFOCOLLECTION_RUNS_SQL_ERROR, QString("Could not complete query to load runs, with error: %1").arg(selectQuery.lastError().text()));
	}
	selectQuery.finish();
}

void AMLightweightScanInfoCollection::populateSingleRun(int id)
{
	QSqlQuery selectQuery = database_->select(AMDbObjectSupport::s()->tableNameForClass("AMRun"), "id, name", QString("id = %1").arg(id));

	if(selectQuery.exec())
	{

		while(selectQuery.next())
		{
			int currentRunId = selectQuery.value(0).toInt();
			QString currentRunName = selectQuery.value(1).toString();

			runMap_.insert(currentRunId, currentRunName);
		}
	}
	else
	{
		AMErrorMon::alert(this, AMLIGHTWEIGHTSCANINFOCOLLECTION_RUNS_SQL_ERROR, QString("Could not complete query to load single run, with error: %1").arg(selectQuery.lastError().text()));
	}
	selectQuery.finish();
}

QString AMLightweightScanInfoCollection::getRunName(int runId) const
{
	if (!runMap_.contains(runId) || runId < 1)
		return QString();

	return runMap_.value(runId);
}

QString AMLightweightScanInfoCollection::getScanType(const QString &dbObjectType) const
{
	if(!objectTypesMap_.contains(dbObjectType) || dbObjectType.length() == 0)
		return QString();

	return objectTypesMap_.value(dbObjectType);
}

QString AMLightweightScanInfoCollection::getSampleName(const QString &sampleResult) const
{
	if(sampleResult.length() == 0)
		return QString();

	QStringList splitResult = sampleResult.split(";");

	if(splitResult.length() < 2)
		return QString();

	QString tableName = splitResult.at(0);
	bool parseSuccess = false;
	int id = splitResult.at(1).toInt(&parseSuccess);

	if(!parseSuccess)
		return QString();

	if(!sampleNameMap_.contains(tableName))
		return QString();

	if(!sampleNameMap_.value(tableName).contains(id))
		return QString();

	return sampleNameMap_.value(tableName).value(id);
}

int AMLightweightScanInfoCollection::getExperimentId(int scanId) const
{
	return experimentIdMap_.value(scanId, -1);
}

int AMLightweightScanInfoCollection::getScanIdFromThumbnailId(int thumbnailId)
{
	QSqlQuery selectQuery = database_->select("AMDbObjectThumbnails_table", "objectId, objectTableName", QString("id = %1").arg(thumbnailId));
	int scanId = -1;
	if(selectQuery.exec())
	{

		while(selectQuery.next())
		{
			int objectId = selectQuery.value(0).toInt();
			QString objectTableName = selectQuery.value(1).toString();

			if(objectTableName == AMDbObjectSupport::s()->tableNameForClass("AMScan"))
			scanId = objectId;
		}
	}
	else
	{
		AMErrorMon::alert(this, AMLIGHTWEIGHTSCANINFOCOLLECTION_RUNS_SQL_ERROR, QString("Could not complete query to load single run, with error: %1").arg(selectQuery.lastError().text()));
	}
	selectQuery.finish();
	return scanId;
}


void AMLightweightScanInfoCollection::onDbItemAdded(const QString &tableName, int id)
{

	if(tableName == AMDbObjectSupport::s()->tableNameForClass("AMScan"))
	{
		// A scan has been added. We need to load its scan info, append it to the collection, emitting
		// the relevant signals
		int newIndex = scanInfos_.count();
		emit scanAboutToBeAdded(newIndex);
		populateSingleScanInfo(id);
		emit scanAdded();
	}
	else if(tableName == AMDbObjectSupport::s()->tableNameForClass("AMRun"))
	{
		populateSingleRun(id);
	}
	else if(tableName == "AMDbObjectThumbnails_table")
	{
		// While thumbnails are removed in bulk (see onDbItemRemoved), they are added back
		// one at a time. However, for optimization reasons we still assume that
		// the scan whose thumbnails are being added is the last one which was updated (lastUpdatedScanId_)

		AMLightweightScanInfo* matchedScanInfo = 0;
		int lightweightScanInfoIndex = -1;
		bool lightweightScanInfoFound = false;
		for (int iScanInfo = 0, size = scanInfos_.count(); iScanInfo < size && !lightweightScanInfoFound; iScanInfo++)
		{
			if(scanInfos_.at(iScanInfo)->id() == lastUpdatedScanId_)
			{
				lightweightScanInfoIndex = iScanInfo;
				matchedScanInfo = scanInfos_.at(iScanInfo);
				lightweightScanInfoFound = true;
			}
		}

		if(!matchedScanInfo)
			return;

		emit scanThumbnailAboutToBeAdded();
		matchedScanInfo->addThumbnailId(id);
		emit scanThumbnailAdded();
		emit scanUpdated(lightweightScanInfoIndex);
	}
	else if(sampleNameMap_.keys().contains(tableName))
	{
		populateSingleSample(tableName, id);
	}
}

void AMLightweightScanInfoCollection::onDbItemUpdated(const QString &tableName, int id)
{
	if(tableName == AMDbObjectSupport::s()->tableNameForClass("AMScan"))
	{
		// A scan has been changed. We need to find the scan that matches the id, update the corresponding
		// scan info and emit our updated signal
		AMLightweightScanInfo* matchedScanInfo = 0;
		int lightweightScanInfoIndex = -1;
		bool lightweightScanInfoFound = false;
		for (int iScanInfo = 0, size = scanInfos_.count(); iScanInfo < size && !lightweightScanInfoFound; iScanInfo++)
		{
			if(scanInfos_.at(iScanInfo)->id() == id)
			{
				lightweightScanInfoIndex = iScanInfo;
				matchedScanInfo = scanInfos_.at(iScanInfo);
				lightweightScanInfoFound = true;
			}
		}

		if(!matchedScanInfo)
			return;

		updateSingleScanInfo(matchedScanInfo);
		lastUpdatedScanId_ = id;
		emit scanUpdated(lightweightScanInfoIndex);
	}
	else if(tableName == AMDbObjectSupport::s()->tableNameForClass("AMRun"))
	{
		// A run name has changed. We need to update our map, then iterate through all the infos, changing
		// the run names of those that are in this run, emitting updated signals as we go
		runMap_.remove(id);
		populateSingleRun(id);
		for (int iScanInfo = 0, size = scanInfos_.count(); iScanInfo < size; iScanInfo++)
		{
			if(scanInfos_.at(iScanInfo)->runId() == id)
			{
				scanInfos_.at(iScanInfo)->setRunName(getRunName(id));
				emit scanUpdated(iScanInfo);
			}
		}
	}
	else if(sampleNameMap_.keys().contains(tableName))
	{
		// A sample name has changed. We need to update our map, then iterate through all the infos, changing
		// the sample names of those that use this sample, emitting updated signals as we go
		QString oldSampleName = sampleNameMap_.value(tableName).value(id);
		sampleNameMap_[tableName].remove(id);
		populateSingleSample(tableName, id);

		for (int iScanInfo = 0, size = scanInfos_.count(); iScanInfo < size; iScanInfo++)
		{
			if(scanInfos_.at(iScanInfo)->sampleName() == oldSampleName)
			{
				scanInfos_.at(iScanInfo)->setSampleName(sampleNameMap_.value(tableName).value(id));
				emit scanUpdated(iScanInfo);
			}
		}
	}
}

void AMLightweightScanInfoCollection::onDbItemRemoved(const QString &tableName, int oldId)
{
	if(tableName == AMDbObjectSupport::s()->tableNameForClass("AMScan"))
	{
		// A scan has been removed from the database. We need to find the relevant scan, remove it
		// from our collection while emitting the relevant removed signals
		AMLightweightScanInfo* matchedScanInfo = 0;
		int lightweightScanInfoIndex = -1;
		bool lightweightScanInfoFound = false;
		for (int iScanInfo = 0, size = scanInfos_.count(); iScanInfo < size && !lightweightScanInfoFound; iScanInfo++)
		{
			if(scanInfos_.at(iScanInfo)->id() == oldId)
			{
				lightweightScanInfoIndex = iScanInfo;
				matchedScanInfo = scanInfos_.at(iScanInfo);
				lightweightScanInfoFound = true;
			}
		}

		if(!matchedScanInfo)
			return;

		emit scanAboutToBeRemoved(lightweightScanInfoIndex);
		scanInfos_.removeAt(lightweightScanInfoIndex);
		emit scanRemoved();

	}
	else if(tableName == "AMDbObjectThumbnails_table")
	{
		// Because thumbnails are deleted in bulk, we only ever receive a -1 here for the oldId. As such
		// a record is kept of the id of the last updated scan (scans are updated prior to having their
		// thumbnail's removed). Here we'll need to signal that all of the thumbnails have been removed
		// for the lastUpdatedScanId
		if(lastUpdatedScanId_ == -1)
			return;


		AMLightweightScanInfo* matchedScanInfo = 0;
		bool scanInfoFound = false;
		int iScanInfo = 0;
		for(int scanInfoCount = scanInfos_.count(); iScanInfo < scanInfoCount && !scanInfoFound; iScanInfo++)
		{
			if(scanInfos_.at(iScanInfo)->id() == lastUpdatedScanId_)
			{
				matchedScanInfo = scanInfos_.at(iScanInfo);
				scanInfoFound = true;
			}
		}

		if(!scanInfoFound)
			return;

		emit scanThumbnailAboutToBeRemoved(iScanInfo, 0, matchedScanInfo->thumbnailCount()-1);
		matchedScanInfo->clearThumbnails();
		emit scanThumbnailRemoved();
		emit scanUpdated(iScanInfo);
	}
}



