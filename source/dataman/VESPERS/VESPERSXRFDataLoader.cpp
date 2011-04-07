#include "VESPERSXRFDataLoader.h"
#include "util/AMErrorMonitor.h"

#include <QFile>
#include <QTextStream>

VESPERSXRFDataLoader::VESPERSXRFDataLoader(AMXRFScan *scan)
	: AMAbstractFileLoader(scan)
{
}

bool VESPERSXRFDataLoader::loadFromFile(const QString &filepath, bool setMetaData, bool setRawDataSources, bool createDefaultAnalysisBlocks)
{
	AMXRFScan *scan = qobject_cast<AMXRFScan *>(scan_);

	if (!scan){

		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, 0, "VESPERS XRF File Loader: Could not load XRF data into a non-XRF scan."));
		return false;
	}

	/// \todo Once I actually have data sources and crap, implement load to file.

	// Currently don't have meta data, raw data or analysis blocks.
	Q_UNUSED(setMetaData)
	Q_UNUSED(setRawDataSources)
	Q_UNUSED(createDefaultAnalysisBlocks)

	return true;
}

bool VESPERSXRFDataLoader::saveToFile(const QString &filepath)
{
	/// \todo Once I actually have data sources and crap, implement save to file.

	return true;
}
