#ifndef AMLIGHTWEIGHTSCANINFOCOLLECTION_H
#define AMLIGHTWEIGHTSCANINFOCOLLECTION_H

#include <QList>
#include <QHash>
#include <QtSql>
#include "AMLightweightScanInfo.h"

class AMDatabase;
/// A class representing a bulk loaded collection of Scan's database info
class AMLightweightScanInfoCollection : public QList<AMLightweightScanInfo*>
{
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

public:
	/// Creates an instance of an AMLightweightScanInfoCollection loaded from the provided database
	AMLightweightScanInfoCollection(AMDatabase* database);
	/// Returns the scan with the provided id in the standard
	/// URL format: amd://databaseConnectionName/tableName/objectId
	QUrl getScanUrl(int index);
protected:
	/// Builds the mapping of SampleTable -> (SampleId -> SampleName)
	void populateSampleNames();
	/// Builds the mapping of ObjectName -> ObjectDescription
	void populateObjectTypes();
	/// Builds the collection by loading scan's info from the database
	void populateCollection();
	/// Builds the mapping of RunId -> RunName
	void populateRuns();
	/// Looks up the run name from the mapping using the runId
	QString getRunName(int runId);
	/// Looks up the scan type description from the object type mapping using the dbObjectName
	QString getScanType(const QString& dbObjectType);
	/// Looks up the sample name from the relevant table mapping and id using the return results of the
	/// format TABLE_NAME;ID
	QString getSampleName(const QString& sampleResult);


};

#endif // AMLIGHTWEIGHTSCANINFOCOLLECTION_H
