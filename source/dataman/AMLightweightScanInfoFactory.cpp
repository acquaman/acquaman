#include "AMLightweightScanInfoFactory.h"

#include "AMLightweightScanInfoCollection.h"
#include "AMLightweightScanInfo.h"
#include "util/AMErrorMonitor.h"
#include "database/AMDatabase.h"
#include "database/AMDbObjectSupport.h"

QList<AMLightweightScanInfo *> AMLightweightScanInfoFactory::all(AMDatabase *database)
{

	AMLightweightScanInfoFactory helperFactory(database);

	helperFactory.populateExperimentIds();
	helperFactory.populateSamples();
	helperFactory.populateObjectTypes();
	helperFactory.populateRuns();

	return helperFactory.loadAll();

}

AMLightweightScanInfo *AMLightweightScanInfoFactory::single(int scanId, AMDatabase *database)
{
	AMLightweightScanInfo* returnScanInfo = 0;

	QSqlQuery selectQuery = database->select(
				AMDbObjectSupport::s()->tableNameForClass("AMScan"), "*",
				QString("id = %1").arg(scanId));


	if(selectQuery.exec()){
		while(selectQuery.next()) {
			QSqlRecord currentRecord = selectQuery.record();
			int id = selectQuery.value(currentRecord.indexOf("id")).toInt();
			QString name = selectQuery.value(currentRecord.indexOf("name")).toString();
			int number = selectQuery.value(currentRecord.indexOf("number")).toInt();
			int thumbnailFirstId = selectQuery.value(currentRecord.indexOf("thumbnailFirstId")).toInt();
			int thumbnailCount = selectQuery.value(currentRecord.indexOf("thumbnailCount")).toInt();
			QDateTime dateTime = selectQuery.value(currentRecord.indexOf("dateTime")).toDateTime();
			int runId = selectQuery.value(currentRecord.indexOf("runId")).toInt();
			QString runName = AMLightweightScanInfoFactory::loadRunName(database, runId);
			QString scanTypeClass = selectQuery.value(currentRecord.indexOf("AMDbObjectType")).toString();

			QString scanType = AMLightweightScanInfoFactory::loadScanType(database, scanTypeClass);
			QString notes = selectQuery.value(currentRecord.indexOf("notes")).toString();

			QString samplePre2013 = selectQuery.value(currentRecord.indexOf("samplePre2013")).toString();
			QString sampleTableEntry = selectQuery.value(currentRecord.indexOf("Sample")).toString();



			QString sampleName = AMLightweightScanInfoFactory::loadSampleName(database, samplePre2013, sampleTableEntry);

			returnScanInfo = new AMLightweightScanInfo(
						id, name, number, dateTime, scanType, runId, runName, notes,
						sampleName, thumbnailFirstId, thumbnailCount, database, 0);


			QList<int> experimentIds = AMLightweightScanInfoFactory::loadExperimentIds(
						database, scanId);
			for (int iExperimentId=0, count = experimentIds.count();
				 iExperimentId < count;
				 ++iExperimentId) {
				returnScanInfo->addExperimentId(experimentIds.at(iExperimentId));
			}
		}
	} else {
	AMErrorMon::alert(0, AMLIGHTWEIGHTSCANINFOFACTORY_SCANS_SQL_ERROR, QString("Could not complete query to load scans, with error: %1").arg(selectQuery.lastError().text()));

	}
	selectQuery.finish();
	return returnScanInfo;

}

QHash<int, QString> AMLightweightScanInfoFactory::runMap(AMDatabase *database)
{
	AMLightweightScanInfoFactory helperFactory(database);
	helperFactory.populateRuns();

	return helperFactory.runNameMap_;
}


