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


#include "SGMAppController.h"

#include "actions3/actions/AMScanAction.h"
#include "acquaman/AMGenericStepScanConfiguration.h"
#include "acquaman/SGM/SGMXASScanConfiguration.h"
#include "acquaman/SGM/SGMLineScanConfiguration.h"
#include "acquaman/SGM/SGMMapScanConfiguration.h"
#include "application/AMAppControllerSupport.h"

#include "application/AMPluginsManager.h"

#include "beamline/SGM/SGMBeamline.h"
#include "beamline/SGM/SGMHexapod.h"
#include "beamline/SGM/energy/SGMEnergyPosition.h"
#include "beamline/SGM/energy/SGMEnergyControlSet.h"
#include "dataman/AMRun.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/SGM/SGMUserConfiguration.h"
#include "dataman/export/AMExporterXDIFormat.h"
#include "dataman/export/AMExporterOptionXDIFormat.h"
#include "ui/AMMainWindow.h"
#include "ui/acquaman/AMGenericStepScanConfigurationView.h"
#include "ui/CLS/CLSAMDSScalerView.h"
#include "ui/CLS/CLSSIS3820ScalerView.h"
#include "ui/CLS/CLSAmptekSDD123DetailedDetectorView.h"
#include "ui/SGM/SGMHexapodView.h"
#include "ui/SGM/SGMPersistentView.h"
#include "ui/SGM/SGMEnergyView.h"
#include "ui/SGM/SGMLaddersView.h"
#include "ui/SGM/SGMSampleChamberView.h"
#include "ui/SGM/SGMXASScanConfigurationView.h"
#include "ui/SGM/SGMLineScanConfigurationView.h"
#include "ui/SGM/SGMMapScanConfigurationView.h"
#include "util/AMErrorMonitor.h"
#include "ui/beamline/AMDetectorView.h"

SGMAppController::SGMAppController(QObject *parent) :
	CLSAppController(CLSAppController::SGMBeamlineId, parent)
{
	// Ensure we're using local storage by default.
	setDefaultUseLocalStorage(true);

	userConfiguration_ = 0;
}

bool SGMAppController::startup() {

	// Run all of the Acquaman App startup routines. Some of these are reimplemented in this class.
	if(!CLSAppController::startup())
		return false;


//===TODO
		// Creates the SGM Beamline object
		SGMBeamline::sgm();

		// Initialize the storage ring.
		CLSStorageRing::sr1();	// Retrieve the current run or create one if there is none
	AMRun existingRun;
	if(!existingRun.loadFromDb(AMDatabase::database("user"), 1)) {
		// no run yet... let's create one.
		AMRun firstRun(CLSFacilityID::beamlineName(CLSFacilityID::SGMBeamline), CLSFacilityID::SGMBeamline); //3: SGM Beamline
		firstRun.storeToDb(AMDatabase::database("user"));
	}

	registerClasses();
	setupAMDSClientAppController();
	setupExporterOptions();
	setupUserInterface();
	makeConnections();
//===TODO ??
	if (!AMActionRegistry3::s()->registerInfoAndAction<AMSampleMoveActionInfo, AMSampleMoveAction>("Move to Sample", "Move the beam over a given sample", ":system-run.png"))
		return false;
	if(!AMActionRegistry3::s()->registerInfoAndEditor<AMSampleMoveActionInfo, AMSampleMoveActionEditor>())
		return false;

	// Places the SGM specific actions in the menu bar
	if(!startupSGMInstallActions())
		return false;

	// From Darren:  Adding this for SGM because they don't want to have scan editors to always be popping up automatically.
	setAutomaticBringScanEditorToFront(false);

	QList<int> allURLIds = AMDatabase::database("user")->objectsWhere(AMDbObjectSupport::s()->tableNameForClass<AMSampleCameraURL>());
	if(allURLIds.count() == 0){
		AMSampleCameraURL *newURL = new AMSampleCameraURL("http://10.52.48.103/axis-cgi/mjpg/video.cgi?resolution=1280x1024&.mjpg");
		newURL->storeToDb(AMDatabase::database("user"));
	}
//===TODO??

	if (!userConfiguration_){
		userConfiguration_ = new SGMUserConfiguration(this);

		// It is sufficient to only connect the user configuration to the single element because the single element and four element are synchronized together.
		connect(userConfiguration_, SIGNAL(loadedFromDb()), this, SLOT(onUserConfigurationLoadedFromDb()));

		if (!userConfiguration_->loadFromDb(AMDatabase::database("user"), 1)){
			userConfiguration_->storeToDb(AMDatabase::database("user"));

			AMDetector *detector = SGMBeamline::sgm()->amptekSDD1();
			// This is connected here because we want to listen to the detectors for updates, but don't want to double add regions on startup.
			connect(detector, SIGNAL(addedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestAdded(AMRegionOfInterest*)));
			connect(detector, SIGNAL(removedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestRemoved(AMRegionOfInterest*)));
		}
	}

	return true;
}


void SGMAppController::shutdown() {
	// Make sure we release/clean-up the beamline interface
	AMBeamline::releaseBl();
	AMAppController::shutdown();
	AMDSClientAppController::releaseAppController();
}


