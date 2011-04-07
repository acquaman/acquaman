#include "AMXRFScan.h"
#include "dataman/VESPERS/VESPERSXRFDataLoader.h"
#include "util/AMErrorMonitor.h"

AMXRFScan::AMXRFScan(QObject *parent)
	: AMScan(parent)
{

}

bool AMXRFScan::loadDataImplementation()
{
	VESPERSXRFDataLoader loader(this);

	if (fileFormat() == loader.formatTag()){

		if (loader.loadFromFile(filePath(), false, false, false))
			return true;

		else {

			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Serious, 0, QString("Could not load XRF scan data from '%1'").arg(filePath())));
			return false;
		}
	}

	AMErrorMon::report(AMErrorReport(this, AMErrorReport::Serious, -1, QString("Could not load XRF scan data. The '%1' file format isn't supported.").arg(fileFormat())));
	return false;
}