void AMLightweightScanInfoFactory::updateSingle(AMLightweightScanInfo *info, AMDatabase *database)
{
	AMLightweightScanInfo* updatedScanInfo = AMLightweightScanInfoFactory::single(info->id(), database);

	info->setName(updatedScanInfo->name());
	info->setNumber(updatedScanInfo->number());
	info->setDateTime(updatedScanInfo->dateTime());
	info->setScanType(updatedScanInfo->scanType());
	info->setRunId(updatedScanInfo->runId());
	info->setRunName(updatedScanInfo->runName());
	info->setNotes(updatedScanInfo->notes());
	info->setSampleName(updatedScanInfo->sampleName());

	info->refreshThumbnails(updatedScanInfo->thumbnailFirstId(),
							updatedScanInfo->thumbnailCount());

	if(info->experimentIds().count() != updatedScanInfo->experimentIds().count()) {
		for(int iExperimentId = 0, experimentIdCount = updatedScanInfo->experimentIds().count();
			iExperimentId < experimentIdCount; ++iExperimentId) {
			if(!info->experimentIds().contains(updatedScanInfo->experimentIds().at(iExperimentId)))
				info->addExperimentId(updatedScanInfo->experimentIds().at(iExperimentId));
		}
	}
	updatedScanInfo->deleteLater();
}

int AMLightweightScanInfoFactory::scanIdForExperimentEntry(int id, AMDatabase *database)
{
	int returnScanId = -1;
	QSqlQuery selectQuery = database->select("ObjectExperimentEntries", "objectId",
											  QString("id = %1").arg(id));

	if(selectQuery.exec()) {
		while(selectQuery.next()) {
			QSqlRecord currentRecord = selectQuery.record();
			int scanId = currentRecord.value(0).toInt();
			returnScanId = scanId;
		}
	} else {
		AMErrorMon::alert(0, AMLIGHTWEIGHTSCANINFOFACTORY_EXPERIMENTS_SQL_ERROR, QString("Could not complete query to obtain the scanId for experiment entry, with error: %1").arg(selectQuery.lastError().text()));
	}
	selectQuery.finish();

	return returnScanId;
}

AMLightweightScanInfoFactory::AMLightweightScanInfoFactory()
{
}

AMLightweightScanInfoFactory::AMLightweightScanInfoFactory(AMDatabase *database)
{
	database_ = database;
}

QList<AMLightweightScanInfo*> AMLightweightScanInfoFactory::loadAll()
{
	QList<AMLightweightScanInfo*> returnCollection;

	QSqlQuery selectQuery = database_->select(AMDbObjectSupport::s()->tableNameForClass("AMScan"), "*");

	if(selectQuery.exec()) {
		while(selectQuery.next()) {
			QSqlRecord currentRecord = selectQuery.record();
			int id = selectQuery.value(currentRecord.indexOf("id")).toInt();
			QString name = selectQuery.value(currentRecord.indexOf("name")).toString();
			int number = selectQuery.value(currentRecord.indexOf("number")).toInt();
			int thumbnailFirstId = selectQuery.value(currentRecord.indexOf("thumbnailFirstId")).toInt();
			int thumbnailCount = selectQuery.value(currentRecord.indexOf("thumbnailCount")).toInt();
			QDateTime dateTime = selectQuery.value(currentRecord.indexOf("dateTime")).toDateTime();
			int runId = selectQuery.value(currentRecord.indexOf("runId")).toInt();
			QString runName = runNameMap_.value(runId, QString());
			QString scanTypeClass = selectQuery.value(currentRecord.indexOf("AMDbObjectType")).toString();

			QString scanType = objectTypesMap_.value(scanTypeClass, QString());
			QString notes = selectQuery.value(currentRecord.indexOf("notes")).toString();

			QString samplePre2013 = selectQuery.value(currentRecord.indexOf("samplePre2013")).toString();
			QString sampleTableEntry = selectQuery.value(currentRecord.indexOf("Sample")).toString();



			QString sampleName = getSampleName(samplePre2013, sampleTableEntry);

			AMLightweightScanInfo* currentLightweightScanInfo = new AMLightweightScanInfo(
						id, name, number, dateTime, scanType, runId, runName, notes,
						sampleName, thumbnailFirstId, thumbnailCount, database_, 0);


			QList<int> experimentIds = experimentMap_.value(id, QList<int>());
			for (int iExperimentId=0, count = experimentIds.count();
				 iExperimentId < count;
				 ++iExperimentId) {
				currentLightweightScanInfo->addExperimentId(experimentIds.at(iExperimentId));
			}
			returnCollection.append(currentLightweightScanInfo);
		}

	} else {
		AMErrorMon::alert(0, AMLIGHTWEIGHTSCANINFOFACTORY_SCANS_SQL_ERROR, QString("Could not complete query to load scans, with error: %1").arg(selectQuery.lastError().text()));
	}
	selectQuery.finish();

	return returnCollection;
}

