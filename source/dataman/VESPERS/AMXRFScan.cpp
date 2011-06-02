#include "AMXRFScan.h"
#include "dataman/VESPERS/VESPERSXRFDataLoader.h"
#include "util/AMErrorMonitor.h"
#include <QFileInfo>
#include "util/AMSettings.h"

AMXRFScan::AMXRFScan(QObject *parent)
	: AMScan(parent)
{

}

bool AMXRFScan::loadDataImplementation()
{
	VESPERSXRFDataLoader loader(this);

	QFileInfo sourceFile(filePath());
	if(sourceFile.isRelative())
		sourceFile.setFile(AMUserSettings::userDataFolder + "/" + filePath());


	if (fileFormat() == loader.formatTag()){

		if (loader.loadFromFile(sourceFile.filePath(), false, false, false))
			return true;

		else {

			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Serious, 0, QString("Could not load XRF scan data from '%1'").arg(filePath())));
			return false;
		}
	}

	AMErrorMon::report(AMErrorReport(this, AMErrorReport::Serious, -1, QString("Could not load XRF scan data. The '%1' file format isn't supported.").arg(fileFormat())));
	return false;
}
