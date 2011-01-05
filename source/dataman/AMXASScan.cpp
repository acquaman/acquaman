#include "AMXASScan.h"
#include "AMErrorMonitor.h"
#include <qdebug.h>

AMXASScan::AMXASScan(QObject *parent)
	: AMScan(parent)
{
	data_->addScanAxis( AMAxisInfo("eV", 0, "Incident Energy", "eV") );
}


#include "dataman/SGM2004FileLoader.h"
#include "dataman/ALSBL8XASFileLoader.h"

bool AMXASScan::loadDataImplementation() {

	SGM2004FileLoader sgmLoader(this);

	if(fileFormat() == sgmLoader.formatTag()) {
		if(sgmLoader.loadFromFile(filePath(), false, false, false)) {
			return true;
		}
		else {
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Serious, -1, QString("Could not load raw XAS scan data from '%1'").arg(filePath())));
			return false;
		}
	}


	ALSBL8XASFileLoader alsLoader(this);

	if(fileFormat() == alsLoader.formatTag()) {
		if(alsLoader.loadFromFile(filePath(), false, false, false)) {
			return true;
		}
		else {
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Serious, -1, QString("Could not load raw XAS scan data from '%1'").arg(filePath())));
			return false;
		}
	}



	AMErrorMon::report(AMErrorReport(this, AMErrorReport::Serious, -1, QString("Could not load raw XAS scan data. The '%1' file format isn't supported.").arg(fileFormat())));
	return false;
}



