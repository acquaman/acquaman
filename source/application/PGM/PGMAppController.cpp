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

#include "beamline/AMControl.h"

#include "beamline/PGM/PGMBeamline.h"
#include "acquaman/PGM/PGMXASScanConfiguration.h"

#include "actions3/AMActionRunner3.h"
#include "actions3/actions/AMScanAction.h"
#include "actions3/AMListAction3.h"

#include "application/AMAppControllerSupport.h"
#include "application/PGM/PGM.h"

#include "dataman/AMRun.h"
#include "dataman/AMScan.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/export/AMExportController.h"
#include "dataman/export/AMExporterOptionGeneralAscii.h"
#include "dataman/export/AMExporterGeneralAscii.h"
#include "dataman/export/AMExporterAthena.h"
#include "dataman/export/AMExporterXDIFormat.h"
#include "dataman/export/AMExporterOptionXDIFormat.h"
#include "dataman/export/AMSMAKExporter.h"
#include "dataman/export/AMExporterOptionSMAK.h"

#include "ui/AMMainWindow.h"
#include "ui/acquaman/AMScanConfigurationViewHolder3.h"
#include "ui/beamline/AMXRFDetailedDetectorView.h"
#include "ui/dataman/AMGenericScanEditor.h"
#include "ui/CLS/CLSBeamlineStatusView.h"
#include "ui/CLS/CLSSynchronizedDwellTimeView.h"
#include "ui/util/AMChooseDataFolderDialog.h"

#include "ui/PGM/PGMPersistentView.h"
#include "ui/PGM/PGMSlitControlView.h"
#include "ui/PGM/PGMPersistentView.h"
#include "ui/PGM/PGMBladeCurrentView.h"
#include "ui/PGM/PGMVariableApertureMaskView.h"
#include "ui/PGM/PGMXASScanConfigurationView.h"
#include "ui/PGM/PGMGratingView.h"
#include "ui/PGM/PGMUndulatorView.h"

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
	if (userConfiguration_ && containsRegionOfInterest(userConfiguration_->regionsOfInterest(), region))
		userConfiguration_->removeRegionOfInterest(region);
}