bool SGMAppController::startupCreateDatabases(){
	// Create the standard databases in AMAppController (user db)
	if(!AMAppController::startupCreateDatabases())
		return false;

	// Create the SGM beamline database
	AMDatabase* dbSGM = AMDatabase::createDatabase("SGMBeamline", SGMSettings::s()->SGMDataFolder() + "/" + SGMSettings::s()->SGMDatabaseFilename());
	if(!dbSGM) {
		AMErrorMon::alert(this, SGMAPPCONTROLLER_COULD_NOT_CREATE_SGM_DATABASE, "Error creating the SGM Database. Please report this problem to the Acquaman developers.");
		return false;
	}

	// Create the public SGM beamline database
	AMDatabase* dbSGMPublic = AMDatabase::createDatabase("SGMPublic", SGMSettings::s()->SGMDataFolder() + "/" + SGMSettings::s()->SGMPublicDatabaseFilename());
	if(!dbSGMPublic) {
		AMErrorMon::alert(this, SGMAPPCONTROLLER_COULD_NOT_CREATE_SGM_PUBLIC_DATABASE, "Error creating the SGM Public Database. Please report this problem to the Acquaman developers.");
		return false;
	}

	return true;
}

bool SGMAppController::startupRegisterDatabases(){
	// Call the AMAppController database registrations
	if(!AMAppController::startupRegisterDatabases())
		return false;

	// Grab the SGM Beamline database
	AMDatabase *dbSGM = AMDatabase::database("SGMBeamline");
	if(!dbSGM)
		return false;

	// Register the SGM Beamline database
	if(!AMDbObjectSupport::s()->registerDatabase(dbSGM)) {
		AMErrorMon::alert(this, SGMAPPCONTROLLER_COULD_NOT_REGISTER_SGM_DATABASE, "Error registering the SGM Database. Please report this problem to the Acquaman developers.");
		return false;
	}

	// Grab the SGM Public database
	AMDatabase *dbSGMPublic = AMDatabase::database("SGMPublic");
	if(!dbSGMPublic)
		return false;

	// Register the SGM Pubilc database
	if(!AMDbObjectSupport::s()->registerDatabase(dbSGMPublic)) {
		AMErrorMon::alert(this, SGMAPPCONTROLLER_COULD_NOT_REGISTER_SGM_PUBLIC_DATABASE, "Error registering the SGM Public Database. Please report this problem to the Acquaman developers.");
		return false;
	}

	return true;
}

bool SGMAppController::startupPopulateNewDatabase(){
	// Call the AMAppController database population methods
	if(!AMAppController::startupPopulateNewDatabase())
		return false;

	bool success = true;

	// Call the SGM setup functions. These are done both for new and existing databases, as these functions check for existance before creating and sometimes alter the existing if need be
	success &= setupSGMPlugins();
	success &= setupSGMPeriodicTable();
	success &= setupSGMExporterOptions();

	return success;
}

bool SGMAppController::startupLoadFromExistingDatabase(){
	// Call the AMAppController database loading methods
	if(!AMAppController::startupLoadFromExistingDatabase())
		return false;

	bool success = true;

	// Call the SGM setup functions. These are done both for new and existing databases, as these functions check for existance before creating and sometimes alter the existing if need be
	success &= setupSGMPlugins();
	success &= setupSGMPeriodicTable();
	success &= setupSGMExporterOptions();

	return success;
}

bool SGMAppController::startupAfterEverything(){
	if(isBadDatabaseDirectory_)
		QMessageBox::warning(0, "Poor Choice of Database Folder", "The folder choosen for the user database is really not a good one.\n\nAcquaman will work, but some features may experience problems.\nThis can be fixed, please contact Tom or David to fix this.");

	return AMAppController::startupAfterEverything();
}

void SGMAppController::onCurrentPaneChanged(QWidget *pane) {
	Q_UNUSED(pane)
}

