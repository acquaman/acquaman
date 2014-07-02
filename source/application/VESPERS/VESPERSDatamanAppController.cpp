/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#include "VESPERSDatamanAppController.h"

#include "ui/AMMainWindow.h"
#include "ui/AMStartScreen.h"

#include "dataman/AMDbObjectSupport.h"

#include "dataman/VESPERS/XRFDetectorInfo.h"
#include "dataman/VESPERS/AMXRFScan.h"
#include "acquaman/VESPERS/VESPERSXRFScanConfiguration.h"

#include <QFileDialog>

VESPERSDatamanAppController::VESPERSDatamanAppController(QObject *parent)
	: AMDatamanAppController(parent)
{
}

bool VESPERSDatamanAppController::startup() {

	// Get a destination folder.
	AMUserSettings::load();
	QString start = AMUserSettings::userDataFolder;
	start.chop(1);
	start = start.left(start.lastIndexOf("/"));
	QString dir = QFileDialog::getExistingDirectory(0, "Choose a destination folder for your data.", start, QFileDialog::ShowDirsOnly);
	if (!dir.isEmpty()){

		dir += "/";

		if (dir.compare(AMUserSettings::userDataFolder) != 0){

			AMUserSettings::userDataFolder = dir;
			AMUserSettings::save();
		}
	}

	// Start up the main program.
	if(AMDatamanAppController::startup()) {

		AMDbObjectSupport::registerClass<XRFDetectorInfo>();
		AMDbObjectSupport::registerClass<VESPERSXRFScanConfiguration>();
		AMDbObjectSupport::registerClass<AMXRFScan>();

		return true;
	}
	else
		return false;
}