void AMLightweightScanInfoFactory::populateExperimentIds()
{
	QSqlQuery selectQuery = database_->select("ObjectExperimentEntries", "objectId, ExperimentId");

	if(selectQuery.exec()) {
		while(selectQuery.next()) {
			int scanId = selectQuery.value(0).toInt();
			int experimentId = selectQuery.value(1).toInt();

			if(experimentMap_.contains(scanId)) {
				experimentMap_[scanId].append(experimentId);
			} else {
				QList<int> experimentIds;
				experimentIds.append(experimentId);
				experimentMap_.insert(scanId, experimentIds);
			}
		}
	} else {
		AMErrorMon::alert(0, AMLIGHTWEIGHTSCANINFOFACTORY_EXPERIMENTS_SQL_ERROR, QString("Could not complete query to load experiments, with error: %1").arg(selectQuery.lastError().text()));
	}
	selectQuery.finish();
}

void AMLightweightScanInfoFactory::populateSamples()
{
	// Building up the first section of the map, such that after this first part is done sampleNameMap_
	// will contain a key for each table name mentioned in the sample field of AMScan_table mapped to
	// an empty Hash (which in the second part will be filled with the sample id mapped to the sample name
	QSqlQuery selectQuery = database_->select(AMDbObjectSupport::s()->tableNameForClass("AMScan"), "sample", "sample IS NOT NULL");

	if(!selectQuery.exec()) {
		selectQuery.finish();
		return;
	}

	while(selectQuery.next()) {
		// Sample results are stored in format table_name;id
		QString sampleResults = selectQuery.value(0).toString();


		QStringList splitResults = sampleResults.split(";");

		if(!sampleMap_.contains(splitResults.at(0)))
			sampleMap_.insert(splitResults.at(0), QHash<int, QString>());
	}

	selectQuery.finish();

	// Second part. For each table mentioned in the sampleNameMap so far, we'll fill the Hash keyed
	// against it with all the sample ids and sample names it contains
	QList<QString> tables = sampleMap_.keys();

	for (int iSampleTable = 0, size = tables.count();
		 iSampleTable < size;
		 iSampleTable++) {
		QString tableName = tables.at(iSampleTable);

		QSqlQuery sampleSelectQuery = database_->select(tableName, "id, name");

		if(sampleSelectQuery.exec())
		{
			while(sampleSelectQuery.next())	{
				int sampleId = sampleSelectQuery.value(0).toInt();
				QString sampleName = sampleSelectQuery.value(1).toString();

				sampleMap_[tableName].insert(sampleId, sampleName);
			}

		} else {
			AMErrorMon::alert(0, AMLIGHTWEIGHTSCANINFOFACTORY_SAMPLES_SQL_ERROR, QString("Could not complete query to load samples, with error: %1").arg(sampleSelectQuery.lastError().text()));
		}
		sampleSelectQuery.finish();
	}
}

void AMLightweightScanInfoFactory::populateObjectTypes()
{
	QSqlQuery selectQuery = database_->select("AMDbObjectTypes_table", "AMDbObjectType, description");

	if(selectQuery.exec()) {
		while(selectQuery.next()) {
			QString dbObjectName = selectQuery.value(0).toString();
			QString dbObjectDescription = selectQuery.value(1).toString();

			objectTypesMap_.insert(dbObjectName, dbObjectDescription);
		}
	} else {
		AMErrorMon::alert(0, AMLIGHTWEIGHTSCANINFOFACTORY_OBJECT_TYPES_SQL_ERROR, QString("Could not complete query to load object types, with error: %1").arg(selectQuery.lastError().text()));
	}
	selectQuery.finish();
}

void AMLightweightScanInfoFactory::populateRuns()
{
	QSqlQuery selectQuery = database_->select(AMDbObjectSupport::s()->tableNameForClass("AMRun"), "id, name");

	if(selectQuery.exec()) {

		while(selectQuery.next()) {
			int currentRunId = selectQuery.value(0).toInt();
			QString currentRunName = selectQuery.value(1).toString();

			runNameMap_.insert(currentRunId, currentRunName);
		}
	} else {
		AMErrorMon::alert(0, AMLIGHTWEIGHTSCANINFOFACTORY_RUNS_SQL_ERROR, QString("Could not complete query to load runs, with error: %1").arg(selectQuery.lastError().text()));
	}

	selectQuery.finish();
}

