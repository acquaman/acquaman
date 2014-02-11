#include "VESPERSDatabaseDuplicateEntryObject.h"

#include <QStringBuilder>
#include <QFile>
#include <QDebug>

VESPERSDatabaseDuplicateEntryObject::VESPERSDatabaseDuplicateEntryObject(AMDatabase *database, int scanId)
{
	database_ = database;
	scanId_ = scanId;
}

bool VESPERSDatabaseDuplicateEntryObject::fix()
{
	QSqlQuery query = database_->query();
	query.prepare(QString("SELECT name, fileFormat, filePath, analyzedDataSourcesConnections FROM AMScan_table WHERE id=%1").arg(scanId_));

	if (!database_->execQuery(query)){

		query.finish();
		return false;
	}

	while (query.next()){

		scanName_ = query.value(0).toString();
		fileFormat_ = query.value(1).toString();
		filePath_ = query.value(2).toString();
		analyzedDataSourceConnections_ = query.value(3).toString();
	}

	query.finish();

	query = database_->query();
	query.prepare(QString("SELECT id, id2 FROM AMScan_table_rawDataSources WHERE id1=%1").arg(scanId_));

	if (!database_->execQuery(query)){

		query.finish();
		return false;
	}

	while (query.next()){

		allRawDataSourcesAssociatedTableIds_ << query.value(0).toInt();
		allRawDataSources_ << query.value(1).toInt();
	}

	query.finish();

	query = database_->query();
	query.prepare(QString("SELECT id, id2, table2 FROM AMScan_table_analyzedDataSources WHERE id1=%1").arg(scanId_));

	if (!database_->execQuery(query)){

		query.finish();
		return false;
	}

	while (query.next()){

		allAnalysisBlocksAssociatedTableIds_ << query.value(0).toInt();
		allAnalysisBlocks_ << query.value(1).toInt();
		allAnalysisBlockTableNames_ << query.value(2).toString();
	}

	query.finish();

	query = database_->query();
	QString duplicateWhereClause = QString("id=%1").arg(allRawDataSources_.first());

	for (int i = 1, size = allRawDataSources_.size(); i < size; i++)
		duplicateWhereClause += QString(" OR id=%1").arg(allRawDataSources_.at(i));

	query.prepare("SELECT name FROM AMRawDataSource_table WHERE " % duplicateWhereClause);

	if (!AMDatabase::execQuery(query)){

		query.finish();
		return false;
	}

	while (query.next())
		allRawDataSourceNames_ << query.value(0).toString();

	query.finish();

	// Only go to N-1 because the last entry was already checked.
	for (int i = 0, size = allRawDataSources_.size(); i < size-1; i++)
		if (allRawDataSourceNames_.at(i) == allRawDataSourceNames_.at(i+1)){

			duplicateRawDataSources_ << allRawDataSources_.at(i+1);
			duplicateRawDataSourceAssociatedTableIds_ << allRawDataSourcesAssociatedTableIds_.at(i+1);
		}

	for (int i = 0, size = allAnalysisBlocks_.size(); i < size; i++)
		allAnalysisBlockNames_ << database_->retrieve(allAnalysisBlocks_.at(i), allAnalysisBlockTableNames_.at(i), "name").toString();

	// This is very specific to the current bug.  Being more thorough would likely be necessary in future versions.
	// Only go to N-2 because the last entry was already checked.
	for (int i = 0, size = allAnalysisBlocks_.size(); i < size-2; i++)
		if (allAnalysisBlockNames_.at(i) == allAnalysisBlockNames_.at(i+2)){

			duplicateAnalysisBlocks_ << allAnalysisBlocks_.at(i+2);
			duplicateAnalysisBlockTableNames_ << allAnalysisBlockTableNames_.at(i+2);
			duplicateAnalysisBlockAssociatedTableIds_ << allAnalysisBlocksAssociatedTableIds_.at(i+2);
		}

	foreach (int rawDataSourceIndex, duplicateRawDataSources_)
		dataSourceIndexToBeRemoved_ << (rawDataSourceIndex-allRawDataSources_.first());

	foreach (int analyzedDataSourceIndex, duplicateAnalysisBlockAssociatedTableIds_)
		dataSourceIndexToBeRemoved_ << (analyzedDataSourceIndex-allAnalysisBlocksAssociatedTableIds_.first()+allRawDataSources_.size());

	QStringList analyzedDataSourceConnections = analyzedDataSourceConnections_.split("\n");

	foreach (QString connection, analyzedDataSourceConnections){

		foreach (int id, dataSourceIndexToBeRemoved_)
			if (connection.contains(QString("%1").arg(id)))
				analyzedDataSourceConnectionsToBeRemoved_ << connection;

		if (!analyzedDataSourceConnectionsToBeRemoved_.contains(connection))
			analyzedDataSourceConnectionsToRemain_ << connection;
	}

	foreach (QString analyzedConnection, analyzedDataSourceConnectionsToRemain_){

		QStringList splitAnalyzedConnection = analyzedConnection.split(",");
		QStringList fixedAnalyzedConnection;

		foreach (QString splitPiece, splitAnalyzedConnection)
			fixedAnalyzedConnection << fixStringIndex(splitPiece, dataSourceIndexToBeRemoved_);

		correctedAnalyzedDataSourceConnectionsToRemain_ << fixedAnalyzedConnection.join(",");
	}

	fixedFileFormat_ = "vespers2012XRF1ElXRD";
	fixedFilePath_ = filePath_;
	fixedFilePath_.replace(".cdf", ".dat");
	fixedAdditionalFilePath_ = filePath_;
	fixedAdditionalFilePath_.replace(".cdf", "_spectra.dat");

	database_->startTransaction();

	if (!database_->update(scanId_, "AMScan_table", "analyzedDataSourcesConnections", QVariant(correctedAnalyzedDataSourceConnectionsToRemain_.join("\n")))){

		qDebug() << "Failed to update row" << scanId_ << "from AMScan_table";
		database_->rollbackTransaction();
		return false;
	}

	if (!database_->update(scanId_, "AMScan_table", "fileFormat", QVariant(fixedFileFormat_))){

		qDebug() << "Failed to update row" << scanId_ << "from AMScan_table";
		database_->rollbackTransaction();
		return false;
	}

	if (!database_->update(scanId_, "AMScan_table", "filePath", QVariant(fixedFilePath_))){

		qDebug() << "Failed to update row" << scanId_ << "from AMScan_table";
		database_->rollbackTransaction();
		return false;
	}

	if (!database_->update(scanId_, "AMScan_table", "additionalFilePaths", QVariant(fixedAdditionalFilePath_))){

		qDebug() << "Failed to update row" << scanId_ << "from AMScan_table";
		database_->rollbackTransaction();
		return false;
	}

	foreach (int id, duplicateRawDataSources_)
		if (!database_->deleteRow(id, "AMRawDataSource_table")){

			qDebug() << "Failed to delete row" << id << "from AMRawDataSource_table";
			database_->rollbackTransaction();
			return false;
		}

	foreach (int id, duplicateRawDataSourceAssociatedTableIds_)
		if (!database_->deleteRow(id, "AMScan_table_rawDataSources")){

			qDebug() << "Failed to delete row" << id << "from AMScan_table_rawDataSources";
			database_->rollbackTransaction();
			return false;
		}

	for (int i = 0, size = duplicateAnalysisBlocks_.size(); i < size; i++)
		if (!database_->deleteRow(duplicateAnalysisBlocks_.at(i), duplicateAnalysisBlockTableNames_.at(i))){

			qDebug() << "Failed to delete row" << duplicateAnalysisBlocks_.at(i) << "from" << duplicateAnalysisBlockTableNames_.at(i);
			database_->rollbackTransaction();
			return false;
		}

	foreach (int id, duplicateAnalysisBlockAssociatedTableIds_)
		if (!database_->deleteRow(id, "AMScan_table_analyzedDataSources")){

			qDebug() << "Failed to delete row" << id << "from AMScan_table_analyzedDataSources";
			database_->rollbackTransaction();
			return false;
		}

	database_->commitTransaction();

	QString path = database_->dbAccessString();
	path.remove("userdata.db");
	QFile::remove(path % filePath_);

//	qDebug() << scanName_;
//	qDebug() << fileFormat_;
//	qDebug() << filePath_;
//	qDebug() << analyzedDataSourceConnections_;
//	qDebug() << allRawDataSources_;
//	qDebug() << allRawDataSourcesAssociatedTableIds_;
//	qDebug() << allAnalysisBlocks_;
//	qDebug() << allAnalysisBlocksAssociatedTableIds_;
//	qDebug() << allAnalysisBlockTableNames_;
//	qDebug() << allRawDataSources_.size() << allAnalysisBlocks_.size();
//	qDebug() << allRawDataSourceNames_;
//	qDebug() << duplicateRawDataSources_;
//	qDebug() << duplicateRawDataSourceAssociatedTableIds_;
//	qDebug() << allAnalysisBlockNames_;
//	qDebug() << duplicateAnalysisBlocks_;
//	qDebug() << duplicateAnalysisBlockAssociatedTableIds_;
//	qDebug() << dataSourceIndexToBeRemoved_;
//	qDebug() << analyzedDataSourceConnectionsToBeRemoved_;
//	qDebug() << analyzedDataSourceConnectionsToRemain_;
//	qDebug() << correctedAnalyzedDataSourceConnectionsToRemain_;
//	qDebug() << fixedFileFormat_;
//	qDebug() << fixedFilePath_;
//	qDebug() << fixedAdditionalFilePath_;
//	qDebug() << QFile::exists(path % filePath_);

	qDebug() << "Successfully fixed" << scanName_;

	return true;
}

QString VESPERSDatabaseDuplicateEntryObject::fixStringIndex(const QString &index, QList<int> indexList) const
{
	int id = index.toInt();
	int newId = -1;

	QList<int> fullList = QList<int>() << 0 << indexList;

	for (int i = 0, size = fullList.size(); i < size-1; i++){

		if (id > fullList.at(i) && id <= fullList.at(i+1))
			newId = id - i;

		else if (id > fullList.at(i+1))
			newId = id - (i+1);
	}

	return QString("%1").arg(newId);
}
