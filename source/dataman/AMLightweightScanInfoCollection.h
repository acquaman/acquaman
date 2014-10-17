#ifndef AMLIGHTWEIGHTSCANINFOCOLLECTION_H
#define AMLIGHTWEIGHTSCANINFOCOLLECTION_H

#include <QList>
#include <QHash>
#include <QtSql>
#include "AMLightweightScanInfo.h"

#define AMLIGHTWEIGHTSCANINFOCOLLECTION_SAMPLES_SQL_ERROR 45977
#define AMLIGHTWEIGHTSCANINFOCOLLECTION_OBJECT_TYPES_SQL_ERROR 45978
#define AMLIGHTWEIGHTSCANINFOCOLLECTION_SCANS_SQL_ERROR 45979
#define AMLIGHTWEIGHTSCANINFOCOLLECTION_RUNS_SQL_ERROR 45980
class AMDatabase;
/// A class representing a bulk loaded collection of Scan's database info
class AMLightweightScanInfoCollection : public QObject
{
	Q_OBJECT
public:
	/// Creates an instance of an AMLightweightScanInfoCollection loaded from the provided database
	AMLightweightScanInfoCollection(AMDatabase* database);
	/// Returns the scan with the provided id in the standard
	/// URL format: amd://databaseConnectionName/tableName/objectId
	QUrl getScanUrl(int index) const;
	/// Returns the AMLightWeightScanInfo at index position i in the list. i must be a valid index position in
	/// in the list (i.e. 0 <= i < size())
	AMLightweightScanInfo* at(int index) const;
	/// Returns the number of items in the list. This is effectively the same as size().
	int count() const;
	/// Returns the index position of the first occurrence of value in the list, searching forward from index
	/// position from. Returns -1 if no item matched
	int indexOf(AMLightweightScanInfo* scan, int from = 0) const;
signals:
	/// Emitted just before a scan info is added to the collection
	void scanAboutToBeAdded(int rowIndex);
	/// Emitted whenever a scan info is finished being added to the collection after the initial loading
	void scanAdded();
	/// Emitted whenever a scan info is altered
	void scanUpdated(int rowIndex);
	/// Emitted just before a scan info is removed from the collection
	void scanAboutToBeRemoved(int rowIndex);
	/// Emitted whenever a scan info is finished being removed from the collection
	void scanRemoved();
	/// Emitted whenever a scan is about to remove its thumbnail from the collection
	void scanThumbnailAboutToBeRemoved(int scanIndex, int oldThumbnailIndex);
	/// Emitted whenever a scan has its thumbnail removed from the collection
	void scanThumbnailRemoved();
	/// Emitted whenever a scan has a new thumbnail added to it
	void scanThumbnailAdded(int scanId, int thumbnailId);
protected slots:
	/// Slot to handle the database signal indicating that a new item has been added to the database
	void onDbItemAdded(const QString& tableName, int id);
	/// Slot to handle the database signal indicating that an existing item in the database has been updated
	void onDbItemUpdated(const QString& tableName, int id);
	/// Slot to handle the database signal indicating that an item has been removed from the database
	void onDbItemRemoved(const QString& tableName, int oldId);
protected:
	/// Builds the mapping of ScanId to ExperimentId
	void populateExperimentIds();
	/// Builds the mapping of SampleTable -> (SampleId -> SampleName)
	void populateSampleNames();
	/// Loads a sample from the database and adds it to the Sample map
	void populateSingleSample(const QString &tableName, int id);
	/// Builds the mapping of ObjectName -> ObjectDescription
	void populateObjectTypes();
	/// Builds the collection by loading scan's info from the database
	void populateCollection();
	/// Loads a single scan info from the database and adds it to the collection
	void populateSingleScanInfo(int id);
	/// Refreshes the provided scan info with data loaded from the database
	void updateSingleScanInfo(AMLightweightScanInfo* scanInfo);
	/// Builds the mapping of RunId -> RunName
	void populateRuns();
	/// Loads a run from the database and adds it to the Run map
	void populateSingleRun(int id);
	/// Looks up the run name from the mapping using the runId
	QString getRunName(int runId) const;
	/// Looks up the scan type description from the object type mapping using the dbObjectName
	QString getScanType(const QString& dbObjectType) const;
	/// Looks up the sample name from the relevant table mapping and id using the return results of the
	/// format TABLE_NAME;ID
	QString getSampleName(const QString& sampleResult) const;
	/// Looks up the ExperimentId for a given scanId, returns -1 if no mapping is found in the hash table
	int getExperimentId(int scanId) const;
	/// Returns the Id of the scan to which a given thumbnail id refers. If the given thumbnailId
	/// does not refer to a scan, then -1 is returned.
	int getScanIdFromThumbnailId(int thumbnailId);
private:
	/// The database from which the scan infos will be retrieved
	AMDatabase* database_;
	/// A mapping of run ids to run names
	QHash<int, QString> runMap_;
	/// A mapping of Object names to their descriptions (used for scan types)
	QHash<QString, QString> objectTypesMap_;
	/// The first hash contains The Table Name containing samples mapped to a secondary hash, which contains
	/// sample ids mapped to sample names
	QHash< QString, QHash < int, QString> > sampleNameMap_;
	/// A list of the contained AMLightweightScanInfos
	QList<AMLightweightScanInfo*> scanInfos_;
	/// A mapping of scan id to experiment id. If entry is found for a scan, it's experimentId is set to -1
	QHash<int, int> experimentIdMap_;
	/// The id of the last updated scan. If no scan has been updated will equal -1
	int lastUpdatedScanId_;


};

#endif // AMLIGHTWEIGHTSCANINFOCOLLECTION_H
