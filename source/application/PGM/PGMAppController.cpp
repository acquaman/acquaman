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

#include "acquaman/PGM/PGMXASScanConfiguration.h"

#include "beamline/AMControl.h"
#include "beamline/PGM/PGMBeamline.h"

#include "dataman/database/AMDbObjectSupport.h"

#include "ui/AMMainWindow.h"
#include "ui/acquaman/AMScanConfigurationViewHolder3.h"
#include "ui/beamline/AMXRFDetailedDetectorView.h"
#include "ui/dataman/AMGenericScanEditor.h"
#include "ui/CLS/CLSBeamlineStatusView.h"
#include "ui/CLS/CLSSynchronizedDwellTimeView.h"
#include "ui/CLS/CLSHVControlGroupView.h"
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
	pgmUserConfiguration_ = new PGMUserConfiguration(this);
	setDefaultUseLocalStorage(true);

	detectorPaneCategoryName_ = "XRF Detectors";
}

void PGMAppController::onUserConfigurationLoadedFromDb()
{
	if (pgmUserConfiguration_) {

		AMXRFDetector *oceanOpticsDetector = PGMBeamline::pgm()->oceanOpticsDetector();

		if (oceanOpticsDetector) {
			// connect the signal first, so that we can add the ROIs without worrying about the initialization
			connect(oceanOpticsDetector, SIGNAL(addedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestAdded(AMRegionOfInterest*)));
			connect(oceanOpticsDetector, SIGNAL(removedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestRemoved(AMRegionOfInterest*)));
			connect(oceanOpticsDetector, SIGNAL(regionOfInterestBoundingRangeChanged(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestBoundingRangeChanged(AMRegionOfInterest*)));

			foreach (AMRegionOfInterest *region, userConfiguration_->regionsOfInterest()){
				oceanOpticsDetector->addRegionOfInterest(region);
			}
		}
	}
}

void PGMAppController::onRegionOfInterestAdded(AMRegionOfInterest *region)
{
	if (region) {

		// Add the region of interest to the user configuration, if it doesn't have it already.

		if (userConfiguration_ )
			userConfiguration_->addRegionOfInterest(region);
	}
}

void PGMAppController::onRegionOfInterestRemoved(AMRegionOfInterest *region)
{
	if (userConfiguration_ )
		userConfiguration_->removeRegionOfInterest(region);
}

void PGMAppController::onRegionOfInterestBoundingRangeChanged(AMRegionOfInterest *region)
{
	if (userConfiguration_)
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
	CLSAppController::registerDBClasses();

	AMDbObjectSupport::s()->registerClass<PGMXASScanConfiguration>();
	AMDbObjectSupport::s()->registerClass<PGMUserConfiguration>();
}

void PGMAppController::registerExporterOptions()
{

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
	if (pgmUserConfiguration_) {

		connect( pgmUserConfiguration_, SIGNAL(loadedFromDb()), this, SLOT(onUserConfigurationLoadedFromDb()) );

		bool loaded = pgmUserConfiguration_->loadFromDb(AMDatabase::database("user"), 1);
		if (!loaded) {
			pgmUserConfiguration_->storeToDb(AMDatabase::database("user"));
			onUserConfigurationLoadedFromDb();
		}

		userConfiguration_ = pgmUserConfiguration_;
	}
}

void PGMAppController::setupUserInterfaceImplementation()
{
	mw_->setWindowTitle("Acquaman - VLS PGM");
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
	addMainWindowView( beamlineStatusView_, "Beamline status", generalPaneCategeryName_, generalPaneIcon_);

	CLSSynchronizedDwellTime *synchronizedDwellTime = qobject_cast<CLSSynchronizedDwellTime *>(AMBeamline::bl()->synchronizedDwellTime());
	CLSSynchronizedDwellTimeView *synchronizedDwellTimeView = new CLSSynchronizedDwellTimeView(synchronizedDwellTime);
	synchronizedDwellTimeView->setAdvancedViewVisible(true);
	addMainWindowView(synchronizedDwellTimeView, "Synchronized Dwell", generalPaneCategeryName_, generalPaneIcon_);

	addMainWindowView(new PGMBladeCurrentView, "Blade Currents", generalPaneCategeryName_, generalPaneIcon_);
	addMainWindowView(new PGMSlitControlView, "Slits", generalPaneCategeryName_, generalPaneIcon_);
	addMainWindowView(new PGMGratingView, "Mono Grating", generalPaneCategeryName_, generalPaneIcon_);
	addMainWindowView(new PGMUndulatorView, "Undulator", generalPaneCategeryName_, generalPaneIcon_);
	addMainWindowView(new PGMVariableApertureMaskView(PGMBeamline::pgm()->vam()), "Variable Aperture Mask", generalPaneCategeryName_, generalPaneIcon_);
	addMainWindowView(new CLSHVControlGroupView(PGMBeamline::pgm()->branchAHVControlSet(), PGMBeamline::pgm()->branchBHVControlSet(), false), "HV Conrols", generalPaneCategeryName_, generalPaneIcon_);
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

void PGMAppController::onScanEditorCreatedImplementation(AMGenericScanEditor *editor)
{
	Q_UNUSED(editor)
}