void SGMAppController::onSGMBeamlineConnected(){
	//if(SGMBeamline::sgm()->isConnected() && SGMBeamline::sgm()->isReady() && !xasScanConfiguration2013View_ && !fastScanConfiguration2013View_){
	if(SGMBeamline::sgm()->isConnected() && !xasScanConfiguration2013View_ && !fastScanConfiguration2013View_){
		double goodEnergy = 10 * floor(SGMBeamline::sgm()->energy()->value() / 10);
		// Do New XAS
		SGMXASScanConfiguration2013 *xasScanConfiguration2013 = new SGMXASScanConfiguration2013(this);
		xasScanConfiguration2013->scanAxisAt(0)->regionAt(0)->setRegionStart(goodEnergy);
		xasScanConfiguration2013->scanAxisAt(0)->regionAt(0)->setRegionStep(1);
		xasScanConfiguration2013->scanAxisAt(0)->regionAt(0)->setRegionEnd(goodEnergy+9);
		xasScanConfiguration2013->scanAxisAt(0)->regionAt(0)->setRegionTime(1);
		xasScanConfiguration2013->setDetectorConfigurations(SGMBeamline::sgm()->XASDetectorGroup()->connectedDetectors()->toInfoSet());
		xasScanConfiguration2013->setTrackingGroup(SGMBeamline::sgm()->trackingSet()->toInfoList());
		xasScanConfiguration2013->setFluxResolutionGroup(SGMBeamline::sgm()->fluxResolutionSet()->toInfoList());

		xasDetectorSelector_ = new AMDetectorSelector(SGMBeamline::sgm()->XASDetectorGroup());
		QStringList preferentialOrdering;
		if(SGMBeamline::sgm()->newAmptekSDD1()){
			preferentialOrdering << SGMBeamline::sgm()->newAmptekSDD1()->name();
			xasDetectorSelector_->setDetectorDefault(SGMBeamline::sgm()->newAmptekSDD1(), true);
		}
		if(SGMBeamline::sgm()->newAmptekSDD2()){
			preferentialOrdering << SGMBeamline::sgm()->newAmptekSDD2()->name();
			xasDetectorSelector_->setDetectorDefault(SGMBeamline::sgm()->newAmptekSDD2(), true);
		}
		if(SGMBeamline::sgm()->newAmptekSDD3()){
			preferentialOrdering << SGMBeamline::sgm()->newAmptekSDD3()->name();
			xasDetectorSelector_->setDetectorDefault(SGMBeamline::sgm()->newAmptekSDD3(), true);
		}
		if(SGMBeamline::sgm()->newAmptekSDD4()){
			preferentialOrdering << SGMBeamline::sgm()->newAmptekSDD4()->name();
			xasDetectorSelector_->setDetectorDefault(SGMBeamline::sgm()->newAmptekSDD4(), true);
		}
		if(SGMBeamline::sgm()->newAmptekSDD5()){
			preferentialOrdering << SGMBeamline::sgm()->newAmptekSDD5()->name();
			xasDetectorSelector_->setDetectorDefault(SGMBeamline::sgm()->newAmptekSDD5(), true);
		}
		if(SGMBeamline::sgm()->newI0Detector()){
			preferentialOrdering << SGMBeamline::sgm()->newI0Detector()->name();
			xasDetectorSelector_->setDetectorDefault(SGMBeamline::sgm()->newI0Detector(), true);
		}
		if(SGMBeamline::sgm()->newTEYDetector()){
			preferentialOrdering << SGMBeamline::sgm()->newTEYDetector()->name();
			xasDetectorSelector_->setDetectorDefault(SGMBeamline::sgm()->newTEYDetector(), true);
		}
		if(SGMBeamline::sgm()->newTFYDetector()){
			preferentialOrdering << SGMBeamline::sgm()->newTFYDetector()->name();
		}
		if(SGMBeamline::sgm()->newPDDetector()){
			preferentialOrdering << SGMBeamline::sgm()->newPDDetector()->name();
		}
		if(SGMBeamline::sgm()->energyFeedbackDetector()) {
			xasDetectorSelector_->setDetectorDefault(SGMBeamline::sgm()->energyFeedbackDetector(), true);
		}
		if(SGMBeamline::sgm()->dwellTimeDetector()) {
			xasDetectorSelector_->setDetectorDefault(SGMBeamline::sgm()->dwellTimeDetector(), true);
		}

		xasDetectorSelector_->setPreferentialOrdering(preferentialOrdering);
		xasDetectorSelector_->setDefaultsSelected();

		xasScanConfiguration2013View_ = new SGMXASScanConfiguration2013View(xasScanConfiguration2013);
		xasScanConfiguration2013View_->setDetectorSelector(xasDetectorSelector_);
		/*
		 removed as temporary fix (see Issue579)
		xasScanConfiguration2013View_->setTrackingSet(SGMBeamline::sgm()->trackingSet());
		*/
		xasScanConfiguration2013Holder3_->setView(xasScanConfiguration2013View_);
		//End New XAS

		// Do New Fast
		SGMFastScanConfiguration2013 *fastScanConfiguration2013 = new SGMFastScanConfiguration2013(this);
		fastScanConfiguration2013->setParametersFromPreset(0);

		fastDetectorSelector_ = new AMDetectorSelector(SGMBeamline::sgm()->FastDetectorGroup());
		QStringList preferentialFastOrdering;
		preferentialFastOrdering << SGMBeamline::sgm()->newTEYDetector()->name();
		preferentialFastOrdering << SGMBeamline::sgm()->newI0Detector()->name();
		preferentialFastOrdering << SGMBeamline::sgm()->newTFYDetector()->name();
		preferentialFastOrdering << SGMBeamline::sgm()->newPDDetector()->name();
		fastDetectorSelector_->setPreferentialOrdering(preferentialFastOrdering);
		fastDetectorSelector_->setDetectorDefault(SGMBeamline::sgm()->newTEYDetector(), true);
		fastDetectorSelector_->setDetectorDefault(SGMBeamline::sgm()->newI0Detector(), true);
		fastDetectorSelector_->setDetectorDefault(SGMBeamline::sgm()->newTFYDetector(), true);
		fastDetectorSelector_->setDetectorDefault(SGMBeamline::sgm()->newPDDetector(), true);
		fastDetectorSelector_->setDetectorDefault(SGMBeamline::sgm()->newFilteredPD1Detector(), true);
		fastDetectorSelector_->setDetectorDefault(SGMBeamline::sgm()->newFilteredPD2Detector(), true);
		fastDetectorSelector_->setDetectorDefault(SGMBeamline::sgm()->newFilteredPD3Detector(), true);
		fastDetectorSelector_->setDetectorDefault(SGMBeamline::sgm()->newFilteredPD4Detector(), true);
		fastDetectorSelector_->setDetectorDefault(SGMBeamline::sgm()->newFilteredPD5Detector(), true);
		fastDetectorSelector_->setDefaultsSelected();

		fastScanConfiguration2013View_ = new SGMFastScanConfiguration2013View(fastScanConfiguration2013);
		fastScanConfiguration2013View_->setDetectorSelector(fastDetectorSelector_);
		fastScanConfiguration2013Holder3_->setView(fastScanConfiguration2013View_);
		// End New Fast
	}
	else if(!SGMBeamline::sgm()->isConnected() && !xasScanConfiguration2013View_ && !fastScanConfiguration2013View_){
		//do nothing
	}
	else if(SGMBeamline::sgm()->isConnected() && !xasScanConfiguration2013Holder3_->isEnabled() && !fastScanConfiguration2013Holder3_->isEnabled()){
		xasScanConfiguration2013Holder3_->setEnabled(true);
		fastScanConfiguration2013Holder3_->setEnabled(true);
	}
	else if(!SGMBeamline::sgm()->isConnected() && xasScanConfiguration2013View_->isEnabled() && fastScanConfiguration2013View_->isEnabled()){
		xasScanConfiguration2013Holder3_->setEnabled(false);
		fastScanConfiguration2013Holder3_->setEnabled(false);
	}

	if(!checkedBadStartupSettings_){
		checkedBadStartupSettings_ = true;
		QString badStartupSettingsMessage = "The beamline seems to have some odd settings, did a fast scan fail?\n\n";

		if(!SGMBeamline::sgm()->gratingVelocity()->withinTolerance(10000))
			badStartupSettingsMessage.append(QString("Grating Velocity %1 versus 10000\n").arg(SGMBeamline::sgm()->gratingVelocity()->value()));
		if(!SGMBeamline::sgm()->gratingBaseVelocity()->withinTolerance(0))
			badStartupSettingsMessage.append(QString("Grating Base Velocity %2 versus 0\n").arg(SGMBeamline::sgm()->gratingBaseVelocity()->value()));
		if(!SGMBeamline::sgm()->gratingAcceleration()->withinTolerance(5000))
			badStartupSettingsMessage.append(QString("Grating Acceleration %3 versus 5000\n").arg(SGMBeamline::sgm()->gratingAcceleration()->value()));
		if(!SGMBeamline::sgm()->rawScaler()->isConnected())
			badStartupSettingsMessage.append("Scaler is not connected\n");
		else if(SGMBeamline::sgm()->rawScaler()->scansPerBuffer() != 1)
			badStartupSettingsMessage.append(QString("Scaler time %1\nScaler Scan per buffer %2 versus 1\nScaler Total Scans %3 versus 1").arg(SGMBeamline::sgm()->rawScaler()->dwellTime()).arg(SGMBeamline::sgm()->rawScaler()->scansPerBuffer()).arg(SGMBeamline::sgm()->rawScaler()->totalScans()));

		if(badStartupSettingsMessage != "The beamline seems to have some odd settings, did a fast scan fail?\n\n"){
			badStartupSettingsMessage.append("\nPress Ok to automatically return to nominal values.");
			QMessageBox badStartupSettingsChoice;
			badStartupSettingsChoice.setInformativeText(badStartupSettingsMessage);
			badStartupSettingsChoice.setText(QString("Questionable Beamline Settings"));
			badStartupSettingsChoice.setIcon(QMessageBox::Question);
			badStartupSettingsChoice.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
			badStartupSettingsChoice.setDefaultButton(QMessageBox::Ok);
			badStartupSettingsChoice.setEscapeButton(QMessageBox::Cancel);

			if(badStartupSettingsChoice.exec() == QMessageBox::Ok){
				AMAction3 *cleanupSettingAction = SGMBeamline::sgm()->createRestorePreFastScanDefaultActions();
				connect(cleanupSettingAction, SIGNAL(succeeded()), cleanupSettingAction, SLOT(deleteLater()));
				connect(cleanupSettingAction, SIGNAL(failed()), cleanupSettingAction, SLOT(deleteLater()));
				cleanupSettingAction->start();
			}
		}
	}
}

