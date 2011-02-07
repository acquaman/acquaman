#include "AMFastScan.h"
#include "util/AMErrorMonitor.h"

#include <QDir>

AMFastScan::AMFastScan(QObject *parent) :
		AMScan(parent)
{
	AMAxisInfo energyAxis("ev", 0, "Incident Energy", "eV");
	//data_->addScanAxis( AMAxisInfo("eV", 0, "Incident Energy", "eV") );
	data_->addScanAxis(energyAxis);

	autoExportFilePath_ = "";
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
		//sgmLoader.saveToFile("/Users/fawkes/Documents/CLS/SGM/ACQUAMANDATA/"+QDateTime::currentDateTime().toString()+".dat");
		//qDebug() << "I want to auto save the fast scan to " << autoExportFilePath_;
		if(!autoExportFilePath_.isEmpty()){
			if(QFile::exists(autoExportFilePath_)){
				autoExportFilePath_.truncate(autoExportFilePath_.indexOf('_'));
				QString path = autoExportFilePath_.section('/', 0, -2);
				QString file = autoExportFilePath_.section('/', -1);
				QDir dir(path);
				QStringList filefilters, likeFiles;
				filefilters << QString("%1_*.dat").arg(file);
				likeFiles = dir.entryList(filefilters, QDir::Files);
				int pIndex, aIndex, maxIndex;
				maxIndex = 0;
				foreach(QString f, likeFiles){
					pIndex = f.indexOf('_');
					aIndex = f.indexOf(".dat");
					maxIndex = std::max(f.mid(pIndex+1, aIndex-(pIndex+1)).toInt(), maxIndex);
				}
				//qDebug() << "File will be renamed" << QString("%1_%2.dat").arg(file).arg(maxIndex+1);
				autoExportFilePath_.append(QString("_%2.dat").arg(maxIndex+1));
			}
			sgmLoader.saveToFile(autoExportFilePath_);
		}
	}
}

void AMFastScan::setAutoExportFilePath(const QString &autoExportFilePath){
	autoExportFilePath_ = autoExportFilePath;
}
