#ifndef VESPERSDATABASEDUPLICATEENTRYOBJECT_H
#define VESPERSDATABASEDUPLICATEENTRYOBJECT_H

#include "dataman/database/AMDatabase.h"

/// This class does all the work to fix all the duplicate entries for a scan.
class VESPERSDatabaseDuplicateEntryObject
{
public:
	/// Constructor.  Takes the id of the scan and the database that it needs to work with.
	VESPERSDatabaseDuplicateEntryObject(AMDatabase *database, int scanId);

	/// Goes through all the work to fix the the database for the given scan.
	bool fix();

protected:
	/// Helper method that takes in a string that represents an index, a list of duplicate indices, and returns a QString with a corrected index string.
	QString fixStringIndex(const QString &index, QList<int> indexList) const;

	/// The database the object works with.
	AMDatabase *database_;
	/// The id of the scan to check.
	int scanId_;
	/// The scan name.
	QString scanName_;
	/// The file format.
	QString fileFormat_;
	/// The file path to the data file.
	QString filePath_;
	/// The analyzed data source connections string.
	QString analyzedDataSourceConnections_;
	/// A list of all the raw data sources.
	QList<int> allRawDataSources_;
	/// A list of all the ids for the associated table of raw data sources.
	QList<int> allRawDataSourcesAssociatedTableIds_;
	/// A list of all the analysis blocks.
	QList<int> allAnalysisBlocks_;
	/// A list of all the ids for the associated table of analysis blocks.
	QList<int> allAnalysisBlocksAssociatedTableIds_;
	/// The list of the corresponding analysis block table names.
	QStringList allAnalysisBlockTableNames_;
	/// The list of raw data source names.
	QStringList allRawDataSourceNames_;
	/// A list of all the duplicate raw data source ids.
	QList<int> duplicateRawDataSources_;
	/// A list of all the duplicate raw data source associated table ids.
	QList<int> duplicateRawDataSourceAssociatedTableIds_;
	/// The list of all the names of the analysis blocks.
	QStringList allAnalysisBlockNames_;
	/// The list of the duplicate analysis block ids.
	QList<int> duplicateAnalysisBlocks_;
	/// The list of duplicate analysis block table names.
	QStringList duplicateAnalysisBlockTableNames_;
	/// The list of duplicate analysis block ids from the associated table.
	QList<int> duplicateAnalysisBlockAssociatedTableIds_;
	/// The list of indices for data sources that will be removed.
	QList<int> dataSourceIndexToBeRemoved_;
	/// The list of analyzed data sources that need to be removed.
	QStringList analyzedDataSourceConnectionsToBeRemoved_;
	/// The list of analyzed data sources that will remain.
	QStringList analyzedDataSourceConnectionsToRemain_;
	/// The list of analyzed data sources that will remain but with fixed ids.
	QStringList correctedAnalyzedDataSourceConnectionsToRemain_;
	/// The fixed file format.
	QString fixedFileFormat_;
	/// The fixed file path.
	QString fixedFilePath_;
	/// The fixed additional file path.
	QString fixedAdditionalFilePath_;
};

#endif // VESPERSDATABASEDUPLICATEENTRYOBJECT_H