void SGMAppController::onSGMScalerConnected(bool connected){
	Q_UNUSED(connected)
	if(SGMBeamline::sgm()->rawScaler() && SGMBeamline::sgm()->rawScaler()->isConnected() && !sgmScalerView_){
		sgmScalerView_ = new SGMSIS3820ScalerView(SGMBeamline::sgm()->scaler());
		sgmScalerView_->setAmplifierViewFormat('g');
		sgmScalerView_->setAmplifierViewPrecision(3);
		mw_->addPane(sgmScalerView_, "Beamline Detectors", "SGM Scaler", ":/system-software-update.png", true);
	}
}

void SGMAppController::onSGMSynchronizedDwellTimeConnected(bool connected){
	Q_UNUSED(connected)
	if(SGMBeamline::sgm()->synchronizedDwellTime() && SGMBeamline::sgm()->synchronizedDwellTime()->isConnected() && !sgmSynchronizedDwellTimeView_){

		CLSSynchronizedDwellTime *clsDwellTime = qobject_cast<CLSSynchronizedDwellTime*>(SGMBeamline::sgm()->synchronizedDwellTime());
		if(clsDwellTime)
			sgmSynchronizedDwellTimeView_ = new CLSSynchronizedDwellTimeView(clsDwellTime);

		mw_->addPane(sgmSynchronizedDwellTimeView_, "Beamline Detectors", "SGM Sync Dwell", ":/system-software-update.png", true);
		sgmSynchronizedDwellTimeView_->setAdvancedViewVisible(true);
	}
}

void SGMAppController::onSGMNewAmptekSDD1Connected(bool connected){
	Q_UNUSED(connected)

	if(SGMBeamline::sgm()->newAmptekSDD1() && SGMBeamline::sgm()->newAmptekSDD1()->isConnected() && !amptekSDD1XRFView_){
		amptekSDD1XRFView_ = new CLSAmptekDetailedDetectorView(qobject_cast<CLSAmptekSDD123DetectorNew*>(SGMBeamline::sgm()->newAmptekSDD1()));
		amptekSDD1XRFView_->buildDetectorView();
		amptekSDD1XRFView_->setEnergyRange(270, 2000);
		amptekSDD1XRFView_->collapsePeriodTableViews();
		mw_->addPane(amptekSDD1XRFView_, "Beamline Detectors", "SGM Amptek1 XRF", ":/system-software-update.png");
		connect(amptekSDD1XRFView_, SIGNAL(resized()), this, SLOT(onXRFDetectorViewResized()));
	}
}

