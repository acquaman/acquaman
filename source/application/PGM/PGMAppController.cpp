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
#include "ui/beamline/AMXRFDetailedDetectorView.h"

#include "ui/PGM/PGMSlitControlView.h"
#include "ui/PGM/PGMPersistentView.h"
#include "ui/PGM/PGMBladeCurrentView.h"

PGMAppController::PGMAppController(QObject *parent)
	: CLSAppController("PGM", parent)
{
	userConfiguration_ = new PGMUserConfiguration(this);
	setDefaultUseLocalStorage(true);

	detectorPaneCategoryName_ = "XRF Detectors";
}

void PGMAppController::onUserConfigurationLoadedFromDb()
{
	if (userConfiguration_) {

		AMXRFDetector *oceanOpticsDetector = PGMBeamline::pgm()->oceanOpticsDetector();

		if (oceanOpticsDetector) {

			foreach (AMRegionOfInterest *region, userConfiguration_->regionsOfInterest()){
				if (!containsRegionOfInterest(oceanOpticsDetector->regionsOfInterest(), region)) {
					oceanOpticsDetector->addRegionOfInterest(region);
					onRegionOfInterestAdded(region);
				}
			}

			connect(oceanOpticsDetector, SIGNAL(addedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestAdded(AMRegionOfInterest*)));
			connect(oceanOpticsDetector, SIGNAL(removedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestRemoved(AMRegionOfInterest*)));
			connect(oceanOpticsDetector, SIGNAL(regionOfInterestBoundingRangeChanged(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestBoundingRangeChanged(AMRegionOfInterest*)));
		}
	}
}

void PGMAppController::onRegionOfInterestAdded(AMRegionOfInterest *region)
{
	if (region) {

		// Add the region of interest to the user configuration, if it doesn't have it already.

		if (userConfiguration_ && !containsRegionOfInterest(userConfiguration_->regionsOfInterest(), region))
			userConfiguration_->addRegionOfInterest(region);
	}
}

void PGMAppController::onRegionOfInterestRemoved(AMRegionOfInterest *region)
{
	if (userConfiguration_ && userConfiguration_->regionsOfInterest().contains(region))
		userConfiguration_->removeRegionOfInterest(region);
}

void PGMAppController::onRegionOfInterestBoundingRangeChanged(AMRegionOfInterest *region)
{
	if (userConfiguration_ && userConfiguration_->regionsOfInterest().contains(region))
		userConfiguration_->setRegionOfInterestBoundingRange(region);
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
	if (userConfiguration_) {

		connect( userConfiguration_, SIGNAL(loadedFromDb()), this, SLOT(onUserConfigurationLoadedFromDb()) );

		bool loaded = userConfiguration_->loadFromDb(AMDatabase::database("user"), 1);
		if (!loaded) {
			userConfiguration_->storeToDb(AMDatabase::database("user"));
			onUserConfigurationLoadedFromDb();
		}
	}
}

void PGMAppController::createPersistentView()
{
	PGMPersistentView *persistentPanel = new PGMPersistentView;
	mw_->addRightWidget(persistentPanel);
}

void PGMAppController::createGeneralPanes()
{
    mw_->addPane(mw_->buildMainWindowPane("Blade Currents", ":/utilities-system-monitor.png", new PGMBladeCurrentView), "General", "Blade Currents", ":/utilities-system-monitor.png");
	PGMSlitControlView *slitView = new PGMSlitControlView();
    QWidget *slitWidget = mw_->buildMainWindowPane("Slits", ":/utilities-system-monitor.png", slitView);
    mw_->addPane(slitWidget, "General", "Slits", ":/utilities-system-monitor.png");
}

void PGMAppController::createDetectorPanes()
{
	AMXRFDetailedDetectorView *oceanOpticsDetectorView = new AMXRFDetailedDetectorView(PGMBeamline::pgm()->oceanOpticsDetector());
	oceanOpticsDetectorView->buildDetectorView();
	mw_->addPane(oceanOpticsDetectorView, detectorPaneCategoryName_, "Ocean Optics", detectorPaneIcon_);
}

void PGMAppController::createScanConfigurationPanes()
{

}


bool PGMAppController::containsRegionOfInterest(QList<AMRegionOfInterest *> regionOfInterestList, AMRegionOfInterest *toFind) const
{
	bool regionOfInterestFound = false;

	if (!regionOfInterestList.isEmpty() && toFind) {
		for (int i = 0, count = regionOfInterestList.count(); i < count && !regionOfInterestFound; i++) {
			AMRegionOfInterest *regionOfInterest = regionOfInterestList.at(i);

			if (regionOfInterest && regionOfInterest->name() == toFind->name())
				regionOfInterestFound = true;
		}
	}

	return regionOfInterestFound;
}
