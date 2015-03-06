#ifndef AMLIGHTWEIGHTSCANINFOFACTORY_H
#define AMLIGHTWEIGHTSCANINFOFACTORY_H

#define AMLIGHTWEIGHTSCANINFOFACTORY_SAMPLES_SQL_ERROR 45977
#define AMLIGHTWEIGHTSCANINFOFACTORY_OBJECT_TYPES_SQL_ERROR 45978
#define AMLIGHTWEIGHTSCANINFOFACTORY_SCANS_SQL_ERROR 45979
#define AMLIGHTWEIGHTSCANINFOFACTORY_RUNS_SQL_ERROR 45980

#include <QList>
#include <QHash>
#include <QString>
#include <QtSql>

class AMDatabase;
class AMLightweightScanInfo;
/**
 * Factory class designed for optimized building of bulk and single
 * AMLightweightScanInfos
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

	// HelperFunction for parsing record data
	QString parseSampleName(const QString& sampleNamePre2013, const QString& sampleTableEntry) const;

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