void SGMAppController::onSGMNewAmptekSDD2Connected(bool connected){
	Q_UNUSED(connected)
	if(SGMBeamline::sgm()->newAmptekSDD2() && SGMBeamline::sgm()->newAmptekSDD2()->isConnected() && !amptekSDD2XRFView_){
		amptekSDD2XRFView_ = new CLSAmptekDetailedDetectorView(qobject_cast<CLSAmptekSDD123DetectorNew*>(SGMBeamline::sgm()->newAmptekSDD2()));
		amptekSDD2XRFView_->buildDetectorView();
		amptekSDD2XRFView_->setEnergyRange(270, 2000);
		amptekSDD2XRFView_->collapsePeriodTableViews();
		mw_->addPane(amptekSDD2XRFView_, "Beamline Detectors", "SGM Amptek2 XRF", ":/system-software-update.png");
		connect(amptekSDD2XRFView_, SIGNAL(resized()), this, SLOT(onXRFDetectorViewResized()));
	}
}

void SGMAppController::onSGMNewAmptekSDD3Connected(bool connected){
	Q_UNUSED(connected)
	if(SGMBeamline::sgm()->newAmptekSDD3() && SGMBeamline::sgm()->newAmptekSDD3()->isConnected() && !amptekSDD3XRFView_){
		amptekSDD3XRFView_ = new CLSAmptekDetailedDetectorView(qobject_cast<CLSAmptekSDD123DetectorNew*>(SGMBeamline::sgm()->newAmptekSDD3()));
		amptekSDD3XRFView_->buildDetectorView();
		amptekSDD3XRFView_->setEnergyRange(270, 2000);
		amptekSDD3XRFView_->collapsePeriodTableViews();
		mw_->addPane(amptekSDD3XRFView_, "Beamline Detectors", "SGM Amptek3 XRF", ":/system-software-update.png");
		connect(amptekSDD3XRFView_, SIGNAL(resized()), this, SLOT(onXRFDetectorViewResized()));
	}
}

void SGMAppController::onSGMNewAmptekSDD4Connected(bool connected){
	Q_UNUSED(connected)
	if(SGMBeamline::sgm()->newAmptekSDD4() && SGMBeamline::sgm()->newAmptekSDD4()->isConnected() && !amptekSDD4XRFView_){
		amptekSDD4XRFView_ = new CLSAmptekDetailedDetectorView(qobject_cast<CLSAmptekSDD123DetectorNew*>(SGMBeamline::sgm()->newAmptekSDD4()));
		amptekSDD4XRFView_->buildDetectorView();
		amptekSDD4XRFView_->setEnergyRange(270, 2000);
		amptekSDD4XRFView_->collapsePeriodTableViews();
		mw_->addPane(amptekSDD4XRFView_, "Beamline Detectors", "SGM Amptek4 XRF", ":/system-software-update.png");
		connect(amptekSDD4XRFView_, SIGNAL(resized()), this, SLOT(onXRFDetectorViewResized()));
	}
}

void SGMAppController::initializeAmptekView()
{
	if(!amptek1DetectorView_) {

		amptek1DetectorView_ = new CLSAmptekDetailedDetectorView(SGMBeamline::sgm()->amptekSDD1());
		amptek1DetectorView_->buildDetectorView();
		amptek1DetectorView_->setEnergyRange(270,2000);
		amptek1DetectorView_->collapsePeriodTableViews();
		mw_->addPane(amptek1DetectorView_, "Amptek Detectors", "Amptek Detector 1", ":/system-software-update.png");
		connect(amptek1DetectorView_, SIGNAL(resized()), this, SLOT(onAmptekDetectorViewResized()));
	}
	if(!amptek2DetectorView_) {

		amptek2DetectorView_ = new CLSAmptekDetailedDetectorView(SGMBeamline::sgm()->amptekSDD2());
		amptek2DetectorView_->buildDetectorView();
		amptek2DetectorView_->setEnergyRange(270,2000);
		amptek2DetectorView_->collapsePeriodTableViews();
		mw_->addPane(amptek2DetectorView_, "Amptek Detectors", "Amptek Detector 2", ":/system-software-update.png");
		connect(amptek2DetectorView_, SIGNAL(resized()), this, SLOT(onAmptekDetectorViewResized()));
	}
	if(!amptek3DetectorView_) {

		amptek3DetectorView_ = new CLSAmptekDetailedDetectorView(SGMBeamline::sgm()->amptekSDD3());
		amptek3DetectorView_->buildDetectorView();
		amptek3DetectorView_->setEnergyRange(270,2000);
		amptek3DetectorView_->collapsePeriodTableViews();
		mw_->addPane(amptek3DetectorView_, "Amptek Detectors", "Amptek Detector 3", ":/system-software-update.png");
		connect(amptek3DetectorView_, SIGNAL(resized()), this, SLOT(onAmptekDetectorViewResized()));
	}
	if(!amptek4DetectorView_) {

		amptek4DetectorView_ = new CLSAmptekDetailedDetectorView(SGMBeamline::sgm()->amptekSDD4());
		amptek4DetectorView_->buildDetectorView();
		amptek4DetectorView_->setEnergyRange(270,2000);
		amptek4DetectorView_->collapsePeriodTableViews();
		mw_->addPane(amptek4DetectorView_, "Amptek Detectors", "Amptek Detector 4", ":/system-software-update.png");
		connect(amptek4DetectorView_, SIGNAL(resized()), this, SLOT(onAmptekDetectorViewResized()));
	}
}

