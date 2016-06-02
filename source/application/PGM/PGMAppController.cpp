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


#include "PGMAppController.h"

#include "beamline/PGM/PGMBeamline.h"

#include "actions3/AMActionRunner3.h"
#include "actions3/actions/AMScanAction.h"
#include "actions3/AMListAction3.h"

#include "application/AMAppControllerSupport.h"

#include "dataman/AMRun.h"
#include "dataman/AMScan.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/export/AMExportController.h"
#include "dataman/export/AMExporterOptionGeneralAscii.h"
#include "dataman/export/AMExporterGeneralAscii.h"
#include "dataman/export/AMExporterAthena.h"
#include "dataman/export/AMSMAKExporter.h"
#include "dataman/export/AMExporterOptionSMAK.h"

#include "ui/util/AMChooseDataFolderDialog.h"
#include "ui/AMMainWindow.h"
#include "ui/dataman/AMGenericScanEditor.h"
#include "ui/acquaman/AMScanConfigurationViewHolder3.h"

#include "ui/PGM/PGMSlitControlView.h"

PGMAppController::PGMAppController(QObject *parent)
	: CLSAppController("PGM", parent)
{
	setDefaultUseLocalStorage(true);

	detectorPaneCategoryName_ = "XRF Detectors";
}

bool PGMAppController::setupDataFolder()
{
	// Get a destination folder.
	return AMChooseDataFolderDialog::getDataFolder("/AcquamanLocalData/pgm",  //local directory
												   "/home/pgm",               //remote directory
												   "users",                   //data directory
												   QStringList());            //extra data directory
}

void PGMAppController::initializeBeamline()
{
	// Initialize central beamline object
	PGMBeamline::pgm();
}

void PGMAppController::registerDBClasses()
{

}

void PGMAppController::registerExporterOptions()
{

}

void PGMAppController::setupScanConfigurations()
{

}

void PGMAppController::setupUserConfiguration()
{

<<<<<<< HEAD
    PGMSlitControlView *slitView = new PGMSlitControlView();
    QWidget *slitWidget = mw_->buildMainWindowPane("General", ":/utilities-system-monitor.png", slitView);
    mw_->addPane(slitWidget, "General", "Slit Control Panel", ":/utilities-system-monitor.png");
=======
>>>>>>> master
}

void PGMAppController::createPersistentView()
{
	//	PGMPersistentView *persistentPanel = new PGMPersistentView;
	//	mw_->addRightWidget(persistentPanel);
}
void PGMAppController::createGeneralPanes()
{
}
void PGMAppController::createDetectorPanes()
{
}
void PGMAppController::createScanConfigurationPanes()
{
}
