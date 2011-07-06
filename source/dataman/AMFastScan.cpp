/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "AMFastScan.h"
#include "util/AMErrorMonitor.h"

#include <QDir>

AMFastScan::AMFastScan(QObject *parent) :
		AMScan(parent)
{
	AMAxisInfo energyAxis("ev", 0, "Incident Energy", "eV");
	data_->addScanAxis(energyAxis);

	autoExportFilePath_ = "";
}

#include "dataman/SGM2010FastFileLoader.h"
#include "dataman/SGM2010FastSensibleFileLoader.h"

bool AMFastScan::loadDataImplementation(){
	SGM2010FastFileLoader sgmLoader(this);

	QFileInfo sourceFileInfo(filePath());
	if(sourceFileInfo.isRelative()){
		qDebug() << "Path IS relative, user data folder is " << AMUserSettings::userDataFolder;
		sourceFileInfo.setFile(AMUserSettings::userDataFolder + "/" + filePath());
	}
	if(fileFormat() == sgmLoader.formatTag()) {
		if(sgmLoader.loadFromFile(sourceFileInfo.filePath(), false, false, false)) {
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
				autoExportFilePath_.append(QString("_%2.dat").arg(maxIndex+1));
			}
			sgmLoader.saveToFile(autoExportFilePath_);
		}
	}
}

void AMFastScan::setAutoExportFilePath(const QString &autoExportFilePath){
	autoExportFilePath_ = autoExportFilePath;
}