void SGMAppController::onAmptekDetectorViewResized()
{
	// Hack solution copied over from the old SGM App Controller.
	// If this timer is not here it doesn't always resize properly.
	QTimer::singleShot(100, this, SLOT(resizeToMinimum()));
}

void SGMAppController::resizeToMinimum()
{
	mw_->resize(mw_->minimumSizeHint());
}

void SGMAppController::onUserConfigurationLoadedFromDb()
{
	AMXRFDetector *detector = SGMBeamline::sgm()->amptekSDD1();

	foreach (AMRegionOfInterest *region, userConfiguration_->regionsOfInterest()){
		detector->addRegionOfInterest(region->createCopy());
		xasScanConfiguration_->addRegionOfInterest(region);
		lineScanConfiguration_->addRegionOfInterest(region);
		mapScanConfiguration_->addRegionOfInterest(region);
	}

	// This is connected here because we want to listen to the detectors for updates, but don't want to double add regions on startup.
	connect(detector, SIGNAL(addedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestAdded(AMRegionOfInterest*)));
	connect(detector, SIGNAL(removedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestRemoved(AMRegionOfInterest*)));
}

void SGMAppController::onRegionOfInterestAdded(AMRegionOfInterest *region)
{
	userConfiguration_->addRegionOfInterest(region);
	xasScanConfiguration_->addRegionOfInterest(region);
	lineScanConfiguration_->addRegionOfInterest(region);
	mapScanConfiguration_->addRegionOfInterest(region);
}

void SGMAppController::onRegionOfInterestRemoved(AMRegionOfInterest *region)
{
	userConfiguration_->removeRegionOfInterest(region);
	xasScanConfiguration_->removeRegionOfInterest(region);
	lineScanConfiguration_->removeRegionOfInterest(region);
	mapScanConfiguration_->removeRegionOfInterest(region);
}

void SGMAppController::connectAMDSServers()
{
	AMDSClientAppController *clientAppController = AMDSClientAppController::clientAppController();
	foreach (AMDSServerConfiguration serverConfiguration, AMDSServerDefs_.values())
		clientAppController->connectToServer(serverConfiguration);
}

void SGMAppController::onAMDSServerConnected(const QString &hostIdentifier)
{
	SGMBeamline::sgm()->configAMDSServer(hostIdentifier);
}

void SGMAppController::setupAMDSClientAppController()
{
	AMDSServerDefs_.insert(QString("AmptekServer"), AMDSServerConfiguration(QString("AmptekServer"), QString("10.52.48.40"), 28044));
	AMDSServerDefs_.insert(QString("ScalerServer"), AMDSServerConfiguration(QString("ScalerServer"), QString("10.52.48.1"), 28044));

	// NOTE: it will be better to move this to CLSBeamline, when
	AMDSClientAppController *AMDSClientController = AMDSClientAppController::clientAppController();
	connect(AMDSClientController, SIGNAL(AMDSClientControllerConnected()), this, SLOT(connectAMDSServers()));
	connect(AMDSClientController, SIGNAL(newServerConnected(QString)), this, SLOT(onAMDSServerConnected(QString)));
	if (AMDSClientController->isSessionOpen()) {
		connectAMDSServers();
	}
}

void SGMAppController::onCurrentScanActionStartedImplementation(AMScanAction */*action*/)
{
	userConfiguration_->storeToDb(AMDatabase::database("user"));
}

void SGMAppController::onCurrentScanActionFinishedImplementation(AMScanAction *action)
{
	const AMScanActionInfo *actionInfo = qobject_cast<const AMScanActionInfo *>(action->info());
	const AMGenericContinuousScanConfiguration *sgmScanConfig = dynamic_cast<const AMGenericContinuousScanConfiguration *>(actionInfo->configuration());

	if (sgmScanConfig){

		userConfiguration_->storeToDb(AMDatabase::database("user"));
	}
}

void SGMAppController::registerClasses()
{
	AMDbObjectSupport::s()->registerClass<SGMXASScanConfiguration>();
	AMDbObjectSupport::s()->registerClass<SGMLineScanConfiguration>();
	AMDbObjectSupport::s()->registerClass<SGMMapScanConfiguration>();
	AMDbObjectSupport::s()->registerClass<SGMUserConfiguration>();
}

void SGMAppController::setupExporterOptions()
{
	AMExporterOptionXDIFormat *sgmXASExportOptions = SGM::buildXDIFormatExporterOption("SGMXASDefault", true);
	if(sgmXASExportOptions->id() > 0)
		AMAppControllerSupport::registerClass<SGMXASScanConfiguration, AMExporterXDIFormat, AMExporterOptionXDIFormat>(sgmXASExportOptions->id());

	AMExporterOptionGeneralAscii *sgmLineExportOptions = SGM::buildAsciiFormatExporterOption("SGMLineDefault", true);
	if(sgmLineExportOptions->id() > 0)
		AMAppControllerSupport::registerClass<SGMLineScanConfiguration, AMExporterGeneralAscii, AMExporterOptionGeneralAscii>(sgmLineExportOptions->id());

	AMExporterOptionGeneralAscii *sgmMapExportOptions = SGM::buildAsciiFormatExporterOption("SGMMapDefault", true);
	if(sgmMapExportOptions->id() > 0)
		AMAppControllerSupport::registerClass<SGMMapScanConfiguration, AMExporterGeneralAscii, AMExporterOptionGeneralAscii>(sgmMapExportOptions->id());

}

