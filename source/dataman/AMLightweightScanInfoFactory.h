#ifndef AMLIGHTWEIGHTSCANINFOFACTORY_H
#define AMLIGHTWEIGHTSCANINFOFACTORY_H

#define AMLIGHTWEIGHTSCANINFOFACTORY_SAMPLES_SQL_ERROR 45977
#define AMLIGHTWEIGHTSCANINFOFACTORY_OBJECT_TYPES_SQL_ERROR 45978
#define AMLIGHTWEIGHTSCANINFOFACTORY_SCANS_SQL_ERROR 45979
#define AMLIGHTWEIGHTSCANINFOFACTORY_RUNS_SQL_ERROR 45980
#define AMLIGHTWEIGHTSCANINFOFACTORY_EXPERIMENTS_SQL_ERROR 45981

#include <QList>
#include <QHash>
#include <QString>
#include <QtSql>

class AMDatabase;
class AMLightweightScanInfo;
/**
 * Factory class designed for optimized building of bulk and single
 * AMLightweightScanInfos.
 * -----
 * For bulk loads an instance of the class is created which is then used to
 * store persistant data for the length of the load. For single loads no
 * instance is created. As such the helper methods defined below are static
 * in the case that they are used in loading a single Scan Info, and not in
 * the case they they are used in the bulk load
 */
class AMLightweightScanInfoFactory
{
public:
	/// Builds a collection of AMLightweightScanInfos from the database
	/// in bulk
	static QList<AMLightweightScanInfo*> all(AMDatabase* database);

	/// Retrieves a single AMLightweightScanInfo for the scan with the provided
	/// id from the passed database
	static AMLightweightScanInfo* single(int scanId, AMDatabase* database);

	/// Retrieves a mapping of run ids to run names which are currently stored
	/// in the database
	static QHash<int, QString> runMap(AMDatabase* database);

	///  Refreshes the passed in AMLightweightScanInfo with the information
	/// currently in the database
	static void updateSingle(AMLightweightScanInfo* info, AMDatabase* database);

	/// Returns the scanId for the scanId refered to in the ObjectExperimentEntries table
	/// with the provided id
	static int scanIdForExperimentEntry(int id, AMDatabase* database);
protected:
	/// Protected default c'tor to stop instances of class being created
	AMLightweightScanInfoFactory();

	/// Protected instance of class which is used to provide non-static
	/// persistant data when performing the bulk load
	AMLightweightScanInfoFactory(AMDatabase* database);

	/// Loads all the Scan's info from the database.
	/// NOTE: The helper methods below for populating the maps must be
	/// run before this.
	QList<AMLightweightScanInfo*> loadAll();

	// Helper methods used during the bulk load. Each one populates a map in a
	// bulk operation which is then used to load each of the ScanInfos quickly

	/// Loads the info for the ExperimentMap from the database
	void populateExperimentIds();
	/// Loads the info for the SampleMap from the database
	void populateSamples();
	/// Loads the info for the ObjectTypes Map from the database
	void populateObjectTypes();
	/// Loads the info for the Run Map from the database
	void populateRuns();

	/// Returns the used sample name from the two entries in the Scan table:
	/// SampleNamePre2013 and Sample
	QString getSampleName(const QString& sampleNamePre2013, const QString& sampleTableEntry) const;

	// Helper methods used for getting single scan accompanying info

	/// Returns the run name from the provided runId
	static QString loadRunName(AMDatabase* database, int runId);
	/// Returns the scan type for the provided class name
	static QString loadScanType(AMDatabase* database, const QString& className);
	/// Returns the name of the sample from the provided Sample entry in the Scan table
	static QString loadSampleName(AMDatabase* database,
								  const QString& sampleNamePre2013,
								  const QString& sampleTableEntry);
	/// Returns the list of experimentIds to which the scan with the provided id belongs
	static QList<int> loadExperimentIds(AMDatabase* database, int scanId);

	// Member Variables used to store persistant data during the bulk load

	/// Scans and experiments have a 0,,n:0,,n relationship.
	/// The hash maps ScanId to a list of ExperimentIds
	QHash<int, QList<int> > experimentMap_;
	/// Scans are linked to Samples in AMScan_table by means of a non atomic
	/// attribute which stores the tablename and the id of the Sample. As such
	/// in order to map a scan to the name we need:
	/// A hash which maps TableName => TableName.Id => SampleName
	QHash< QString, QHash < int, QString> > sampleMap_;
	/// The type of a scan is stored as the Class name. In order to get the
	/// human readable name we need to check the AMDbObjectTypes_table an
	/// get the description.
	/// The hash maps the class name to a human readable class description
	QHash<QString, QString> objectTypesMap_;
	/// Scans and Runs have a 1,,n:1,,1 relationship with the RunId simply being
	/// stored as an id in AMScan_table.
	/// The hash maps runId to runName
	QHash<int, QString> runNameMap_;
	/// A reference to the database from which the data is to be loaded.
	AMDatabase* database_;
};

#endif // AMLIGHTWEIGHTSCANINFOFACTORY_H
