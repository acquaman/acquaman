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

#include <QHostInfo>

#include "application/AMAppControllerSupport.h"
#include "application/AMPluginsManager.h"

#include "beamline/CLS/CLSFacilityID.h"
#include "beamline/SGM/SGMBeamline.h"
//#include "beamline/CLS/CLSProcServManager.h"
#include "beamline/AMDetectorSelector.h"
#include "beamline/AMDetectorSet.h"
#include "beamline/SGM/SGMMAXvMotor.h"
#include "beamline/camera/AMSampleCamera.h"
#include "beamline/camera/AMSampleCameraBrowser.h"
#include "beamline/CLS/CLSPGTDetectorV2.h"
#include "beamline/CLS/CLSAmptekSDD123DetectorNew.h"

#include "dataman/SGM/SGMDbUpgrade1Pt1.h"
#include "dataman/AMDbUpgrade1Pt1.h"
#include "dataman/AMDbUpgrade1Pt2.h"
#include "dataman/AMDbUpgrade1Pt4.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/AMRun.h"
#include "dataman/export/AMExporterOptionGeneralAscii.h"
#include "dataman/export/AMExporterGeneralAscii.h"
#include "dataman/export/AMExportController.h"
#include "dataman/export/SGM/SGMAxis2000Exporter.h"
#include "dataman/info/CLSOceanOptics65000DetectorInfo.h"
#include "dataman/info/CLSPGTDetectorInfo.h"
#include "dataman/info/CLSAmptekSDD123DetectorInfo.h"
#include "dataman/SGM/SGMMCPDetectorInfo.h"

#include "acquaman/AMScanController.h"
#include "acquaman/AMScanActionControllerScanAssembler.h"
#include "acquaman/SGM/SGMXASScanActionController.h"
#include "acquaman/SGM/SGMXASScanConfiguration2013.h"
#include "acquaman/SGM/SGMFastScanConfiguration2013.h"
#include "acquaman/AMAgnosticDataAPI.h"
#include "actions3/actions/AMSampleMoveActionInfo.h"
#include "actions3/actions/AMSampleMoveAction.h"
#include "actions3/editors/AMSampleMoveActionEditor.h"

#include "beamline/CLS/CLSStorageRing.h"

#include "actions3/AMActionRunner3.h"
#include "ui/actions3/AMWorkflowView3.h"

#include "util/SGM/SGMSettings.h"
#include "util/SGM/SGMPluginsLocation.h"
#include "util/SGM/SGMPeriodicTable.h"
#include "util/AMBuildReporter.h"

#include "ui/acquaman/AMScanConfigurationViewHolder3.h"
#include "ui/AMMotorGroupView.h"

#include "ui/SGM/SGMXASScanConfigurationView.h"
#include "ui/SGM/SGMFastScanConfigurationView.h"
#include "ui/SGM/SGMXASScanConfiguration2013View.h"
#include "ui/SGM/SGMFastScanConfiguration2013View.h"
#include "ui/SGM/SGMSidebar.h"
#include "ui/SGM/SGMAdvancedControlsView.h"
#include "ui/SGM/SGMPeriodicTableView.h"
#include "ui/SGM/SGMAdvancedMirrorView.h"
#include "ui/SGM/SGMSampleManagementView.h"
#include "ui/SGM/SGMSampleManipulatorView.h"
#include "ui/SGM/SGMSIS3820ScalerView.h"

#include "ui/CLS/CLSAmptekSDD123DetailedDetectorView.h"
#include "ui/CLS/CLSPGTDetectorV2View.h"
#include "ui/CLS/CLSAmptekSDD123DetectorNewView.h"
#include "ui/CLS/CLSSynchronizedDwellTimeView.h"

#include "ui/dataman/AMSampleManagementPre2013Widget.h"
#include "ui/dataman/AMSamplePlateView.h"

#include "ui/beamline/AMDetectorView.h"
#include "ui/beamline/AMXRFDetailedDetectorView.h"
#include "ui/beamline/AMDetectorSelectorView.h"
#include "ui/beamline/camera/AMSampleCameraBrowserView.h"
#include "ui/beamline/AMOldDetectorView.h"

#include "ui/AMMainWindow.h"
#include "ui/AMDatamanStartupSplashScreen.h"

#include "ui/util/SGM/SGMSettingsMasterView.h"
#include "ui/util/AMGithubIssueSubmissionView.h"



#include "actions3/actions/AMControlMoveAction3.h"
#include "actions3/AMActionRegistry3.h"


SGMAppController::SGMAppController(QObject *parent) :
	AMAppController(parent)
{
	const QString userNameSGM = "sgm";
	const QString argumentEnableSGMDatabaseUpgrade = "--enableSGMDatabaseUpgrade";

	setDefaultUseLocalStorage(true);

	if(!resetFinishedSignal(SGMBeamline::sgm(), SIGNAL(beamlineInitialized())))
		AMErrorMon::alert(this, SGMAPPCONTROLLER_COULD_NOT_RESET_FINISHED_SIGNAL, "Could not reset the finish signal to the SGM Beamline");

	// Prepend the SGM upgrade 1.1 to the list for both the user database and the SGM Beamline database
	AMDbUpgrade *sgm1Pt1SGMDb = new SGMDbUpgrade1Pt1("SGMBeamline", this);
	AMDbUpgrade *sgm1Pt1SGMPublicDb = new SGMDbUpgrade1Pt1("SGMPublic", this);

	// Don't need to do SGMBeamline ... that's not the user's responsibility unless we're SGM or fawkes
	bool hasUpgradePermission = false;
	QString userName = QDir::fromNativeSeparators(QDir::homePath()).section("/", -1);
	if (userName == userNameSGM || QApplication::instance()->arguments().contains(argumentEnableSGMDatabaseUpgrade)) {
		hasUpgradePermission = true;
	}

	sgm1Pt1SGMDb->setIsResponsibleForUpgrade(hasUpgradePermission);
	sgm1Pt1SGMPublicDb->setIsResponsibleForUpgrade(hasUpgradePermission);

	prependDatabaseUpgrade(sgm1Pt1SGMDb);
	prependDatabaseUpgrade(sgm1Pt1SGMPublicDb);

	AMDbUpgrade *sgm1Pt1UserDb = new SGMDbUpgrade1Pt1("user", this);
	AMDbUpgrade *sgm1Pt1ActionsDb = new SGMDbUpgrade1Pt1("actions", this);
//	sgm1Pt1UserDb->setIsResponsibleForUpgrade(hasUpgradePermission);;
//	sgm1Pt1ActionsDb->setIsResponsibleForUpgrade(hasUpgradePermission);;
	prependDatabaseUpgrade(sgm1Pt1UserDb);
	prependDatabaseUpgrade(sgm1Pt1ActionsDb);

	// Append the AM upgrade 1.1 to the list for the SGMBeamline database and public database
	AMDbUpgrade *am1Pt1SGMDb = new AMDbUpgrade1Pt1("SGMBeamline", this);
	AMDbUpgrade *am1Pt1SGMPublicDb = new AMDbUpgrade1Pt1("SGMPublic", this);
	am1Pt1SGMDb->setIsResponsibleForUpgrade(hasUpgradePermission);;
	am1Pt1SGMPublicDb->setIsResponsibleForUpgrade(hasUpgradePermission);;
	appendDatabaseUpgrade(am1Pt1SGMDb);
	appendDatabaseUpgrade(am1Pt1SGMPublicDb);

	// Append the AM upgrade 1.2 to the list for the SGMBeamline database and public database
	AMDbUpgrade *am1Pt2SGMDb = new AMDbUpgrade1Pt2("SGMBeamline", this);
	AMDbUpgrade *am1Pt2SGMPublicDb = new AMDbUpgrade1Pt2("SGMPublic", this);
	am1Pt2SGMDb->setIsResponsibleForUpgrade(hasUpgradePermission);;
	am1Pt2SGMPublicDb->setIsResponsibleForUpgrade(hasUpgradePermission);;
	appendDatabaseUpgrade(am1Pt2SGMDb);
	appendDatabaseUpgrade(am1Pt2SGMPublicDb);

	// Append the AM upgrade 1.4 to the list for the SGMBeamline database and public database
	AMDbUpgrade *am1Pt4SGMDb = new AMDbUpgrade1Pt4("SGMBeamline", this);
	AMDbUpgrade *am1Pt4SGMPublicDb = new AMDbUpgrade1Pt4("SGMPublic", this);
	am1Pt4SGMDb->setIsResponsibleForUpgrade(hasUpgradePermission);;
	am1Pt4SGMPublicDb->setIsResponsibleForUpgrade(hasUpgradePermission);;
	appendDatabaseUpgrade(am1Pt4SGMDb);
	appendDatabaseUpgrade(am1Pt4SGMPublicDb);

	// Add the SGM Beamline database as a source of exporter options
	additionalExporterOptionsDatabases_.append("SGMBeamline");

	checkedBadStartupSettings_ = false;
}