void PGMAppController::onRegionOfInterestBoundingRangeChanged(AMRegionOfInterest *region)
{
	if (userConfiguration_ && containsRegionOfInterest(userConfiguration_->regionsOfInterest(), region))
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

void PGMAppController::goToBeamlineStatusView(AMControl *control)
{
	if (beamlineStatusView_) {

		// Set the given control as the view's selected control.

		beamlineStatusView_->setSelectedComponent(control);

		// Set the beam status pane as the current pane.
		QWidget *windowPane = viewPaneMapping_.value(beamlineStatusView_, 0);
		if (windowPane)
			mw_->setCurrentPane(windowPane);
	}
}

void PGMAppController::initializeBeamline()
{
	// Initialize central beamline object
	PGMBeamline::pgm();
}

void PGMAppController::registerDBClasses()
{
	AMDbObjectSupport::s()->registerClass<PGMXASScanConfiguration>();
}

void PGMAppController::registerExporterOptions()
{
        AMExporterOptionXDIFormat *pgmXASExportOptions = PGM::buildXDIFormatExporterOption("PGMXASDefault", true);
        if(pgmXASExportOptions->id() > 0)
            AMAppControllerSupport::registerClass<PGMXASScanConfiguration, AMExporterXDIFormat, AMExporterOptionXDIFormat>(pgmXASExportOptions->id());
}

void PGMAppController::setupScanConfigurations()
{
	xasScanConfiguration_ = new PGMXASScanConfiguration;
	xasScanConfiguration_->setControl(0, AMBeamline::bl()->exposedControlByName("Energy")->toInfo());
//	xasScanConfiguration_->addDetector(AMBeamline::bl()->exposedDetectorByName("teyBladeCurrentDetector")->toInfo());
//	xasScanConfiguration_->addDetector(AMBeamline::bl()->exposedDetectorByName("flyBladeCurrentDetector")->toInfo());
	xasScanConfiguration_->addDetector(AMBeamline::bl()->exposedDetectorByName("i0EndstationBladeCurrentDetector")->toInfo());
	xasScanConfiguration_->addDetector(AMBeamline::bl()->exposedDetectorByName("i0BeamlineBladeCurrentDetector")->toInfo());
//	xasScanConfiguration_->addDetector(AMBeamline::bl()->exposedDetectorByName("photodiodeBladeCurrentDetector")->toInfo());

	xasScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStart(91);
	xasScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStep(0.1);
	xasScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionEnd(92);
	xasScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionTime(1);
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

void PGMAppController::setupUserInterfaceImplementation()
{
	mw_->setWindowTitle("Acquaman - VLS PGM");
}

void PGMAppController::onScanEditorCreatedImplementation(AMGenericScanEditor *editor)
{
	Q_UNUSED(editor)
}

void PGMAppController::createPersistentView()
{
	PGMPersistentView *pgmPersistentView = new PGMPersistentView;
	connect( pgmPersistentView, SIGNAL(beamlineStatusSelectedComponentChanged(AMControl*)), this, SLOT(goToBeamlineStatusView(AMControl*)) );
	mw_->addRightWidget(pgmPersistentView);
}

void PGMAppController::createGeneralPanes()
{
	// create beamline status view
	beamlineStatusView_ = new CLSBeamlineStatusView(PGMBeamline::pgm()->beamlineStatus(), false);
        beamlineStatusView_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	addMainWindowViewToPane( beamlineStatusView_, "Beamline status", generalPaneCategeryName_, generalPaneIcon_);

	CLSSynchronizedDwellTime *synchronizedDwellTime = qobject_cast<CLSSynchronizedDwellTime *>(AMBeamline::bl()->synchronizedDwellTime());
	CLSSynchronizedDwellTimeView *synchronizedDwellTimeView = new CLSSynchronizedDwellTimeView(synchronizedDwellTime);
	synchronizedDwellTimeView->setAdvancedViewVisible(true);
	addMainWindowViewToPane(synchronizedDwellTimeView, "Synchronized Dwell", generalPaneCategeryName_, generalPaneIcon_);

	addMainWindowViewToPane(new PGMBladeCurrentView, "Blade Currents", generalPaneCategeryName_, generalPaneIcon_);
	addMainWindowViewToPane(new PGMSlitControlView, "Slits", generalPaneCategeryName_, generalPaneIcon_);
	addMainWindowViewToPane(new PGMGratingView, "Mono Grating", generalPaneCategeryName_, generalPaneIcon_);
	addMainWindowViewToPane(new PGMUndulatorView, "Undulator", generalPaneCategeryName_, generalPaneIcon_);
	addMainWindowViewToPane(new PGMVariableApertureMaskView(PGMBeamline::pgm()->vam()), "Variable Aperture Mask", generalPaneCategeryName_, generalPaneIcon_);
}

void PGMAppController::createDetectorPanes()
{
	AMXRFDetailedDetectorView *oceanOpticsDetectorView = new AMXRFDetailedDetectorView(PGMBeamline::pgm()->oceanOpticsDetector());
	oceanOpticsDetectorView->buildDetectorView();
        mw_->addPane(oceanOpticsDetectorView, detectorPaneCategoryName_, "Ocean Optics", detectorPaneIcon_);
}

void PGMAppController::createScanConfigurationPanes()
{
        xasScanConfigurationView_ = new PGMXASScanConfigurationView(xasScanConfiguration_);
	xasScanConfigurationViewHolder3_ = new AMScanConfigurationViewHolder3(xasScanConfigurationView_, true);
	mw_->addPane(xasScanConfigurationViewHolder3_, scanPaneCategoryName_, "XAS", scanPaneIcon_);
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