void SGMAppController::setupUserInterface()
{
	SGMPersistentView* persistentView =
			new SGMPersistentView();

	mw_->addRightWidget(persistentView);

	CLSAMDSScalerView *amdsScalerView = new CLSAMDSScalerView(SGMBeamline::sgm()->amdsScaler());
	amdsScalerView->setAmplifierViewFormat('g');
	amdsScalerView->setAmplifierViewPrecision(3);

	mw_->insertHeading("Components", 0);

	mw_->addPane(AMMainWindow::buildMainWindowPane("AMDS Scaler", ":/system-software-update.png", amdsScalerView),"Components", "AMDS Scaler",  ":/system-software-update.png");

	SGMHexapodView* hexapodView =
			new SGMHexapodView(SGMBeamline::sgm()->hexapod());

	mw_->addPane(AMMainWindow::buildMainWindowPane("Hexapod", ":/system-software-update.png", hexapodView), "Components", "Hexapod", ":/system-software-update.png");

	SGMEnergyView* energyView =
			new SGMEnergyView(SGMBeamline::sgm()->energyControlSet(),
							  SGMEnergyView::Advanced);

	mw_->addPane(AMMainWindow::buildMainWindowPane("Energy", ":/system-software-update.png", energyView), "Components", "Energy", ":/system-software-update.png");

	SGMLaddersView *laddersView =
			new SGMLaddersView(SGMBeamline::sgm()->xpsLadder(), SGMBeamline::sgm()->bypassLadder(), SGMBeamline::sgm()->xasLadder());

	mw_->addPane(AMMainWindow::buildMainWindowPane("Diagnostics", ":/system-software-update.png", laddersView), "Components", "Diagnostics", ":/system-software-update.png");

	SGMSampleChamberView *sampleChamberView =
			new SGMSampleChamberView(SGMBeamline::sgm()->sampleChamber());

	mw_->addPane(AMMainWindow::buildMainWindowPane("Sample Chamber", ":/system-software-update.png", sampleChamberView), "Components", "Sample Chamber", ":/system-software-update.png");

	mw_->insertHeading("Scans", 1);

	qe65000DetectorView_ = new AMDetectorGeneralDetailedView(SGMBeamline::sgm()->qe6500Detector());
	mw_->addPane(qe65000DetectorView_, "Beamline Detectors", "SGM QE 65000", ":/system-software-update.png");

	commissioningStepConfiguration_ = new AMGenericStepScanConfiguration;
	commissioningStepConfiguration_->setAutoExportEnabled(false);
	commissioningStepConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("I0")->toInfo());
	commissioningStepConfigurationView_ = new AMGenericStepScanConfigurationView(commissioningStepConfiguration_, SGMBeamline::sgm()->exposedControls(), SGMBeamline::sgm()->exposedScientificDetectors());
	commissioningStepConfigurationViewHolder_ = new AMScanConfigurationViewHolder3("Commissioning Tool", false, true, commissioningStepConfigurationView_);

	xasScanConfiguration_ = new SGMXASScanConfiguration;
	xasScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStart(270);
	xasScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionEnd(320);
	xasScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionTime(10);
	xasScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("TEY")->toInfo());
	xasScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("ConstantFrequency")->toInfo());
	xasScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("I0")->toInfo());
	xasScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("PD")->toInfo());
	xasScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("EncoderUp")->toInfo());
	xasScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("EncoderDown")->toInfo());
	xasScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("HexapodRed")->toInfo());
	xasScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("HexapodBlack")->toInfo());
	xasScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekSDD1")->toInfo());
	xasScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekSDD2")->toInfo());
	xasScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekSDD3")->toInfo());
	xasScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekSDD4")->toInfo());
	xasScanConfiguration_->setI0(SGMBeamline::sgm()->exposedDetectorByName("ConstantFrequency")->toInfo());
	xasScanConfigurationView_ = new SGMXASScanConfigurationView(xasScanConfiguration_, AMBeamline::bl()->exposedScientificDetectors());
	xasScanConfigurationViewHolder_ = new AMScanConfigurationViewHolder3("XAS", false, true, xasScanConfigurationView_);

	lineScanConfiguration_ = new SGMLineScanConfiguration;
	lineScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStart(-1);
	lineScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionEnd(1);
	lineScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionTime(10);
	lineScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("ConstantFrequency")->toInfo());
	lineScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("I0")->toInfo());
	lineScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("HexapodRed")->toInfo());
	lineScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("HexapodBlack")->toInfo());
	lineScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekSDD1")->toInfo());
	lineScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekSDD2")->toInfo());
	lineScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekSDD3")->toInfo());