bool SGMAppController::startup() {
	qDebug() << "On startup the build reporter says:\n";
	qDebug() << "Branch:" << AMBuildReporter::buildReporter()->buildInfo()->branchName();
	qDebug() << "Commit:" << AMBuildReporter::buildReporter()->buildInfo()->commitSHA();
	qDebug() << "Author:" << AMBuildReporter::buildReporter()->buildInfo()->lastCommitAuthor();
	qDebug() << "Description:" << AMBuildReporter::buildReporter()->buildInfo()->describeName();
	qDebug() << "Date:" << AMBuildReporter::buildReporter()->buildInfo()->commitDate();

	if(AMBuildReporter::buildReporter()->buildInfo()->branchName() != "SGM_Release")
		QMessageBox::warning(0, "Deployment Warning", QString("Acquaman has detected that this application has been deployed from the wrong branch (%1).\nPlease contact David Chevrier immediately for assistance.").arg(AMBuildReporter::buildReporter()->buildInfo()->branchName()), QMessageBox::Ok, QMessageBox::NoButton);

	SGMSettings::s()->load();

	// Run all of the Acquaman App startup routines. Some of these are reimplemented in this class.
	if(!AMAppController::startup())
		return false;

	if (!AMActionRegistry3::s()->registerInfoAndAction<AMSampleMoveActionInfo, AMSampleMoveAction>("Move to Sample", "Move the beam over a given sample", ":system-run.png"))
		return false;
	if(!AMActionRegistry3::s()->registerInfoAndEditor<AMSampleMoveActionInfo, AMSampleMoveActionEditor>())
		return false;

	// Places the SGM specific actions in the menu bar
	if(!startupSGMInstallActions())
		return false;

	// Creates the SGM Beamline object
	SGMBeamline::sgm();
	connect(SGMBeamline::sgm(), SIGNAL(detectorAvailabilityChanged(AMDetector*,bool)), this, SLOT(onSGMBeamlineDetectorAvailabilityChanged(AMDetector*,bool)));
	AMErrorMon::information(this, AMDATAMANAPPCONTROLLER_STARTUP_MESSAGES, QString("SGM Startup: Waiting for detectors"));
	onSGMBeamlineDetectorAvailabilityChanged(0, false);

	// Initialize the storage ring.
	CLSStorageRing::sr1();

	// From Darren:  Adding this for SGM because they don't want to have scan editors to always be popping up automatically.
	setAutomaticBringScanEditorToFront(false);

	// Retrieve the current run or create one if there is none
	AMRun existingRun;
	if(!existingRun.loadFromDb(AMDatabase::database("user"), 1)) {
		// no run yet... let's create one.
		AMRun firstRun(CLSFacilityID::beamlineName(CLSFacilityID::SGMBeamline), CLSFacilityID::SGMBeamline); //3: SGM Beamline
		firstRun.storeToDb(AMDatabase::database("user"));
	}

	QList<int> allURLIds = AMDatabase::database("user")->objectsWhere(AMDbObjectSupport::s()->tableNameForClass<AMSampleCameraURL>());
	if(allURLIds.count() == 0){
		AMSampleCameraURL *newURL = new AMSampleCameraURL("http://10.52.48.103/axis-cgi/mjpg/video.cgi?resolution=1280x1024&.mjpg");
		newURL->storeToDb(AMDatabase::database("user"));

	}

	// Set up the GUI portions of the SGMAcquamanAppController
	if(!setupSGMViews())
		return false;

	// Center the completed GUI on the screen (which is normally 0,0 but has to be 0 1081 on OPI1611-408)
	if(QHostInfo::localHostName() == "OPI1611-408"){
		mw_->move(0, 1081);
	}
	else
		mw_->move(0, 0);

	return true;
}


void SGMAppController::shutdown() {
	// Make sure we release/clean-up the beamline interface
	AMBeamline::releaseBl();
	AMAppController::shutdown();
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

	bool success = true;

	// Register the detector and scan classes
	success &= AMDbObjectSupport::s()->registerClass<SGMMCPDetectorInfo>();
	success &= AMDbObjectSupport::s()->registerClass<CLSPGTDetectorInfo>();
	success &= AMDbObjectSupport::s()->registerClass<CLSAmptekSDD123DetectorInfo>();
	success &= AMDbObjectSupport::s()->registerClass<CLSOceanOptics65000DetectorInfo>();
	success &= AMDbObjectSupport::s()->registerClass<SGMXASScanConfiguration>();
	success &= AMDbObjectSupport::s()->registerClass<SGMFastScanConfiguration>();
	success &= AMDbObjectSupport::s()->registerClass<SGMXASScanConfiguration2013>();
	success &= AMDbObjectSupport::s()->registerClass<SGMFastScanConfiguration2013>();
	success &= AMDbObjectSupport::s()->registerClass<SGMSScanConfigurationDbObject>();

	// Register the configuration file and file loader plugin supports
	success &= AMDbObjectSupport::s()->registerClass<SGMPluginsLocation>();

	// Register the SGM periodic table support classes
	success &= AMDbObjectSupport::s()->registerClass<SGMEnergyPosition>();
	success &= AMDbObjectSupport::s()->registerClass<SGMScanInfo>();
	success &= AMDbObjectSupport::s()->registerClass<SGMElementInfo>();
	success &= AMDbObjectSupport::s()->registerClass<SGMFastScanSettings>();
	success &= AMDbObjectSupport::s()->registerClass<SGMFastScanParameters>();

	return success;
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

void SGMAppController::onSGMNewAmptekSDD5Connected(bool connected){
	Q_UNUSED(connected)
	if(SGMBeamline::sgm()->newAmptekSDD5() && SGMBeamline::sgm()->newAmptekSDD5()->isConnected() && !amptekSDD5XRFView_){
		amptekSDD5XRFView_ = new CLSAmptekDetailedDetectorView(qobject_cast<CLSAmptekSDD123DetectorNew*>(SGMBeamline::sgm()->newAmptekSDD5()));
		amptekSDD5XRFView_->buildDetectorView();
		amptekSDD5XRFView_->setEnergyRange(270, 2000);
		amptekSDD5XRFView_->collapsePeriodTableViews();
		mw_->addPane(amptekSDD5XRFView_, "Beamline Detectors", "SGM Amptek5 XRF", ":/system-software-update.png");
		connect(amptekSDD5XRFView_, SIGNAL(resized()), this, SLOT(onXRFDetectorViewResized()));
	}
}

void SGMAppController::onSGMNewPGTDetectorConnected(bool connected){
	Q_UNUSED(connected)
	/*
	if(SGMBeamline::sgm()->newPGTDetector() && SGMBeamline::sgm()->newPGTDetector()->isConnected() && !newPGTDetectorView_){
		newPGTDetectorView_ = new CLSPGTDetectorV2View(qobject_cast<CLSPGTDetectorV2*>(SGMBeamline::sgm()->newPGTDetector()));
		mw_->addPane(newPGTDetectorView_, "Beamline Detectors", "SGM PGT", ":/system-software-update.png");
	}
	*/
}

void SGMAppController::onSGMNewQE65000DetectorConnected(bool connected){
	Q_UNUSED(connected)
	if(SGMBeamline::sgm()->newQE65000Detector() && SGMBeamline::sgm()->newQE65000Detector()->isConnected() && !newQE65000DetectorView_){
		newQE65000DetectorView_ = new AMDetectorGeneralDetailedView(SGMBeamline::sgm()->newQE65000Detector());
		mw_->addPane(newQE65000DetectorView_, "Beamline Detectors", "SGM QE 65000", ":/system-software-update.png");
	}
}

void SGMAppController::onSGMNewTEYDetectorConnected(bool connected){
	Q_UNUSED(connected)
	if(SGMBeamline::sgm()->newTEYDetector() && SGMBeamline::sgm()->newTEYDetector()->isConnected() && !newTEYDetectorView_){
		newTEYDetectorView_ = new AMDetectorGeneralDetailedView(SGMBeamline::sgm()->newTEYDetector());
		mw_->addPane(newTEYDetectorView_, "Beamline Detectors", "SGM TEY", ":/system-software-update.png");
	}
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

void SGMAppController::onBeamAvailabilityChanged(bool beamAvailable)
{
	if (!beamAvailable && !AMActionRunner3::workflow()->pauseCurrentAction())
		AMActionRunner3::workflow()->setQueuePaused(true);

	// On SGM, we don't like having the scan restart on it's own.
	else if (beamAvailable && AMActionRunner3::workflow()->queuedActionCount() > 0)
		AMActionRunner3::workflow()->setQueuePaused(false);
}

void SGMAppController::onActionSGMSettings(){
	if(!sgmSettingsMasterView_)
		sgmSettingsMasterView_ = new SGMSettingsMasterView();
	sgmSettingsMasterView_->show();
}


void SGMAppController::onAdvancedCameraOptionsRequested(){
	if(sampleManagementView_)
		sampleManagementView_->requestAdvancedCameraOptionsWindow();
}

void SGMAppController::onActionMirrorVeiw(){
	if(!SGMAdvancedMirror_)
		SGMAdvancedMirror_ = new SGMAdvancedMirrorView();
	SGMAdvancedMirror_->show();
}

void SGMAppController::onSGMBeamlineDetectorAvailabilityChanged(AMDetector *detector, bool isAvailable){
	Q_UNUSED(detector)
	Q_UNUSED(isAvailable)
	QStringList waitingForDetectorNames;
	AMDetectorSet *currentlyUnconnected = SGMBeamline::sgm()->allDetectorGroup()->unconnectedDetectors();
	for(int x = 0, size = currentlyUnconnected->count(); x < size; x++)
		waitingForDetectorNames.append(currentlyUnconnected->at(x)->toInfo().description());

	QString waitingDetectors = QString("Waiting for:\n %1").arg(waitingForDetectorNames.join("\n"));
	if(lastWaitingDetectors_ != waitingDetectors){
		lastWaitingDetectors_ = waitingDetectors;
		AMErrorMon::information(this, AMDATAMANAPPCONTROLLER_STARTUP_MODECHANGE,  "Waiting");
		AMErrorMon::information(this, AMDATAMANAPPCONTROLLER_STARTUP_SUBTEXT,  lastWaitingDetectors_);
		qApp->processEvents();
	}
}

void SGMAppController::onXRFDetectorViewResized(){
	// This is a HACK, why it needs 100ms I have no idea. But any less and sometimes it doesn't resize properly
	QTimer::singleShot(100, this, SLOT(resizeToMinimum()));
}

void SGMAppController::resizeToMinimum(){
	mw_->resize(mw_->minimumSizeHint());
}

void SGMAppController::resizeAfterStartup(){
	QSize afterSize(1800, 1400);
	qDebug() << "Attempting to resize to " << afterSize;
	mw_->resize(afterSize);
	mw_->move(0, 1081);
}

void SGMAppController::onWorkflowActionAddedFromDialog(AMAction3 *action){
	AMControlMoveAction3 *controlMoveAction = qobject_cast<AMControlMoveAction3*>(action);
	if(controlMoveAction){
		AMControlMoveActionInfo3 *controlMoveActionInfo = qobject_cast<AMControlMoveActionInfo3*>(controlMoveAction->info());
		if(controlMoveActionInfo)
			controlMoveActionInfo->setIsRelativeMove(true);
		controlMoveAction->setFailureResponseAsSubAction(AMAction3::AttemptAnotherCopyResponse);
		controlMoveAction->setFailureResponseInActionRunner(AMAction3::AttemptAnotherCopyResponse);
	}
}

bool SGMAppController::startupSGMInstallActions(){
	QAction *sgmSettingAction = new QAction("SGM Beamline Settings...", mw_);
	sgmSettingAction->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_T));
	sgmSettingAction->setStatusTip("View or Change SGM Beamline Settings");
	connect(sgmSettingAction, SIGNAL(triggered()), this, SLOT(onActionSGMSettings()));

	sgmSettingsMasterView_ = 0;

	QAction *advancedSGMCameraOptionsAction = new QAction("SGM Camera Advanced...", mw_);
	advancedSGMCameraOptionsAction->setStatusTip("Advanced Camera Settings");
	connect(advancedSGMCameraOptionsAction, SIGNAL(triggered()), this, SLOT(onAdvancedCameraOptionsRequested()));

	QAction *SGMMirrorAction = new QAction("SGM Beamline Mirrors...", mw_);
	SGMMirrorAction->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_M));
	SGMMirrorAction->setStatusTip("View or Change SGM Beamline Mirrors");
	connect(SGMMirrorAction, SIGNAL(triggered()), this, SLOT(onActionMirrorVeiw()));

	SGMAdvancedMirror_ = 0;

	fileMenu_->addSeparator();
	fileMenu_->addAction(sgmSettingAction);

	viewMenu_->addAction(advancedSGMCameraOptionsAction);
	viewMenu_->addAction(SGMMirrorAction);

	return true;
}

