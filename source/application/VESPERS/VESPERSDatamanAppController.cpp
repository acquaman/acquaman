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
