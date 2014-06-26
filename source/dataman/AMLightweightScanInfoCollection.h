#ifndef AMLIGHTWEIGHTSCANINFOCOLLECTION_H
#define AMLIGHTWEIGHTSCANINFOCOLLECTION_H

#include <QList>
#include <QHash>
#include <QtSql>
#include "AMLightweightScanInfo.h"

class AMDatabase;
class AMLightweightScanInfoCollection : QList<AMLightweightScanInfo>
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
	AMLightweightScanInfoCollection(AMDatabase* database);
protected:
	void populateSampleNames();
	void populateObjectTypes();
	void populateCollection();
	void populateRuns();
	QString getRunName(int runId);
	QString getScanType(const QString& dbObjectType);
	QString getSampleName(const QString& sampleResult);


};

#endif // AMLIGHTWEIGHTSCANINFOCOLLECTION_H
