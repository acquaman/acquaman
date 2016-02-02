/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


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
	query.prepare(QString("SELECT name, thumbnailCount, fileFormat, filePath, analyzedDataSourcesConnections FROM AMScan_table WHERE id=%1").arg(scanId_));

	if (!database_->execQuery(query)){

		query.finish();
		return false;
	}

	while (query.next()){

		scanName_ = query.value(0).toString();
		thumbnailCount_ = query.value(1).toInt();
		fileFormat_ = query.value(2).toString();
		filePath_ = query.value(3).toString();
		analyzedDataSourceConnections_ = query.value(4).toString();
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

	if (!database_->execQuery(query)){

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

	query = database_->query();
	query.prepare(QString("SELECT id, number, subtitle FROM AMDbObjectThumbnails_table WHERE objectId=%1 AND objectTableName='AMScan_table'").arg(scanId_));

	if (!database_->execQuery(query)){

		query.finish();
		return false;
	}

	while (query.next()){

		allThumbnailIds_ << query.value(0).toInt();
		allThumbnailNumbers_ << query.value(1).toInt();
		allThumbnailNames_ << query.value(2).toString();
	}

	query.finish();

	// Only go to N-1 because the last entry was already checked.
	for (int i = 0, size = allThumbnailIds_.size(); i < size-1; i++)
		if (allThumbnailNames_.at(i) == allThumbnailNames_.at(i+1)){

			duplicateThumbnailIds_ << allThumbnailIds_.at(i+1);
			duplicateThumbnailNumbers_ << allThumbnailNumbers_.at(i+1);
		}

	correctedThumbnailIds_ = allThumbnailIds_;

	foreach (int id, duplicateThumbnailIds_)
		correctedThumbnailIds_.removeOne(id);

	correctedThumbnailNumbers_ = allThumbnailNumbers_;

	foreach (int id, duplicateThumbnailNumbers_)
		correctedThumbnailNumbers_.removeOne(id);

	for (int i = 0, size = correctedThumbnailNumbers_.size(); i < size; i++)
		correctedThumbnailNumbers_[i] = fixIntIndex(correctedThumbnailNumbers_.at(i), duplicateThumbnailNumbers_);

	// The corrected thumbnail count correction is due to explicit knowledge of how this bug came to be.  In the future, all data sources would need to have their "hiddenFromUsers" column checked.
	correctedThumbnailCount_ = thumbnailCount_ - duplicateThumbnailIds_.size();
	fixedFileFormat_ = "vespers2012XRF1ElXRD";
	fixedFilePath_ = filePath_;
	fixedFilePath_.replace(".cdf", ".dat");
	fixedAdditionalFilePath_ = filePath_;
	fixedAdditionalFilePath_.replace(".cdf", "_spectra.dat");

	// Start the database patch!
	////////////////////////////////////////////////////////

	database_->startTransaction();

	if (!database_->update(scanId_, "AMScan_table", "analyzedDataSourcesConnections", QVariant(correctedAnalyzedDataSourceConnectionsToRemain_.join("\n")))){

		qDebug() << "Failed to update row" << scanId_ << "from AMScan_table";
		database_->rollbackTransaction();
		return false;
	}

	if (!database_->update(scanId_, "AMScan_table", "thumbnailCount", QVariant(correctedThumbnailCount_))){

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

	foreach (int id, duplicateThumbnailIds_)
		if (!database_->deleteRow(id, "AMDbObjectThumbnails_table")){

			qDebug() << "Failed to delete row" << id << "from AMDbObjectThumbnails_table";
			database_->rollbackTransaction();
			return false;
		}

	for (int i = 0, size = correctedThumbnailIds_.size(); i < size; i++)
		if (!database_->update(correctedThumbnailIds_.at(i), "AMDbObjectThumbnails_table", "number", QVariant(correctedThumbnailNumbers_.at(i)))){

			qDebug() << "Failed to update row" << correctedThumbnailIds_.at(i) << "from AMDbObjectThumbnails_table";
			database_->rollbackTransaction();
			return false;
		}

	database_->commitTransaction();

	QString path = database_->dbAccessString();
	path.remove("userdata.db");
	QFile::remove(path % filePath_);

	// End of patch.
	/////////////////////////////////////////////////////////////

	// End of helper debug statements.
	//////////////////////////////////////////////////////////////

	qDebug() << "Successfully fixed" << scanName_;

	return true;
}

QString VESPERSDatabaseDuplicateEntryObject::fixStringIndex(const QString &index, QList<int> indexList) const
{
	int id = index.toInt();
	int newId = -1;

	QList<int> fullList = QList<int>() << -1 << indexList;

	for (int i = 0, size = fullList.size(); i < size-1; i++){

		if (id > fullList.at(i) && id <= fullList.at(i+1))
			newId = id - i;

		else if (id > fullList.at(i+1))
			newId = id - (i+1);
	}

	return QString("%1").arg(newId);
}

int VESPERSDatabaseDuplicateEntryObject::fixIntIndex(int index, QList<int> indexList) const
{
	int newId = -1;

	QList<int> fullList = QList<int>() << -1 << indexList;

	for (int i = 0, size = fullList.size(); i< size-1; i++){

		if (index > fullList.at(i) && index <= fullList.at(i+1))
			newId = index - i;

		else if (index > fullList.at(i+1))
			newId = index - (i+1);
	}

	return newId;
}
