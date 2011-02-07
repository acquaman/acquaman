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


#include "AMXESScan.h"

AMXESScan::AMXESScan(QObject *parent) :
	AMScan(parent)
{

}



#include "dataman/REIXS/REIXSXESRawFileLoader.h"
#include "util/AMErrorMonitor.h"

bool AMXESScan::loadDataImplementation() {

	REIXSXESRawFileLoader rawLoader(this);

	if(fileFormat() == rawLoader.formatTag()) {
		if(rawLoader.loadFromFile(filePath(), false, false, false)) {
			return true;
		}
		else {
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Serious, -1, QString("Could not load raw XES scan data from '%1'").arg(filePath())));
			return false;
		}
	}



	AMErrorMon::report(AMErrorReport(this, AMErrorReport::Serious, -1, QString("Could not load raw XES scan data. The '%1' file format isn't supported.").arg(fileFormat())));
	return false;
}