//	lineScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekSDD4")->toInfo());
	lineScanConfigurationView_ = new SGMLineScanConfigurationView(lineScanConfiguration_, SGMBeamline::sgm()->hexapodControlSet(), AMBeamline::bl()->exposedScientificDetectors());
	lineScanConfigurationViewHolder_ = new AMScanConfigurationViewHolder3("Line", false, true, lineScanConfigurationView_);

	mapScanConfiguration_ = new SGMMapScanConfiguration;
	mapScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStart(0);
	mapScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionEnd(0.5);
	mapScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionTime(10);
	mapScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionStart(-0.5);
	mapScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionStep(0.1);
	mapScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionEnd(0);
	mapScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionTime(10);
	mapScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("TEY")->toInfo());
	mapScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("ConstantFrequency")->toInfo());
	mapScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("I0")->toInfo());
	mapScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("PD")->toInfo());
	mapScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekROI1")->toInfo());
	mapScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekROI2")->toInfo());
	mapScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekROI3")->toInfo());
	mapScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekROI4")->toInfo());
	mapScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("HexapodRed")->toInfo());
	mapScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("HexapodBlack")->toInfo());
	mapScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("EncoderUp")->toInfo());
	mapScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("EncoderDown")->toInfo());
	mapScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekSDD1")->toInfo());
	mapScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekSDD2")->toInfo());
	mapScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekSDD3")->toInfo());
	mapScanConfiguration_->addDetector(SGMBeamline::sgm()->exposedDetectorByName("AmptekSDD4")->toInfo());
	mapScanConfigurationView_ = new SGMMapScanConfigurationView(mapScanConfiguration_, AMBeamline::bl()->exposedScientificDetectors());
	mapScanConfigurationViewHolder_ = new AMScanConfigurationViewHolder3("Line", false, true, mapScanConfigurationView_);

	mw_->addPane(commissioningStepConfigurationViewHolder_, "Scans", "Commissioning Tool", ":/utilities-system-monitor.png");
	mw_->addPane(xasScanConfigurationViewHolder_, "Scans", "XAS", ":/utilities-system-monitor.png");
	mw_->addPane(lineScanConfigurationViewHolder_, "Scans", "Line", ":/utilities-system-monitor.png");
	mw_->addPane(mapScanConfigurationViewHolder_, "Scans", "Mapping", ":/utilities-system-monitor.png");

	amptek1DetectorView_ = 0;
	amptek2DetectorView_ = 0;
	amptek3DetectorView_ = 0;
	amptek4DetectorView_ = 0;
	if(SGMBeamline::sgm()->amptekSDD1() && SGMBeamline::sgm()->amptekSDD1()->isConnected()) {
		initializeAmptekView();
	} else if(SGMBeamline::sgm()->amptekSDD1()) {
		connect(SGMBeamline::sgm()->amptekSDD1(), SIGNAL(connected(bool)), this, SLOT(initializeAmptekView()));
	}
}

void SGMAppController::initializeBeamline()
{
	// Creates the SGM Beamline object
	SGMBeamline::sgm();

	connect(SGMBeamline::sgm(), SIGNAL(detectorAvailabilityChanged(AMDetector*,bool)), this, SLOT(onSGMBeamlineDetectorAvailabilityChanged(AMDetector*,bool)));

	AMErrorMon::information(this, AMDATAMANAPPCONTROLLER_STARTUP_MESSAGES, QString("SGM Startup: Waiting for detectors"));
	onSGMBeamlineDetectorAvailabilityChanged(0, false);
}

void SGMAppController::registerBeamlineDBClasses()
{
	// Register the detector and scan classes
	AMDbObjectSupport::s()->registerClass<SGMMCPDetectorInfo>();
	AMDbObjectSupport::s()->registerClass<CLSPGTDetectorInfo>();
	AMDbObjectSupport::s()->registerClass<CLSAmptekSDD123DetectorInfo>();
	AMDbObjectSupport::s()->registerClass<CLSOceanOptics65000DetectorInfo>();
	AMDbObjectSupport::s()->registerClass<SGMXASScanConfiguration>();
	AMDbObjectSupport::s()->registerClass<SGMFastScanConfiguration>();
	AMDbObjectSupport::s()->registerClass<SGMXASScanConfiguration2013>();
	AMDbObjectSupport::s()->registerClass<SGMFastScanConfiguration2013>();
	AMDbObjectSupport::s()->registerClass<SGMSScanConfigurationDbObject>();

	// Register the configuration file and file loader plugin supports
	AMDbObjectSupport::s()->registerClass<SGMPluginsLocation>();

	// Register the SGM periodic table support classes
	AMDbObjectSupport::s()->registerClass<SGMEnergyPosition>();
	AMDbObjectSupport::s()->registerClass<SGMScanInfo>();
	AMDbObjectSupport::s()->registerClass<SGMElementInfo>();
	AMDbObjectSupport::s()->registerClass<SGMFastScanSettings>();
	AMDbObjectSupport::s()->registerClass<SGMFastScanParameters>();
}

//====TODO
//// Register the configuration file and file loader plugin supports
//AMDbObjectSupport::s()->registerClass<SGMPluginsLocation>();

//// Register the SGM periodic table support classes
//AMDbObjectSupport::s()->registerClass<SGMEnergyPosition>();
//AMDbObjectSupport::s()->registerClass<SGMScanInfo>();
//AMDbObjectSupport::s()->registerClass<SGMElementInfo>();
//AMDbObjectSupport::s()->registerClass<SGMFastScanSettings>();
//AMDbObjectSupport::s()->registerClass<SGMFastScanParameters>();
//}

//void SGMAppController::setupBeamlineExporterOptions()
//{

//}

//void SGMAppController::setupAcquamanUserInterface()
//{

//}

//void SGMAppController::setupBeamlineSignalConnections()


void SGMAppController::setupExporterOptions()
{

}

#include "dataman/AMScanEditorModelItem.h"
#include "ui/dataman/AMGenericScanEditor.h"

void SGMAppController::onCurrentScanActionStartedImplementation(AMScanAction *action){
	Q_UNUSED(action)

	connect(CLSStorageRing::sr1(), SIGNAL(beamAvaliability(bool)), this, SLOT(onBeamAvailabilityChanged(bool)));
}

void SGMAppController::onCurrentScanActionFinishedImplementation(AMScanAction *action){
	Q_UNUSED(action)
	disconnect(CLSStorageRing::sr1(), SIGNAL(beamAvaliability(bool)), this, SLOT(onBeamAvailabilityChanged(bool)));
}

//TODO void SGMAppController::onBeamAvailabilityChanged(bool beamAvailable)

void SGMAppController::makeConnections()
{

}