QString AMLightweightScanInfoFactory::getSampleName(const QString &sampleNamePre2013, const QString &sampleTableEntry) const
{
	if(sampleTableEntry.length()==0)
		return sampleNamePre2013;

	QStringList splitResult = sampleTableEntry.split(";");

	if(splitResult.length() < 2)
		return QString();

	QString tableName = splitResult.at(0);
	bool parseSuccess = false;
	int id = splitResult.at(1).toInt(&parseSuccess);

	if(!parseSuccess)
		return QString();

	if(!sampleMap_.contains(tableName))
		return QString();

	if(!sampleMap_.value(tableName).contains(id))
		return QString();

	return sampleMap_.value(tableName).value(id);
}

QString AMLightweightScanInfoFactory::loadRunName(AMDatabase *database, int runId)
{
	QString returnName;

	if(runId > 0) {
		QSqlQuery selectQuery = database->select(
					AMDbObjectSupport::s()->tableNameForClass("AMRun"),
					"name", QString("id = %1").arg(runId));


		if(selectQuery.exec()) {
			while(selectQuery.next()) {
				returnName = selectQuery.value(0).toString();
			}
		} else {
			AMErrorMon::alert(0, AMLIGHTWEIGHTSCANINFOFACTORY_RUNS_SQL_ERROR, QString("Could not complete query to load run name with error: %1").arg(selectQuery.lastError().text()));
		}
	}
	return returnName;
}

QString AMLightweightScanInfoFactory::loadScanType(AMDatabase *database, const QString &className)
{
	QString returnName;

	QSqlQuery selectQuery = database->select(
				"AMDbObjectTypes_table",
				"description",
				QString("AMDbObjectType = '%1'").arg(className));


	if(selectQuery.exec()) {
		while(selectQuery.next()) {
			returnName = selectQuery.value(0).toString();
		}
	} else {
		AMErrorMon::alert(0, AMLIGHTWEIGHTSCANINFOFACTORY_OBJECT_TYPES_SQL_ERROR, QString("Could not complete query to load object types, with error: %1").arg(selectQuery.lastError().text()));
	}
	selectQuery.finish();
	return returnName;
}

QString AMLightweightScanInfoFactory::loadSampleName(AMDatabase *database,
													 const QString &sampleNamePre2013,
													 const QString &sampleTableEntry)
{
	QString returnValue;

	if(sampleNamePre2013 != "") {
		returnValue = sampleNamePre2013;
		return returnValue;
	}

	if(sampleTableEntry == "")
		return returnValue;


	QStringList splitResults = sampleTableEntry.split(";");

	if(splitResults.count() < 2)
		return returnValue;

	QString sampleTableName = splitResults.at(0);
	int id = splitResults.at(1).toInt();

	QSqlQuery selectQuery = database->select(sampleTableName,
											 "name",
											 QString("id = %1").arg(id));

	if(selectQuery.exec()) {
		while(selectQuery.next()) {
			returnValue = selectQuery.value(0).toString();
		}
	} else {
		AMErrorMon::alert(0, AMLIGHTWEIGHTSCANINFOFACTORY_SAMPLES_SQL_ERROR, QString("Could not complete query to load sample, with error: %1").arg(selectQuery.lastError().text()));
	}
	selectQuery.finish();

	return returnValue;
}

QList<int> AMLightweightScanInfoFactory::loadExperimentIds(AMDatabase *database, int scanId)
{
	QList<int> returnList;

	QSqlQuery selectQuery = database->select("ObjectExperimentEntries",
											 "ExperimentId",
											 QString("objectId = %1").arg(scanId));

	if(selectQuery.exec()) {
		while(selectQuery.next()) {
			int experimentId = selectQuery.value(0).toInt();
			returnList.append(experimentId);
		}
	} else {
		AMErrorMon::alert(0, AMLIGHTWEIGHTSCANINFOFACTORY_EXPERIMENTS_SQL_ERROR, QString("Could not complete query to load experiments, with error: %1").arg(selectQuery.lastError().text()));
	}
	selectQuery.finish();

	return returnList;
}
