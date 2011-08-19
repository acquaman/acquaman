/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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


#include "AMXASScan.h"
#include "util/AMErrorMonitor.h"
#include <qdebug.h>

AMXASScan::AMXASScan(QObject *parent)
	: AMScan(parent)
{
	data_->addScanAxis( AMAxisInfo("eV", 0, "Incident Energy", "eV") );
}


#include "dataman/SGM2004FileLoader.h"
#include "dataman/ALSBL8XASFileLoader.h"
#include "dataman/SGM2011XASFileLoader.h"
#include <QFileInfo>
#include "util/AMSettings.h"

bool AMXASScan::loadDataImplementation() {

	SGM2004FileLoader sgmLoader(this);

	QFileInfo sourceFileInfo(filePath());
	if(sourceFileInfo.isRelative()){
		qDebug() << "Path IS relative, user data folder is " << AMUserSettings::userDataFolder;
		sourceFileInfo.setFile(AMUserSettings::userDataFolder + "/" + filePath());
	}
	else
		qDebug() << "Path IS NOT relative.";

	if(fileFormat() == sgmLoader.formatTag()) {
		if(sgmLoader.loadFromFile(sourceFileInfo.filePath(), false, false, false)) {
			return true;
		}
		else {
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Serious, -1, QString("Could not load raw XAS scan data from '%1'").arg(filePath())));
			return false;
		}
	}

	SGM2011XASFileLoader sgm2011Loader(this);

	if(fileFormat() == sgm2011Loader.formatTag()) {
		if(sgm2011Loader.loadFromFile(sourceFileInfo.filePath(), false, false, false)) {
			return true;
		}
		else {
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Serious, -1, QString("Could not load raw XAS scan data from '%1'").arg(filePath())));
			return false;
		}
	}

	ALSBL8XASFileLoader alsLoader(this);

	if(fileFormat() == alsLoader.formatTag()) {
		if(alsLoader.loadFromFile(sourceFileInfo.filePath(), false, false, false)) {
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
