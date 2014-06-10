#include "AMCDFv1FileLoaderPlugin.h"

#include <QFileInfo>
#include "dataman/datastore/AMCDFDataStore.h"
#include "util/AMErrorMonitor.h"

#include "dataman/AMScan.h"

bool AMCDFv1FileLoaderPlugin::accepts(AMScan *scan){
	return (scan->fileFormat() == "amCDFv1");
}

bool AMCDFv1FileLoaderPlugin::load(AMScan *scan, const QString &userDataFolder, AMErrorMon *errorMonitor){

	if(!scan)
		return false;

	QFileInfo sourceFileInfo(scan->filePath());
	if(sourceFileInfo.isRelative()){
		sourceFileInfo.setFile(userDataFolder + "/" + scan->filePath());
	}

	// for now, we create a new AMCDFDataStore by opening this CDF file in read-only mode.  If read-only is a problem in the future (ex: data sources writing to the data store for caching values) we'll need to adjust this strategy.
	AMCDFDataStore* ds = new AMCDFDataStore(sourceFileInfo.filePath(), false, true);
	if(!ds->isValid()) {
		delete ds;
		errorMonitor->exteriorReport(AMErrorReport(0, AMErrorReport::Serious, AMCDFV1FILELOADERPLUGIN_CANNOT_OPEN_FILE, QString("Could not open the CDF file at '%1'. It's possible that the file has been moved, or is corrupted.").arg(sourceFileInfo.filePath())));
		return false;
	}

	if(!scan->replaceRawDataStore(ds)) {
		delete ds;
		errorMonitor->exteriorReport(AMErrorReport(0, AMErrorReport::Serious, AMCDFV1FILELOADERPLUGIN_CANNOT_USE_FILE, QString("Could not use the CDF file at '%1' for the scan '%2': it does not contain the correct measurements for the data sources that should exist in this scan.").arg(sourceFileInfo.filePath()).arg(scan->fullName())));
		return false;
	}

	return true;
}

bool AMCDFv1FileLoaderFactory::accepts(AMScan *scan)
{
	return (scan->fileFormat() == "amCDFv1");
}

Q_EXPORT_PLUGIN2(AMCDFv1FileLoaderFactory, AMCDFv1FileLoaderFactory)

