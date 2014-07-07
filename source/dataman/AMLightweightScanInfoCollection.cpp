#include "AMLightweightScanInfoCollection.h"
#include "database/AMDatabase.h"
#include "database/AMDbObjectSupport.h"
AMLightweightScanInfoCollection::AMLightweightScanInfoCollection(AMDatabase *database)
{
	database_ = database;
	populateSampleNames();
	populateRuns();
	populateObjectTypes();
	populateCollection();
}

void AMLightweightScanInfoCollection::populateSampleNames()
{
	// Building up the first section of the map, such that after this first part is done sampleNameMap_
	// will contain a key for each table name mentioned in the sample field of AMScan_table mapped to
	// an empty Hash (which in the second part will be filled with the sample id mapped to the sample name
	QSqlQuery selectQuery = database_->select("AMScan_table", "sample");

	if(!selectQuery.exec())
		return;

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

	for (int iSampleTable = 0; iSampleTable < tables.count(); iSampleTable++)
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

			sampleSelectQuery.finish();
		}
	}

}

void AMLightweightScanInfoCollection::populateObjectTypes()
{
	QSqlQuery selectQuery = database_->select("AMDbObjectTypes_table", "AMDbObjectType, description");

	if(!selectQuery.exec())
		return;

	while(selectQuery.next())
	{
		QString dbObjectName = selectQuery.value(0).toString();
		QString dbObjectDescription = selectQuery.value(1).toString();

		objectTypesMap_.insert(dbObjectName, dbObjectDescription);
	}

	selectQuery.finish();
}

void AMLightweightScanInfoCollection::populateCollection()
{
	QSqlQuery selectQuery = database_->select("AMScan_table", "*");

	if(!selectQuery.exec())
		return;

	while(selectQuery.next())
	{
		QSqlRecord currentRecord = selectQuery.record();
		int id = selectQuery.value(currentRecord.indexOf("id")).toInt();
		QString name = selectQuery.value(currentRecord.indexOf("name")).toString();
		int number = selectQuery.value(currentRecord.indexOf("number")).toInt();
		int thumbnailFirstId = selectQuery.value(currentRecord.indexOf("thumbnailFirstId")).toInt();
		int thumbnailCount = selectQuery.value(currentRecord.indexOf("thumbnailCount")).toInt();
		QDateTime dateTime = selectQuery.value(currentRecord.indexOf("dateTime")).toDateTime();
		QString runName = getRunName(selectQuery.value(currentRecord.indexOf("runId")).toInt());
		QString scanType = getScanType(selectQuery.value(currentRecord.indexOf("AMDbObjectType")).toString());
		QString notes = selectQuery.value(currentRecord.indexOf("notes")).toString();
		QString sampleName = getSampleName(selectQuery.value(currentRecord.indexOf("Sample")).toString());

		append(new AMLightweightScanInfo(id, name, number, dateTime, scanType, runName, notes, sampleName, thumbnailFirstId, thumbnailCount));

	}

	selectQuery.finish();
}

void AMLightweightScanInfoCollection::populateRuns()
{
	QSqlQuery selectQuery = database_->select("AMRun_table", "id, name");

	if(!selectQuery.exec())
		return;

	while(selectQuery.next())
	{
		int currentRunId = selectQuery.value(0).toInt();
		QString currentRunName = selectQuery.value(1).toString();

		runMap_.insert(currentRunId, currentRunName);
	}

	selectQuery.finish();
}

QString AMLightweightScanInfoCollection::getRunName(int runId)
{
	if (!runMap_.contains(runId) || runId < 1)
		return QString();

	return runMap_.value(runId);
}

QString AMLightweightScanInfoCollection::getScanType(const QString &dbObjectType)
{
	if(!objectTypesMap_.contains(dbObjectType) || dbObjectType.length() == 0)
		return QString();

	return objectTypesMap_.value(dbObjectType);
}

QString AMLightweightScanInfoCollection::getSampleName(const QString &sampleResult)
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

QUrl AMLightweightScanInfoCollection::getScanUrl(int index)
{
	AMLightweightScanInfo* scanAt = at(index);

	QString urlString = QString("amd://%1/%2/%3").arg(database_->connectionName()).arg(AMDbObjectSupport::s()->tableNameForClass("AMScan")).arg(scanAt->id());

	QUrl returnUrl(urlString);

	return returnUrl;
}