bool SGMAppController::setupSGMPlugins()
{
	bool success = true;

	AMDatabase *dbSGM = AMDatabase::database("SGMBeamline");
	if(!dbSGM)
		return false;

	QList<int> matchIDs;

	SGMPluginsLocation *fileLoaderPluginsLocation, *analysisBlockPluginsLocation;
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMPluginsLocation>(), "name", "FileLoaders");
	fileLoaderPluginsLocation = new SGMPluginsLocation();
	if(matchIDs.count() == 0){
		fileLoaderPluginsLocation->setName("FileLoaders");
		fileLoaderPluginsLocation->setPluginFolderPath("/home/sgm/beamline/programming/acquaman/plugins/FileLoaders");
		success &= fileLoaderPluginsLocation->storeToDb(dbSGM);
	}
	else
		success &= fileLoaderPluginsLocation->loadFromDb(dbSGM, matchIDs.at(0));
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMPluginsLocation>(), "name", "AnalysisBlocks");
	analysisBlockPluginsLocation = new SGMPluginsLocation();
	if(matchIDs.count() == 0){
		analysisBlockPluginsLocation->setName("AnalysisBlocks");
		analysisBlockPluginsLocation->setPluginFolderPath("/home/sgm/beamline/programming/acquaman/plugins/AnalysisBlocks");
		success &= analysisBlockPluginsLocation->storeToDb(dbSGM);
	}
	else
		success &= analysisBlockPluginsLocation->loadFromDb(dbSGM, matchIDs.at(0));
	if(success)
		AMPluginsManager::s()->loadApplicationPlugins(fileLoaderPluginsLocation->pluginFolderPath(), analysisBlockPluginsLocation->pluginFolderPath());

	return success;
}

bool SGMAppController::setupSGMExporterOptions(){
	/*
	 Removed following line until the SGM Axis 2000 export is completed, in reference to Issue594
	AMExportController::registerExporter<SGMAxis2000Exporter>();
	*/

	bool success = true;

	AMDatabase *dbSGM = AMDatabase::database("SGMBeamline");
	if(!dbSGM)
		return false;

	success &= setupSGMXASExporterOptions(dbSGM);
	success &= setupSGMFastScanExporterOptions(dbSGM);

	return success;
}

