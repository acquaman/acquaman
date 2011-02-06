#include "AMFastScan.h"
#include "util/AMErrorMonitor.h"

AMFastScan::AMFastScan(QObject *parent) :
		AMScan(parent)
{
	AMAxisInfo energyAxis("ev", 0, "Incident Energy", "eV");
	//data_->addScanAxis( AMAxisInfo("eV", 0, "Incident Energy", "eV") );
	data_->addScanAxis(energyAxis);
}

#include "dataman/SGM2010FastFileLoader.h"
#include "dataman/SGM2010FastSensibleFileLoader.h"

bool AMFastScan::loadDataImplementation(){
	SGM2010FastFileLoader sgmLoader(this);

	if(fileFormat() == sgmLoader.formatTag()) {
		if(sgmLoader.loadFromFile(filePath(), false, false, false)) {
			return true;
		}
		else {
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Serious, -1, QString("Could not load raw fast scan data from '%1'").arg(filePath())));
			return false;
		}
	}

	AMErrorMon::report(AMErrorReport(this, AMErrorReport::Serious, -1, QString("Could not load raw fast scan data. The '%1' file format isn't supported.").arg(fileFormat())));
	return false;
}

bool AMFastScan::storeToDb(AMDatabase *db){
	bool succeeded = AMScan::storeToDb(db);
	if(succeeded){
		SGM2010FastSensibleFileLoader sgmLoader(this);
		sgmLoader.saveToFile("/Users/fawkes/Documents/CLS/SGM/ACQUAMANDATA/"+QDateTime::currentDateTime().toString()+".dat");
	}
}