bool SGMAppController::setupSGMXASExporterOptions(AMDatabase *dbSGM)
{
	bool success = true;

		QList<int> matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMExporterOptionGeneralAscii>(), "name", "SGMXASScanDefault");

	if(!dbSGM->isReadOnly()){
		AMExporterOptionGeneralAscii *sgmXASDefault;
		// Don't have one called "SGMXASScanDefault", so make one. If we have one, retreive it and check it.
		sgmXASDefault = new AMExporterOptionGeneralAscii();
		if(matchIDs.count() != 0)
			sgmXASDefault->loadFromDb(dbSGM, matchIDs.at(0));
		sgmXASDefault->setName("SGMXASScanDefault");
		sgmXASDefault->setFileName("$name_$fsIndex.txt");
		sgmXASDefault->setHeaderText("Scan: $name #$number\nDate: $dateTime\nSample: $sample\nFacility: $facilityDescription\nGrating: $scanConfiguration[grating%enumConvert]\nHarmonic: $scanConfiguration[harmonic%enumConvert]\nExit Slit Gap: $scanConfiguration[exitSlitGap%double%2] um\nNotes: $notes");
		sgmXASDefault->setHeaderIncluded(true);
		sgmXASDefault->setColumnHeader("$dataSetName $dataSetInfoDescription");
		sgmXASDefault->setColumnHeaderIncluded(true);
		sgmXASDefault->setColumnHeaderDelimiter("==========");
		sgmXASDefault->setSectionHeader("");
		sgmXASDefault->setSectionHeaderIncluded(true);
		if(sgmXASDefault->dataSources().count() > 0 && sgmXASDefault->dataSources().at(0) == "EnergyFeedback")
			sgmXASDefault->removeDataSourceAt(0);
		sgmXASDefault->ensureDataSource("I0", false, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmXASDefault->ensureDataSource("EnergyFeedback", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmXASDefault->ensureDataSource("Photodiode", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmXASDefault->ensureDataSource("TEY", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmXASDefault->ensureDataSource("TFY", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmXASDefault->ensureDataSource("TEYNorm", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmXASDefault->ensureDataSource("TFYNorm", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmXASDefault->ensureDataSource("OceanOptics65000", false, AMExporterOptionGeneral::SeparateFilesMode, false);
		sgmXASDefault->ensureDataSource("PLY", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmXASDefault->ensureDataSource("PLYNorm", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmXASDefault->ensureDataSource("Amptek1", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmXASDefault->ensureDataSource("Amptek2", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmXASDefault->ensureDataSource("Amptek3", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmXASDefault->ensureDataSource("Amptek4", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmXASDefault->ensureDataSource("Amptek5", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmXASDefault->setSeparateSectionFileName("$name_$dataSetName_$fsIndex.txt");
		sgmXASDefault->setIncludeAllDataSources(true);
		sgmXASDefault->setFirstColumnOnly(true);
		sgmXASDefault->setSeparateHigherDimensionalSources(true);
		sgmXASDefault->storeToDb(dbSGM);
	}
	else{
		//Should I do a check of some sort to make sure this is up to date?
	}

	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMExporterOptionGeneralAscii>(), "name", "SGMXASScanDefault");

	if(matchIDs.count() > 0){
		success &= AMAppControllerSupport::registerClass<SGMXASScanConfiguration, AMExporterGeneralAscii, AMExporterOptionGeneralAscii>(matchIDs.at(0), dbSGM->connectionName());
		success &= AMAppControllerSupport::registerClass<SGMXASScanConfiguration2013, AMExporterGeneralAscii, AMExporterOptionGeneralAscii>(matchIDs.at(0), dbSGM->connectionName());
	}
	else
		return false;

	return success;
}

bool SGMAppController::setupSGMFastScanExporterOptions(AMDatabase *dbSGM)
{
	bool success = true;

		QList<int> matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMExporterOptionGeneralAscii>(), "name", "SGMFastScanDefault");

	if(!dbSGM->isReadOnly()){
		AMExporterOptionGeneralAscii *sgmFastScanDefault;
		// Don't have one called "sgmFastScanDefault", so make one. If we have one, retreive it and check it.
		sgmFastScanDefault = new AMExporterOptionGeneralAscii();
		if(matchIDs.count() != 0)
			sgmFastScanDefault->loadFromDb(dbSGM, matchIDs.at(0));
		sgmFastScanDefault->setName("SGMFastScanDefault");
		sgmFastScanDefault->setFileName("$name_$fsIndex.txt");
		sgmFastScanDefault->setHeaderText("Scan: $name #$number\nDate: $dateTime\nSample: $sample\nFacility: $facilityDescription\nNotes: $notes");
		sgmFastScanDefault->setHeaderIncluded(true);
		sgmFastScanDefault->setColumnHeader("$dataSetName $dataSetInfoDescription");
		sgmFastScanDefault->setColumnHeaderIncluded(true);
		sgmFastScanDefault->setColumnHeaderDelimiter("==========");
		sgmFastScanDefault->setSectionHeader("");
		sgmFastScanDefault->setSectionHeaderIncluded(true);
		if(sgmFastScanDefault->dataSources().count() > 0 && sgmFastScanDefault->dataSources().at(0) == "EnergyFeedback")
			sgmFastScanDefault->removeDataSourceAt(0);
		sgmFastScanDefault->ensureDataSource("I0", false, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmFastScanDefault->ensureDataSource("EnergyFeedback", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmFastScanDefault->ensureDataSource("Photodiode", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmFastScanDefault->ensureDataSource("TEY", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmFastScanDefault->ensureDataSource("TFY", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmFastScanDefault->ensureDataSource("TEYNorm", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmFastScanDefault->ensureDataSource("TFYNorm", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmFastScanDefault->ensureDataSource("OceanOptics65000", false, AMExporterOptionGeneral::SeparateFilesMode, false);
		sgmFastScanDefault->ensureDataSource("PLY", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmFastScanDefault->ensureDataSource("PLYNorm", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmFastScanDefault->ensureDataSource("FPD1", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmFastScanDefault->ensureDataSource("FPD2", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmFastScanDefault->ensureDataSource("FPD3", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmFastScanDefault->ensureDataSource("FPD4", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmFastScanDefault->ensureDataSource("FPD5", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sgmFastScanDefault->setSeparateSectionFileName("$name_$dataSetName_$fsIndex.txt");
		sgmFastScanDefault->setIncludeAllDataSources(true);
		sgmFastScanDefault->setFirstColumnOnly(true);
		sgmFastScanDefault->setSeparateHigherDimensionalSources(true);
		sgmFastScanDefault->storeToDb(dbSGM);
	}
	else{
		//Should I do a check of some sort to make sure this is up to date?
	}

	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMExporterOptionGeneralAscii>(), "name", "SGMFastScanDefault");

	if(matchIDs.count() > 0){
		success &= AMAppControllerSupport::registerClass<SGMFastScanConfiguration, AMExporterGeneralAscii, AMExporterOptionGeneralAscii>(matchIDs.at(0), dbSGM->connectionName());
		success &= AMAppControllerSupport::registerClass<SGMFastScanConfiguration2013, AMExporterGeneralAscii, AMExporterOptionGeneralAscii>(matchIDs.at(0), dbSGM->connectionName());
	}
	else
		return false;

	return success;
}

bool SGMAppController::setupSGMPeriodicTable(){
	bool success = true;

	AMDatabase *dbSGM = AMDatabase::database("SGMBeamline");
	if(!dbSGM)
		return false;

	QList<int> matchIDs;

	QString elementSymbol;
	QString elementName;
	QString elementEdge;

	// CARBON SETTINGS
	elementSymbol = "C";
	elementName = "Carbon";
	elementEdge = "K";
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMElementInfo>(), "name", elementName+"ElementInfo");
	if(matchIDs.count() == 0){

		SGMEnergyPosition epStart(elementName%elementEdge%"Start", 270.0, -397720, -149991, 286.63, 0);
		SGMEnergyPosition epMiddle(elementName%elementEdge%"Middle", 295.0, -377497, -140470, 200.46, 0);
		SGMEnergyPosition epEnd(elementName%elementEdge%"End", 320.0, -348005, -133061, 100.54, 0);
		success &= epStart.storeToDb(dbSGM);
		success &= epMiddle.storeToDb(dbSGM);
		success &= epEnd.storeToDb(dbSGM);

		/* Settings for faking really quick carbon scans
		SGMEnergyPosition epStart(elementName%elementEdge%"Start", 300.0, -371207, -141240, 1.0, 0);
		SGMEnergyPosition epMiddle(elementName%elementEdge%"Middle", 301.0, -369973, -140888, 1.0, 0);
		SGMEnergyPosition epEnd(elementName%elementEdge%"End", 302.0, -368750, -140571, 1.0, 0);
		success &= epStart.storeToDb(dbSGM);
		success &= epMiddle.storeToDb(dbSGM);
		success &= epEnd.storeToDb(dbSGM);
		*/

		AMAbsorptionEdge edge = AMPeriodicTable::table()->elementBySymbol(elementSymbol)->KEdge();
		SGMScanInfo scanInfo(elementName%" "%elementEdge, edge, epStart, epMiddle, epEnd);
		success &= scanInfo.storeToDb(dbSGM);

		/*
		SGMFastScanSettings fs5Settings(elementName%elementEdge%"5sSettings", 5.0, 40000, 5.0, 200, 6000);
		success &= fs5Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp5 = new SGMFastScanParameters(elementName%elementEdge%"5s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs5Settings);
		success &= fsp5->storeToDb(dbSGM);
		*/

		/* Quick scans for carbon testing
		SGMFastScanSettings fs1Settings(elementName%elementEdge%"1sSettings", 1.0, 50000, 1.0, 200, 8000);
		success &= fs1Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp1 = new SGMFastScanParameters(elementName%elementEdge%"1s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs1Settings);
		success &= fsp1->storeToDb(dbSGM);

		SGMFastScanSettings fs2Settings(elementName%elementEdge%"2sSettings", 2.0, 40000, 2.0, 200, 6000);
		success &= fs2Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp2 = new SGMFastScanParameters(elementName%elementEdge%"2s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs2Settings);
		success &= fsp2->storeToDb(dbSGM);
		*/

		SGMFastScanSettings fs120Settings(elementName%elementEdge%"120sSettings", 120.0, 872, 120.0, 200, 141);
		success &= fs120Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp120 = new SGMFastScanParameters(elementName%elementEdge%"120s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs120Settings);
		success &= fsp120->storeToDb(dbSGM);

		SGMFastScanSettings fs20Settings(elementName%elementEdge%"20sSettings", 20.0, 5800, 20.0, 200, 970);
		success &= fs20Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp20 = new SGMFastScanParameters(elementName%elementEdge%"20s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs20Settings);
		success &= fsp20->storeToDb(dbSGM);

		/*
		SGMFastScanSettings fs120Settings(elementName%elementEdge%"120sSettings", 120.0, 872, 120.0, 200, 141);
		success &= fs120Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp120 = new SGMFastScanParameters(elementName%elementEdge%"120s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs120Settings);
		success &= fsp120->storeToDb(dbSGM);
		*/

		SGMElementInfo *elementInfo = new SGMElementInfo(elementName%"ElementInfo", AMPeriodicTable::table()->elementBySymbol(elementSymbol), this);
		elementInfo->addEdgeInfo(scanInfo);
		/*
		elementInfo->addFastScanParameters(fsp1);
		elementInfo->addFastScanParameters(fsp2);
		*/
		elementInfo->addFastScanParameters(fsp20);
		elementInfo->addFastScanParameters(fsp120);
		success &= elementInfo->storeToDb(dbSGM);
	}

	// NITROGEN SETTINGS
	elementSymbol = "N";
	elementName = "Nitrogen";
	elementEdge = "K";
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMElementInfo>(), "name", elementName+"ElementInfo");
	if(matchIDs.count() == 0){
		SGMEnergyPosition epStart(elementName%elementEdge%"Start", 380.0, -293057, -115327, 1.37942, 0);
		SGMEnergyPosition epMiddle(elementName%elementEdge%"Middle", 410.0, -268341, -105051, 3.88, 0);
		SGMEnergyPosition epEnd(elementName%elementEdge%"End", 440.0, -253095, -99461, 15.5151, 0);
		success &= epStart.storeToDb(dbSGM);
		success &= epMiddle.storeToDb(dbSGM);
		success &= epEnd.storeToDb(dbSGM);

		AMAbsorptionEdge edge = AMPeriodicTable::table()->elementBySymbol(elementSymbol)->KEdge();
		SGMScanInfo scanInfo(elementName%" "%elementEdge, edge, epStart, epMiddle, epEnd);
		success &= scanInfo.storeToDb(dbSGM);

		/*
		SGMFastScanSettings fs5Settings(elementName%elementEdge%"5sSettings", 5.0, 10000, 5.0, 200, 1500);
		success &= fs5Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp5 = new SGMFastScanParameters(elementName%elementEdge%"5s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs5Settings);
		success &= fsp5->storeToDb(dbSGM);
		*/

		SGMFastScanSettings fs120Settings(elementName%elementEdge%"120sSettings", 120.0, 540, 120.0, 800, 132);
		success &= fs120Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp120 = new SGMFastScanParameters(elementName%elementEdge%"120s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs120Settings);
		success &= fsp120->storeToDb(dbSGM);

		SGMFastScanSettings fs20Settings(elementName%elementEdge%"20sSettings", 20.0, 3800, 20.0, 800, 825);
		success &= fs20Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp20 = new SGMFastScanParameters(elementName%elementEdge%"20s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs20Settings);
		success &= fsp20->storeToDb(dbSGM);

		/*
		SGMFastScanSettings fs120Settings(elementName%elementEdge%"120sSettings", 120.0, 540, 120.0, 800, 132);
		success &= fs120Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp120 = new SGMFastScanParameters(elementName%elementEdge%"120s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs120Settings);
		success &= fsp120->storeToDb(dbSGM);
		*/

		SGMElementInfo *elementInfo = new SGMElementInfo(elementName%"ElementInfo", AMPeriodicTable::table()->elementBySymbol(elementSymbol), this);
		elementInfo->addEdgeInfo(scanInfo);
		elementInfo->addFastScanParameters(fsp20);
		elementInfo->addFastScanParameters(fsp120);
		success &= elementInfo->storeToDb(dbSGM);
	}

	// OXYGEN SETTINGS
	elementSymbol = "O";
	elementName = "Oxygen";
	elementEdge = "K";
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMElementInfo>(), "name", elementName+"ElementInfo");
	if(matchIDs.count() == 0){
		SGMEnergyPosition epStart(elementName%elementEdge%"Start", 530.0, -210116, -77335, 166.586, 0);
		SGMEnergyPosition epMiddle(elementName%elementEdge%"Middle", 545.0, -204333, -73988, 198.591, 0);
		SGMEnergyPosition epEnd(elementName%elementEdge%"End", 560.0, -198860, -70692, 232.356, 0);
		success &= epStart.storeToDb(dbSGM);
		success &= epMiddle.storeToDb(dbSGM);
		success &= epEnd.storeToDb(dbSGM);

		AMAbsorptionEdge edge = AMPeriodicTable::table()->elementBySymbol(elementSymbol)->KEdge();
		SGMScanInfo scanInfo(elementName%" "%elementEdge, edge, epStart, epMiddle, epEnd);
		success &= scanInfo.storeToDb(dbSGM);

		SGMFastScanSettings fs5Settings(elementName%elementEdge%"5sSettings", 5.0, 10000, 5.0, 200, 1450);
		success &= fs5Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp5 = new SGMFastScanParameters(elementName%elementEdge%"5s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs5Settings);
		success &= fsp5->storeToDb(dbSGM);

		SGMFastScanSettings fs20Settings(elementName%elementEdge%"20sSettings", 20.0, 1050, 20.0, 800, 330);
		success &= fs20Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp20 = new SGMFastScanParameters(elementName%elementEdge%"20s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs20Settings);
		success &= fsp20->storeToDb(dbSGM);

		SGMElementInfo *elementInfo = new SGMElementInfo(elementName%"ElementInfo", AMPeriodicTable::table()->elementBySymbol(elementSymbol), this);
		elementInfo->addEdgeInfo(scanInfo);
		elementInfo->addFastScanParameters(fsp5);
		elementInfo->addFastScanParameters(fsp20);
		success &= elementInfo->storeToDb(dbSGM);
	}

	// CALCIUM SETTINGS
	elementSymbol = "Ca";
	elementName = "Calcium";
	elementEdge = "L";
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMElementInfo>(), "name", elementName+"ElementInfo");
	if(matchIDs.count() == 0){
		SGMEnergyPosition epStart(elementName%elementEdge%"Start", 345.0, -322785, -125202, 1.0, 1);
		SGMEnergyPosition epMiddle(elementName%elementEdge%"Middle", 355.0, -313695, -122224, 1.0, 1);
		SGMEnergyPosition epEnd(elementName%elementEdge%"End", 365.0, -305100, -119343, 1.0, 1);
		success &= epStart.storeToDb(dbSGM);
		success &= epMiddle.storeToDb(dbSGM);
		success &= epEnd.storeToDb(dbSGM);

		AMAbsorptionEdge edge = AMPeriodicTable::table()->elementBySymbol(elementSymbol)->L3Edge();
		SGMScanInfo scanInfo(elementName%" "%elementEdge, edge, epStart, epMiddle, epEnd);
		success &= scanInfo.storeToDb(dbSGM);

		SGMFastScanSettings fs5Settings(elementName%elementEdge%"5sSettings", 5.0, 8500, 5.0, 200, 1800);
		success &= fs5Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp5 = new SGMFastScanParameters(elementName%elementEdge%"5s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs5Settings);
		success &= fsp5->storeToDb(dbSGM);

		SGMFastScanSettings fs20Settings(elementName%elementEdge%"20sSettings", 20.0, 1800, 20.0, 800, 300);
		success &= fs20Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp20 = new SGMFastScanParameters(elementName%elementEdge%"20s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs20Settings);
		success &= fsp20->storeToDb(dbSGM);

		SGMElementInfo *elementInfo = new SGMElementInfo(elementName%"ElementInfo", AMPeriodicTable::table()->elementBySymbol(elementSymbol), this);
		elementInfo->addEdgeInfo(scanInfo);
		elementInfo->addFastScanParameters(fsp5);
		elementInfo->addFastScanParameters(fsp20);
		success &= elementInfo->storeToDb(dbSGM);
	}

	// TITANIUM SETTINGS
	elementSymbol = "Ti";
	elementName = "Titanium";
	elementEdge = "L";
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMElementInfo>(), "name", elementName+"ElementInfo");
	if(matchIDs.count() == 0){
		SGMEnergyPosition epStart(elementName%elementEdge%"Start", 450.0, -247469, -96777, 1.0, 0);
		SGMEnergyPosition epMiddle(elementName%elementEdge%"Middle", 465.0, -239490, -93072, 1.0, 0);
		SGMEnergyPosition epEnd(elementName%elementEdge%"End", 480.0, -232004, -89479, 21.226, 0);
		success &= epStart.storeToDb(dbSGM);
		success &= epMiddle.storeToDb(dbSGM);
		success &= epEnd.storeToDb(dbSGM);

		AMAbsorptionEdge edge = AMPeriodicTable::table()->elementBySymbol(elementSymbol)->L3Edge();
		SGMScanInfo scanInfo(elementName%" "%elementEdge, edge, epStart, epMiddle, epEnd);
		success &= scanInfo.storeToDb(dbSGM);

		SGMFastScanSettings fs5Settings(elementName%elementEdge%"5sSettings", 5.0, 8500, 5.0, 200, 1800);
		success &= fs5Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp5 = new SGMFastScanParameters(elementName%elementEdge%"5s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs5Settings);
		success &= fsp5->storeToDb(dbSGM);

		SGMFastScanSettings fs20Settings(elementName%elementEdge%"20sSettings", 20.0, 1800, 20.0, 800, 300);
		success &= fs20Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp20 = new SGMFastScanParameters(elementName%elementEdge%"20s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs20Settings);
		success &= fsp20->storeToDb(dbSGM);

		SGMElementInfo *elementInfo = new SGMElementInfo(elementName%"ElementInfo", AMPeriodicTable::table()->elementBySymbol(elementSymbol), this);
		elementInfo->addEdgeInfo(scanInfo);
		elementInfo->addFastScanParameters(fsp5);
		elementInfo->addFastScanParameters(fsp20);
		success &= elementInfo->storeToDb(dbSGM);
	}

	// CHROMIUM SETTINGS
	elementSymbol = "Cr";
	elementName = "Chromium";
	elementEdge = "L";
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMElementInfo>(), "name", elementName+"ElementInfo");
	if(matchIDs.count() == 0){
		SGMEnergyPosition epStart(elementName%elementEdge%"Start", 565.0, -197101, -70227, 189.973, 0);
		SGMEnergyPosition epMiddle(elementName%elementEdge%"Middle", 585.0, -190363, -65922, 238.105, 0);
		SGMEnergyPosition epEnd(elementName%elementEdge%"End", 605.0, -184068, -61689, 288.669, 0);
		success &= epStart.storeToDb(dbSGM);
		success &= epMiddle.storeToDb(dbSGM);
		success &= epEnd.storeToDb(dbSGM);

		AMAbsorptionEdge edge = AMPeriodicTable::table()->elementBySymbol(elementSymbol)->L3Edge();
		SGMScanInfo scanInfo(elementName%" "%elementEdge, edge, epStart, epMiddle, epEnd);
		success &= scanInfo.storeToDb(dbSGM);

		SGMFastScanSettings fs5Settings(elementName%elementEdge%"5sSettings", 5.0, 8500, 5.0, 200, 1800);
		success &= fs5Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp5 = new SGMFastScanParameters(elementName%elementEdge%"5s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs5Settings);
		success &= fsp5->storeToDb(dbSGM);

		SGMFastScanSettings fs20Settings(elementName%elementEdge%"20sSettings", 20.0, 1800, 20.0, 800, 300);
		success &= fs20Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp20 = new SGMFastScanParameters(elementName%elementEdge%"20s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs20Settings);
		success &= fsp20->storeToDb(dbSGM);

		SGMElementInfo *elementInfo = new SGMElementInfo(elementName%"ElementInfo", AMPeriodicTable::table()->elementBySymbol(elementSymbol), this);
		elementInfo->addEdgeInfo(scanInfo);
		elementInfo->addFastScanParameters(fsp5);
		elementInfo->addFastScanParameters(fsp20);
		success &= elementInfo->storeToDb(dbSGM);
	}

	// IRON SETTINGS
	elementSymbol = "Fe";
	elementName = "Iron";
	elementEdge = "L";
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMElementInfo>(), "name", elementName+"ElementInfo");
	if(matchIDs.count() == 0){
		SGMEnergyPosition epStart(elementName%elementEdge%"Start", 690.0, -295012, -46140, 70.26, 1);
		SGMEnergyPosition epMiddle(elementName%elementEdge%"Middle", 720.0, -282721, -40231, 70.39, 1);
		SGMEnergyPosition epEnd(elementName%elementEdge%"End", 750.0, -271411, -34383, 77.68, 1);
		success &= epStart.storeToDb(dbSGM);
		success &= epMiddle.storeToDb(dbSGM);
		success &= epEnd.storeToDb(dbSGM);

		AMAbsorptionEdge edge = AMPeriodicTable::table()->elementBySymbol(elementSymbol)->L3Edge();
		SGMScanInfo scanInfo(elementName%" "%elementEdge, edge, epStart, epMiddle, epEnd);
		success &= scanInfo.storeToDb(dbSGM);

		SGMFastScanSettings fs5Settings(elementName%elementEdge%"5sSettings", 5.0, 8500, 5.0, 200, 1800);
		success &= fs5Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp5 = new SGMFastScanParameters(elementName%elementEdge%"5s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs5Settings);
		success &= fsp5->storeToDb(dbSGM);

		SGMFastScanSettings fs20Settings(elementName%elementEdge%"20sSettings", 20.0, 2400, 20.0, 600, 300);
		success &= fs20Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp20 = new SGMFastScanParameters(elementName%elementEdge%"20s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs20Settings);
		success &= fsp20->storeToDb(dbSGM);

		SGMElementInfo *elementInfo = new SGMElementInfo(elementName%"ElementInfo", AMPeriodicTable::table()->elementBySymbol(elementSymbol), this);
		elementInfo->addEdgeInfo(scanInfo);
		elementInfo->addFastScanParameters(fsp5);
		elementInfo->addFastScanParameters(fsp20);
		success &= elementInfo->storeToDb(dbSGM);
	}

	// COBALT SETTINGS
	elementSymbol = "Co";
	elementName = "Cobalt";
	elementEdge = "L";
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMElementInfo>(), "name", elementName+"ElementInfo");
	if(matchIDs.count() == 0){
		SGMEnergyPosition epStart(elementName%elementEdge%"Start", 750.0, -271410, -34592, 77.70, 1);
		SGMEnergyPosition epMiddle(elementName%elementEdge%"Middle", 780.0, -260974, -28793, 91.09, 1);
		SGMEnergyPosition epEnd(elementName%elementEdge%"End", 820.0, -248243, -21152, 116.99, 1);
		success &= epStart.storeToDb(dbSGM);
		success &= epMiddle.storeToDb(dbSGM);
		success &= epEnd.storeToDb(dbSGM);

		AMAbsorptionEdge edge = AMPeriodicTable::table()->elementBySymbol(elementSymbol)->L3Edge();
		SGMScanInfo scanInfo(elementName%" "%elementEdge, edge, epStart, epMiddle, epEnd);
		success &= scanInfo.storeToDb(dbSGM);

		/*
		SGMFastScanSettings fs5Settings(elementName%elementEdge%"5sSettings", 5.0, 8500, 5.0, 200, 1800);
		success &= fs5Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp5 = new SGMFastScanParameters(elementName%elementEdge%"5s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs5Settings);
		success &= fsp5->storeToDb(dbSGM);
		*/

		SGMFastScanSettings fs20Settings(elementName%elementEdge%"20sSettings", 20.0, 2000, 20.0, 800, 700);
		success &= fs20Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp20 = new SGMFastScanParameters(elementName%elementEdge%"20s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs20Settings);
		success &= fsp20->storeToDb(dbSGM);

		SGMElementInfo *elementInfo = new SGMElementInfo(elementName%"ElementInfo", AMPeriodicTable::table()->elementBySymbol(elementSymbol), this);
		elementInfo->addEdgeInfo(scanInfo);
		elementInfo->addFastScanParameters(fsp20);
		success &= elementInfo->storeToDb(dbSGM);
	}

	// NICKEL SETTINGS
	elementSymbol = "Ni";
	elementName = "Nickel";
	elementEdge = "L";
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMElementInfo>(), "name", elementName+"ElementInfo");
	if(matchIDs.count() == 0){
		SGMEnergyPosition epStart(elementName%elementEdge%"Start", 840.0, -242330, -15364, 78.917, 1);
		SGMEnergyPosition epMiddle(elementName%elementEdge%"Middle", 865.0, -235326, -10614, 101.285, 1);
		SGMEnergyPosition epEnd(elementName%elementEdge%"End", 890.0, -228716, -5858, 126.123, 1);
		success &= epStart.storeToDb(dbSGM);
		success &= epMiddle.storeToDb(dbSGM);
		success &= epEnd.storeToDb(dbSGM);

		AMAbsorptionEdge edge = AMPeriodicTable::table()->elementBySymbol(elementSymbol)->L3Edge();
		SGMScanInfo scanInfo(elementName%" "%elementEdge, edge, epStart, epMiddle, epEnd);
		success &= scanInfo.storeToDb(dbSGM);

		SGMFastScanSettings fs5Settings(elementName%elementEdge%"5sSettings", 5.0, 8500, 5.0, 200, 1800);
		success &= fs5Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp5 = new SGMFastScanParameters(elementName%elementEdge%"5s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs5Settings);
		success &= fsp5->storeToDb(dbSGM);

		SGMFastScanSettings fs20Settings(elementName%elementEdge%"20sSettings", 20.0, 1800, 20.0, 800, 300);
		success &= fs20Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp20 = new SGMFastScanParameters(elementName%elementEdge%"20s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs20Settings);
		success &= fsp20->storeToDb(dbSGM);

		SGMElementInfo *elementInfo = new SGMElementInfo(elementName%"ElementInfo", AMPeriodicTable::table()->elementBySymbol(elementSymbol), this);
		elementInfo->addEdgeInfo(scanInfo);
		elementInfo->addFastScanParameters(fsp5);
		elementInfo->addFastScanParameters(fsp20);
		success &= elementInfo->storeToDb(dbSGM);
	}

	// COPPER SETTINGS
	elementSymbol = "Cu";
	elementName = "Copper";
	elementEdge = "L";
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMElementInfo>(), "name", elementName+"ElementInfo");
	if(matchIDs.count() == 0){
		SGMEnergyPosition epStart(elementName%elementEdge%"Start", 920.0, -221259, 1300, 218.584, 1);
		SGMEnergyPosition epMiddle(elementName%elementEdge%"Middle", 950.0, -214272, 3318, 230.292, 1);
		SGMEnergyPosition epEnd(elementName%elementEdge%"End", 970.0, -209854, 9900, 242.299, 1);
		success &= epStart.storeToDb(dbSGM);
		success &= epMiddle.storeToDb(dbSGM);
		success &= epEnd.storeToDb(dbSGM);

		AMAbsorptionEdge edge = AMPeriodicTable::table()->elementBySymbol(elementSymbol)->L3Edge();
		SGMScanInfo scanInfo(elementName%" "%elementEdge, edge, epStart, epMiddle, epEnd);
		success &= scanInfo.storeToDb(dbSGM);

		SGMFastScanSettings fs5Settings(elementName%elementEdge%"5sSettings", 5.0, 3000, 5.0, 200, 1000);
		success &= fs5Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp5 = new SGMFastScanParameters(elementName%elementEdge%"5s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs5Settings);
		success &= fsp5->storeToDb(dbSGM);

		SGMFastScanSettings fs20Settings(elementName%elementEdge%"20sSettings", 20.0, 1100, 20.0, 800, 510);
		success &= fs20Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp20 = new SGMFastScanParameters(elementName%elementEdge%"20s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs20Settings);
		success &= fsp20->storeToDb(dbSGM);

		SGMElementInfo *elementInfo = new SGMElementInfo(elementName%"ElementInfo", AMPeriodicTable::table()->elementBySymbol(elementSymbol), this);
		elementInfo->addEdgeInfo(scanInfo);
		elementInfo->addFastScanParameters(fsp5);
		elementInfo->addFastScanParameters(fsp20);
		success &= elementInfo->storeToDb(dbSGM);
	}

	// ZINC SETTINGS
	elementSymbol = "Zn";
	elementName = "Zinc";
	elementEdge = "L";
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMElementInfo>(), "name", elementName+"ElementInfo");
	if(matchIDs.count() == 0){
		SGMEnergyPosition epStart(elementName%elementEdge%"Start", 1010.0, -201543, 17796, 326.886, 1);
		SGMEnergyPosition epMiddle(elementName%elementEdge%"Middle", 1035.0, -196675, 22723, 362.103, 1);
		SGMEnergyPosition epEnd(elementName%elementEdge%"End", 1060.0, -192036, 27678, 398.628, 1);
		success &= epStart.storeToDb(dbSGM);
		success &= epMiddle.storeToDb(dbSGM);
		success &= epEnd.storeToDb(dbSGM);

		AMAbsorptionEdge edge = AMPeriodicTable::table()->elementBySymbol(elementSymbol)->L3Edge();
		SGMScanInfo scanInfo(elementName%" "%elementEdge, edge, epStart, epMiddle, epEnd);
		success &= scanInfo.storeToDb(dbSGM);

		SGMFastScanSettings fs5Settings(elementName%elementEdge%"5sSettings", 5.0, 6000, 5.0, 200, 3500);
		success &= fs5Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp5 = new SGMFastScanParameters(elementName%elementEdge%"5s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs5Settings);
		success &= fsp5->storeToDb(dbSGM);

		SGMFastScanSettings fs20Settings(elementName%elementEdge%"20sSettings", 20.0, 850, 20.0, 800, 600);
		success &= fs20Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp20 = new SGMFastScanParameters(elementName%elementEdge%"20s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs20Settings);
		success &= fsp20->storeToDb(dbSGM);

		SGMElementInfo *elementInfo = new SGMElementInfo(elementName%"ElementInfo", AMPeriodicTable::table()->elementBySymbol(elementSymbol), this);
		elementInfo->addEdgeInfo(scanInfo);
		elementInfo->addFastScanParameters(fsp5);
		elementInfo->addFastScanParameters(fsp20);
		success &= elementInfo->storeToDb(dbSGM);
	}

	// SODIUM SETTINGS
	elementSymbol = "Na";
	elementName = "Sodium";
	elementEdge = "K";
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMElementInfo>(), "name", elementName+"ElementInfo");
	if(matchIDs.count() == 0){
		SGMEnergyPosition epStart(elementName%elementEdge%"Start", 1050.0, -193868, 25085, 329.87, 1);
		SGMEnergyPosition epMiddle(elementName%elementEdge%"Middle", 1080.0, -188479, 31109, 374.72, 1);
		SGMEnergyPosition epEnd(elementName%elementEdge%"End", 1110.0, -183388, 37235, 421.201, 1);
		success &= epStart.storeToDb(dbSGM);
		success &= epMiddle.storeToDb(dbSGM);
		success &= epEnd.storeToDb(dbSGM);

		AMAbsorptionEdge edge = AMPeriodicTable::table()->elementBySymbol(elementSymbol)->KEdge();
		SGMScanInfo scanInfo(elementName%" "%elementEdge, edge, epStart, epMiddle, epEnd);
		success &= scanInfo.storeToDb(dbSGM);

		SGMFastScanSettings fs5Settings(elementName%elementEdge%"5sSettings", 5.0, 24000, 5.0, 200, 4000);
		success &= fs5Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp5 = new SGMFastScanParameters(elementName%elementEdge%"5s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs5Settings);
		success &= fsp5->storeToDb(dbSGM);

		SGMFastScanSettings fs20Settings(elementName%elementEdge%"20sSettings", 20.0, 4800, 20.0, 200, 820);
		success &= fs20Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp20 = new SGMFastScanParameters(elementName%elementEdge%"20s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs20Settings);
		success &= fsp20->storeToDb(dbSGM);

		SGMElementInfo *elementInfo = new SGMElementInfo(elementName%"ElementInfo", AMPeriodicTable::table()->elementBySymbol(elementSymbol), this);
		elementInfo->addEdgeInfo(scanInfo);
		elementInfo->addFastScanParameters(fsp5);
		elementInfo->addFastScanParameters(fsp20);
		success &= elementInfo->storeToDb(dbSGM);
	}

	// MAGNESIUM SETTINGS
	elementSymbol = "Mg";
	elementName = "Magnesium";
	elementEdge = "K";
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMElementInfo>(), "name", elementName+"ElementInfo");
	if(matchIDs.count() == 0){
		SGMEnergyPosition epStart(elementName%elementEdge%"Start", 1290.0, -249736, -101803, 91.21, 2);
		SGMEnergyPosition epMiddle(elementName%elementEdge%"Middle", 1315.0, -239101, -99682, 105.38, 2);
		SGMEnergyPosition epEnd(elementName%elementEdge%"End", 1340.0, -234638, -97582, 120.65, 2);
		success &= epStart.storeToDb(dbSGM);
		success &= epMiddle.storeToDb(dbSGM);
		success &= epEnd.storeToDb(dbSGM);

		AMAbsorptionEdge edge = AMPeriodicTable::table()->elementBySymbol(elementSymbol)->KEdge();
		SGMScanInfo scanInfo(elementName%" "%elementEdge, edge, epStart, epMiddle, epEnd);
		success &= scanInfo.storeToDb(dbSGM);

		SGMFastScanSettings fs5Settings(elementName%elementEdge%"5sSettings", 5.0, 24000, 5.0, 200, 4000);
		success &= fs5Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp5 = new SGMFastScanParameters(elementName%elementEdge%"5s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs5Settings);
		success &= fsp5->storeToDb(dbSGM);

		SGMFastScanSettings fs20Settings(elementName%elementEdge%"20sSettings", 20.0, 4800, 20.0, 200, 820);
		success &= fs20Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp20 = new SGMFastScanParameters(elementName%elementEdge%"20s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs20Settings);
		success &= fsp20->storeToDb(dbSGM);

		SGMElementInfo *elementInfo = new SGMElementInfo(elementName%"ElementInfo", AMPeriodicTable::table()->elementBySymbol(elementSymbol), this);
		elementInfo->addEdgeInfo(scanInfo);
		elementInfo->addFastScanParameters(fsp5);
		elementInfo->addFastScanParameters(fsp20);
		success &= elementInfo->storeToDb(dbSGM);
	}

	// ALUMINUM SETTINGS
	elementSymbol = "Al";
	elementName = "Aluminum";
	elementEdge = "K";
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMElementInfo>(), "name", elementName+"ElementInfo");
	if(matchIDs.count() == 0){
		SGMEnergyPosition epStart(elementName%elementEdge%"Start", 1550.0, -202852, -80942, 283.82, 2);
		SGMEnergyPosition epMiddle(elementName%elementEdge%"Middle", 1580.0, -198998, -78694, 311.23, 2);
		SGMEnergyPosition epEnd(elementName%elementEdge%"End", 1610.0, -195288, -76434, 339.47, 2);
		success &= epStart.storeToDb(dbSGM);
		success &= epMiddle.storeToDb(dbSGM);
		success &= epEnd.storeToDb(dbSGM);

		AMAbsorptionEdge edge = AMPeriodicTable::table()->elementBySymbol(elementSymbol)->KEdge();
		SGMScanInfo scanInfo(elementName%" "%elementEdge, edge, epStart, epMiddle, epEnd);
		success &= scanInfo.storeToDb(dbSGM);

		SGMFastScanSettings fs5Settings(elementName%elementEdge%"5sSettings", 5.0, 24000, 5.0, 200, 4000);
		success &= fs5Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp5 = new SGMFastScanParameters(elementName%elementEdge%"5s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs5Settings);
		success &= fsp5->storeToDb(dbSGM);

		SGMFastScanSettings fs20Settings(elementName%elementEdge%"20sSettings", 20.0, 800, 20.0, 200, 250);
		success &= fs20Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp20 = new SGMFastScanParameters(elementName%elementEdge%"20s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs20Settings);
		success &= fsp20->storeToDb(dbSGM);

		SGMElementInfo *elementInfo = new SGMElementInfo(elementName%"ElementInfo", AMPeriodicTable::table()->elementBySymbol(elementSymbol), this);
		elementInfo->addEdgeInfo(scanInfo);
		elementInfo->addFastScanParameters(fsp5);
		elementInfo->addFastScanParameters(fsp20);
		success &= elementInfo->storeToDb(dbSGM);
	}

	// SILICON SETTINGS
	elementSymbol = "Si";
	elementName = "Silicon";
	elementEdge = "K";
	matchIDs = dbSGM->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMElementInfo>(), "name", elementName+"ElementInfo");
	if(matchIDs.count() == 0){
		SGMEnergyPosition epStart(elementName%elementEdge%"Start", 1830.0, -171811, -60656, 567.57, 2);
		SGMEnergyPosition epMiddle(elementName%elementEdge%"Middle", 1860.0, -169041, -58574, 601.08, 2);
		SGMEnergyPosition epEnd(elementName%elementEdge%"End", 1890.0, -166359, -56493, 635.07, 2);
		success &= epStart.storeToDb(dbSGM);
		success &= epMiddle.storeToDb(dbSGM);
		success &= epEnd.storeToDb(dbSGM);

		AMAbsorptionEdge edge = AMPeriodicTable::table()->elementBySymbol(elementSymbol)->KEdge();
		SGMScanInfo scanInfo(elementName%" "%elementEdge, edge, epStart, epMiddle, epEnd);
		success &= scanInfo.storeToDb(dbSGM);

		SGMFastScanSettings fs5Settings(elementName%elementEdge%"5sSettings", 5.0, 24000, 5.0, 200, 4000);
		success &= fs5Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp5 = new SGMFastScanParameters(elementName%elementEdge%"5s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs5Settings);
		success &= fsp5->storeToDb(dbSGM);

		SGMFastScanSettings fs20Settings(elementName%elementEdge%"20sSettings", 20.0, 580, 20.0, 200, 220);
		success &= fs20Settings.storeToDb(dbSGM);
		SGMFastScanParameters *fsp20 = new SGMFastScanParameters(elementName%elementEdge%"20s", AMPeriodicTable::table()->elementBySymbol(elementSymbol)->name(), scanInfo, fs20Settings);
		success &= fsp20->storeToDb(dbSGM);

		SGMElementInfo *elementInfo = new SGMElementInfo(elementName%"ElementInfo", AMPeriodicTable::table()->elementBySymbol(elementSymbol), this);
		elementInfo->addEdgeInfo(scanInfo);
		elementInfo->addFastScanParameters(fsp5);
		elementInfo->addFastScanParameters(fsp20);
		success &= elementInfo->storeToDb(dbSGM);
	}

	return success;
}

bool SGMAppController::setupSGMViews(){
	// Create panes in the main window:
	mw_->insertHeading("Beamline Control", 0);

	AMSampleCamera::set()->setSSAManipulatorX(SGMBeamline::sgm()->ssaManipulatorX());
	AMSampleCamera::set()->setSSAManipulatorY(SGMBeamline::sgm()->ssaManipulatorY());
	AMSampleCamera::set()->setSSAManipulatorZ(SGMBeamline::sgm()->ssaManipulatorZ());
	AMSampleCamera::set()->setSSAManipulatorRot(SGMBeamline::sgm()->ssaManipulatorRot());
	sampleManagementView_ = new SGMSampleManagementView();
	mw_->addPane(sampleManagementView_, "Beamline Control", "SGM Sample Management", ":/system-software-update.png");

	SGMAdvancedControls_ = new SGMAdvancedControlsView();
	mw_->addPane(SGMAdvancedControls_, "Beamline Control", "SGM Advanced Controls", ":/system-software-update.png");

	mw_->insertHeading("Beamline Detectors", 1);

	sgmScalerView_ = 0;
	connect(SGMBeamline::sgm()->rawScaler(), SIGNAL(connectedChanged(bool)), this, SLOT(onSGMScalerConnected(bool)));
	onSGMScalerConnected(false);
	sgmSynchronizedDwellTimeView_ = 0;
	connect(SGMBeamline::sgm()->synchronizedDwellTime(), SIGNAL(connected(bool)), this, SLOT(onSGMSynchronizedDwellTimeConnected(bool)));
	onSGMSynchronizedDwellTimeConnected(false);


	amptekSDD1XRFView_ = 0;
	if(SGMBeamline::sgm()->newAmptekSDD1()){
		connect(SGMBeamline::sgm()->newAmptekSDD1(), SIGNAL(connected(bool)), this, SLOT(onSGMNewAmptekSDD1Connected(bool)));
		onSGMNewAmptekSDD1Connected(false);
	}

	amptekSDD2XRFView_ = 0;
	if(SGMBeamline::sgm()->newAmptekSDD2()){
		connect(SGMBeamline::sgm()->newAmptekSDD2(), SIGNAL(connected(bool)), this, SLOT(onSGMNewAmptekSDD2Connected(bool)));
		onSGMNewAmptekSDD2Connected(false);
	}

	amptekSDD3XRFView_ = 0;
	if(SGMBeamline::sgm()->newAmptekSDD3()){
		connect(SGMBeamline::sgm()->newAmptekSDD3(), SIGNAL(connected(bool)), this, SLOT(onSGMNewAmptekSDD3Connected(bool)));
		onSGMNewAmptekSDD3Connected(false);
	}

	amptekSDD4XRFView_ = 0;
	if(SGMBeamline::sgm()->newAmptekSDD4()){
		connect(SGMBeamline::sgm()->newAmptekSDD4(), SIGNAL(connected(bool)), this, SLOT(onSGMNewAmptekSDD4Connected(bool)));
		onSGMNewAmptekSDD4Connected(false);
	}

	amptekSDD5XRFView_ = 0;
	if(SGMBeamline::sgm()->newAmptekSDD5()){
		connect(SGMBeamline::sgm()->newAmptekSDD5(), SIGNAL(connected(bool)), this, SLOT(onSGMNewAmptekSDD5Connected(bool)));
		onSGMNewAmptekSDD5Connected(false);
	}

	newPGTDetectorView_ = 0;
	/*
	connect(SGMBeamline::sgm()->newPGTDetector(), SIGNAL(connected(bool)), this, SLOT(onSGMNewPGTDetectorConnected(bool)));
	onSGMNewPGTDetectorConnected(false);
	*/

	newQE65000DetectorView_ = 0;
	connect(SGMBeamline::sgm()->newQE65000Detector(), SIGNAL(connected(bool)), this, SLOT(onSGMNewQE65000DetectorConnected(bool)));
	onSGMNewQE65000DetectorConnected(false);

	newTEYDetectorView_ = 0;
	connect(SGMBeamline::sgm()->newTEYDetector(), SIGNAL(connected(bool)), this, SLOT(onSGMNewTEYDetectorConnected(bool)));
	onSGMNewTEYDetectorConnected(false);

	mw_->sidebar()->setExpanded(mw_->windowPaneModel()->headingItem("Beamline Detectors")->index(), false);

	mw_->insertHeading("Experiment Setup", 2);

	xasScanConfiguration2013View_ = 0; //NULL
	xasScanConfiguration2013Holder3_ = new AMScanConfigurationViewHolder3();
	mw_->addPane(xasScanConfiguration2013Holder3_, "Experiment Setup", "SGM XAS Scan", ":/utilities-system-monitor.png");

	fastScanConfiguration2013View_ = 0; //NULL
	fastScanConfiguration2013Holder3_ = new AMScanConfigurationViewHolder3();
	mw_->addPane(fastScanConfiguration2013Holder3_, "Experiment Setup", "SGM Fast Scan", ":/utilities-system-monitor.png");

	connect(SGMBeamline::sgm(), SIGNAL(beamlineInitialized()), this, SLOT(onSGMBeamlineConnected()));
	connect(SGMBeamline::sgm(), SIGNAL(criticalConnectionsChanged()), this, SLOT(onSGMBeamlineConnected()));
	connect(SGMBeamline::sgm(), SIGNAL(beamlineReadyChanged()), this, SLOT(onSGMBeamlineConnected()));
	onSGMBeamlineConnected();

	additionalIssueTypesAndAssignees_.append("I think it's an SGM specific issue", "davidChevrier");

	sgmSidebar_ = new SGMSidebar();
	mw_->addRightWidget(sgmSidebar_);

	/*
	SGMPeriodicTableView *sgmPeriodicTableView = new SGMPeriodicTableView(SGMPeriodicTable::sgmTable());
	sgmPeriodicTableView->show();
	SGMFastScanParametersModificationWizard *fastScanWizard = new SGMFastScanParametersModificationWizard();
	fastScanWizard->show();
	*/

	connect(workflowView_, SIGNAL(actionAddedFromDialog(AMAction3*)), this, SLOT(onWorkflowActionAddedFromDialog(AMAction3*)));

	return true;
}
