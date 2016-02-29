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


#include "SXRMBAppController.h"

#include <QMessageBox>

#include "application/AMAppControllerSupport.h"
#include "application/SXRMB/SXRMB.h"
#include "acquaman/SXRMB/SXRMBEXAFSScanConfiguration.h"

#include "beamline/CLS/CLSFacilityID.h"
#include "beamline/CLS/CLSStorageRing.h"
#include "beamline/SXRMB/SXRMBBeamline.h"

#include "actions3/AMActionRunner3.h"
#include "actions3/actions/AMScanAction.h"
#include "actions3/AMListAction3.h"

#include "dataman/AMRun.h"
#include "dataman/AMScan.h"
#include "dataman/AMRegionOfInterest.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/export/AMExportController.h"
#include "dataman/export/AMExporterOptionGeneralAscii.h"
#include "dataman/export/AMExporterGeneralAscii.h"
#include "dataman/export/AMExporterAthena.h"
#include "dataman/export/AMSMAKExporter.h"
#include "dataman/export/AMExporter2DAscii.h"
#include "dataman/SXRMB/SXRMBUserConfiguration.h"
#include "dataman/SXRMB/SXRMBDbUpgrade1pt1.h"

#include "ui/AMMainWindow.h"
#include "ui/AMMotorGroupView.h"
#include "ui/dataman/AMGenericScanEditor.h"
#include "ui/acquaman/AMScanConfigurationViewHolder3.h"
#include "ui/util/AMDialog.h"
#include "ui/util/AMChooseDataFolderDialog.h"

#include "ui/beamline/AMSlitsView.h"

#include "ui/CLS/CLSSIS3820ScalerView.h"
#include "ui/CLS/CLSCrossHairGeneratorControlView.h"

#include "ui/acquaman/SXRMB/SXRMBOxidationMapScanConfigurationViewHolder.h"
#include "ui/SXRMB/SXRMBBrukerDetectorView.h"
#include "ui/SXRMB/SXRMBFourElementVortexDetectorView.h"
#include "ui/SXRMB/SXRMBPersistentView.h"
#include "ui/SXRMB/SXRMBEXAFSScanConfigurationView.h"
#include "ui/SXRMB/SXRMB2DMapScanConfigurationView.h"
#include "ui/SXRMB/SXRMB2DOxidationMapScanConfigurationView.h"
#include "ui/SXRMB/SXRMBHVControlView.h"
#include "ui/SXRMB/SXRMBCrystalChangeView.h"

#include "util/AMErrorMonitor.h"
#include "util/AMPeriodicTable.h"

SXRMBAppController::SXRMBAppController(QObject *parent)
	: AMAppController(parent)
{
	userConfiguration_ = 0;
	moveImmediatelyAction_ = 0;
	ambiantSampleStageMotorGroupView_ = 0;

	setDefaultUseLocalStorage(true);

	// Remember!!!!  Every upgrade needs to be done to the user AND actions databases!
	////////////////////////////////////////////////////////////////////////////////////////
	AMDbUpgrade *sxrmb1Pt1UserDb = new SXRMBDbUpgrade1pt1("user", this);
	appendDatabaseUpgrade(sxrmb1Pt1UserDb);
	AMDbUpgrade *sxrmb1Pt1ActionDb = new SXRMBDbUpgrade1pt1("actions", this);
	appendDatabaseUpgrade(sxrmb1Pt1ActionDb);

}

SXRMBAppController::~SXRMBAppController()
{
	if (ambiantSampleStageMotorGroupView_) {
		ambiantSampleStageMotorGroupView_->deleteLater();
		ambiantSampleStageMotorGroupView_ = 0;
	}
}

bool SXRMBAppController::startup()
{
	// Get a destination folder.
	if (!AMChooseDataFolderDialog::getDataFolder("/AcquamanLocalData/sxrmb", "/home/sxrmb", "acquamanData"))
		return false;

	// Start up the main program.
	if(!AMAppController::startup())
		return false;

	// Initialize central beamline object
	SXRMBBeamline::sxrmb();
	// Initialize the periodic table object.
	AMPeriodicTable::table();

	registerClasses();
	setupOnFirstRun();

	setupExporterOptions();
	setupUserInterface();
	makeConnections();

	// Ensuring we automatically switch scan editors for new scans.
	setAutomaticBringScanEditorToFront(true);

	return true;
}

void SXRMBAppController::shutdown()
{
	// Make sure we release/clean-up the beamline interface
	AMBeamline::releaseBl();
	AMAppController::shutdown();
}

bool SXRMBAppController::startupInstallActions()
{
	if(AMAppController::startupInstallActions()) {
		QAction *switchEndstationAction = new QAction("Switch Beamline Endstation ...", mw_);
		switchEndstationAction->setStatusTip("Switch beamline endstation.");
		connect(switchEndstationAction, SIGNAL(triggered()), this, SLOT(onSwitchBeamlineEndstationTriggered()));

		QAction *ambiantWithGasChamberMotorViewAction = new QAction("See Ambiant Sample Stage Motors ...", mw_);
		ambiantWithGasChamberMotorViewAction->setStatusTip("Display the motors of the Ambiant Sample Stage.");
		connect(ambiantWithGasChamberMotorViewAction, SIGNAL(triggered()), this, SLOT(onShowAmbiantSampleStageMotorsTriggered()));

		viewMenu_->addSeparator();
		viewMenu_->addAction(switchEndstationAction);
		viewMenu_->addAction(ambiantWithGasChamberMotorViewAction);

		return true;
	} else
		return false;
}

void SXRMBAppController::onBeamlineConnected(bool connected)
{
	SXRMBBeamline * sxrmbBL = SXRMBBeamline::sxrmb();

	if (connected && !exafsScanConfigurationView_) {
		exafsScanConfiguration_ = new SXRMBEXAFSScanConfiguration();

		exafsScanConfiguration_->setY(sxrmbBL->microprobeSampleStageY()->value());
		exafsScanConfiguration_->setX(sxrmbBL->microprobeSampleStageX()->value());
		exafsScanConfiguration_->setZ(sxrmbBL->microprobeSampleStageZ()->value());

		exafsScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStart(-11);
		exafsScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStep(0.5);
		exafsScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionEnd(9);
		exafsScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionTime(1.0);

		exafsScanConfigurationView_ = new SXRMBEXAFSScanConfigurationView(exafsScanConfiguration_);
		exafsScanConfigurationViewHolder_ = new AMScanConfigurationViewHolder3("Configure an EXAFS Scan", true, true, exafsScanConfigurationView_);

		mw_->addPane(exafsScanConfigurationViewHolder_, "Scans", "EXAFS Scan", ":/utilites-system-monitor.png");

		connect(exafsScanConfiguration_, SIGNAL(totalTimeChanged(double)), exafsScanConfigurationViewHolder_, SLOT(updateOverallScanTime(double)));
		exafsScanConfigurationViewHolder_->updateOverallScanTime(exafsScanConfiguration_->totalTime());
	}

	if (connected && !microProbe2DScanConfigurationView_) {
		microProbe2DScanConfiguration_ = new SXRMB2DMapScanConfiguration();

		microProbe2DScanConfiguration_->setEnergy(sxrmbBL->energy()->value());

		microProbe2DScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStart(0.0);
		microProbe2DScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStep(0.01);
		microProbe2DScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionEnd(0.1);

		microProbe2DScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionStart(0.0);
		microProbe2DScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionStep(0.01);
		microProbe2DScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionEnd(0.1);

		microProbe2DScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionTime(1.0);
		microProbe2DScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionTime(1.0);

		microProbe2DScanConfigurationView_ = new SXRMB2DMapScanConfigurationView(microProbe2DScanConfiguration_);
		microProbe2DScanConfigurationViewHolder_ = new AMScanConfigurationViewHolder3("SXRMB 2D Map Configuration", true, true, microProbe2DScanConfigurationView_);

	}

	if (connected && !microProbe2DOxidationScanConfigurationView_) {
		microProbe2DOxidationScanConfiguration_ = new SXRMB2DMapScanConfiguration();
		microProbe2DOxidationScanConfiguration_->setName("Oxidation Map");
		microProbe2DOxidationScanConfiguration_->setUserScanName("Oxidation Map");

		microProbe2DScanConfiguration_->setEnergy(sxrmbBL->energy()->value());

		microProbe2DOxidationScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStart(0.0);
		microProbe2DOxidationScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStep(0.01);
		microProbe2DOxidationScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionEnd(0.1);

		microProbe2DOxidationScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionStart(0.0);
		microProbe2DOxidationScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionStep(0.01);
		microProbe2DOxidationScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionEnd(0.1);

		microProbe2DOxidationScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionTime(1.0);
		microProbe2DOxidationScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionTime(1.0);

		microProbe2DOxidationScanConfigurationView_ = new SXRMB2DOxidationMapScanConfigurationView(microProbe2DOxidationScanConfiguration_);
		microProbe2DOxidationScanConfigurationViewHolder_ = new SXRMBOxidationMapScanConfigurationViewHolder(microProbe2DOxidationScanConfigurationView_);

	}

	if (connected && !sxrmbPersistentView_){

		sxrmbPersistentView_ = new SXRMBPersistentView();
		mw_->addRightWidget(sxrmbPersistentView_);
	}

	if (connected && !userConfiguration_){
		userConfiguration_ = new SXRMBUserConfiguration(this);

		// It is sufficient to only connect the user configuration to the single element because the single element and four element are synchronized together.
		connect(userConfiguration_, SIGNAL(loadedFromDb()), this, SLOT(onUserConfigurationLoadedFromDb()));

		if (!userConfiguration_->loadFromDb(AMDatabase::database("user"), 1)){
			userConfiguration_->storeToDb(AMDatabase::database("user"));

			AMDetector *detector = sxrmbBL->brukerDetector();
			// This is connected here because we want to listen to the detectors for updates, but don't want to double add regions on startup.
			connect(detector, SIGNAL(addedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestAdded(AMRegionOfInterest*)));
			connect(detector, SIGNAL(removedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestRemoved(AMRegionOfInterest*)));
			connect(detector, SIGNAL(regionOfInterestBoundingRangeChanged(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestBoundingRangeChanged(AMRegionOfInterest*)));
		}
	}

	if (connected) {
		onBeamlineEndstationSwitched(sxrmbBL->currentEndstation(), sxrmbBL->currentEndstation());
		onScalerConnected(sxrmbBL->scaler()->isConnected());
	}
}

void SXRMBAppController::onBeamControlShuttersTimeout()
{
	QString errorMessage = "One (several) Beamline Valve/PSH shutter(s) can't be connected. Please contact beamline staff. This might affect your usage of Acuqaman.";
	AMErrorMon::alert(this, ERR_SXRMB_SHUTTERS_TIMEOUT, errorMessage, true);
}

void SXRMBAppController::onBeamlineEndstationSwitched(SXRMB::Endstation fromEndstation, SXRMB::Endstation toEndstation)
{
	Q_UNUSED(fromEndstation)

	if (toEndstation == SXRMB::InvalidEndstation)
		return;

	if (!microProbe2DScanConfiguration_ || !exafsScanConfiguration_ || !microProbe2DOxidationScanConfiguration_)
		return;

	microProbe2DScanConfiguration_->setEndstation(toEndstation);
	exafsScanConfiguration_->setEndstation(toEndstation);
	microProbe2DOxidationScanConfiguration_->setEndstation(toEndstation);

	if (toEndstation == SXRMB::Microprobe){

		mw_->addPane(microProbe2DScanConfigurationViewHolder_, "Scans", "2D Scan", ":/utilites-system-monitor.png");
		mw_->addPane(microProbe2DOxidationScanConfigurationViewHolder_, "Scans", "Oxidation Map", ":/utilites-system-monitor.png");
	}

	else {

		mw_->removePane(microProbe2DScanConfigurationViewHolder_);
		mw_->removePane(microProbe2DOxidationScanConfigurationViewHolder_);

		microProbe2DScanConfigurationViewHolder_->hide();
		microProbe2DOxidationScanConfigurationViewHolder_->hide();
	}
}

void SXRMBAppController::onBeamAvailabilityChanged(bool beamAvailable)
{
	if (!beamAvailable && !AMActionRunner3::workflow()->pauseCurrentAction())
		AMActionRunner3::workflow()->setQueuePaused(true);

	// On SXRMB, we don't like having the scan restart on it's own.
	else if (beamAvailable && AMActionRunner3::workflow()->queuedActionCount() > 0)
		AMActionRunner3::workflow()->setQueuePaused(false);
}

void SXRMBAppController::onScalerConnected(bool isConnected){
	if(isConnected && SXRMBBeamline::sxrmb()->isConnected()){
		if(!scalerView_){

			scalerView_ = new CLSSIS3820ScalerView(SXRMBBeamline::sxrmb()->scaler());
			scalerView_->setAmplifierViewFormat('g');
			scalerView_->setAmplifierViewPrecision(3);
		}

		mw_->addPane(AMMainWindow::buildMainWindowPane("Scaler", ":/system-search.png", scalerView_), "Detectors", "Scaler", ":/system-search.png", true);
	}
	else if(scalerView_)
		mw_->removePane(scalerView_);
}

void SXRMBAppController::registerClasses()
{
	AMDbObjectSupport::s()->registerClass<SXRMBScanConfigurationDbObject>();
	AMDbObjectSupport::s()->registerClass<SXRMBEXAFSScanConfiguration>();
	AMDbObjectSupport::s()->registerClass<SXRMB2DMapScanConfiguration>();
	AMDbObjectSupport::s()->registerClass<SXRMBUserConfiguration>();
}

void SXRMBAppController::setupOnFirstRun()
{
	// Some first time things.
	AMRun existingRun;
	// We'll use loading a run from the db as a sign of whether this is the first time an application has been run because startupIsFirstTime will return false after the user data folder is created.
	if (!existingRun.loadFromDb(AMDatabase::database("user"), 1)){
		AMRun firstRun(CLSFacilityID::beamlineName(CLSFacilityID::SXRMBBeamline), CLSFacilityID::SXRMBBeamline); //9: SXRMB Beamline
		firstRun.storeToDb(AMDatabase::database("user"));
	}
}

void SXRMBAppController::setupExporterOptions()
{
	AMExporterOptionGeneralAscii *sxrmbExportOptions = SXRMB::buildStandardExporterOption("SXRMBXASDefault", true, true, true, true);
	if(sxrmbExportOptions->id() > 0)
		AMAppControllerSupport::registerClass<SXRMBEXAFSScanConfiguration, AMExporterAthena, AMExporterOptionGeneralAscii>(sxrmbExportOptions->id());

	sxrmbExportOptions = SXRMB::buildSMAKExporterOption("SXRMB2DDefault", true, false, false, true);
	if(sxrmbExportOptions->id() > 0)
		AMAppControllerSupport::registerClass<SXRMB2DMapScanConfiguration, AMSMAKExporter, AMExporterOptionSMAK>(sxrmbExportOptions->id());

	sxrmbExportOptions = SXRMB::buildStandardExporterOption("SXRMB2DDefault", true, false, false, true);
	if(sxrmbExportOptions->id() > 0)
		AMAppControllerSupport::registerClass<SXRMB2DMapScanConfiguration, AMExporter2DAscii, AMExporterOptionGeneralAscii>(sxrmbExportOptions->id());
}

void SXRMBAppController::setupUserInterface()
{
	exafsScanConfiguration_ = 0; //NULL
	exafsScanConfigurationView_ = 0; //NULL
	exafsScanConfigurationViewHolder_ = 0; //NULL

	microProbe2DScanConfiguration_ = 0; //NULL
	microProbe2DScanConfigurationView_ = 0; //NULL
	microProbe2DScanConfigurationViewHolder_ = 0; //NULL

	microProbe2DOxidationScanConfiguration_ = 0; //NULL
	microProbe2DOxidationScanConfigurationView_ = 0; //NULL
	microProbe2DOxidationScanConfigurationViewHolder_ = 0; //NULL

	scalerView_ = 0; //NULL
	sxrmbPersistentView_ = 0;


	// Create panes in the main window:
	////////////////////////////////////

	// General heading
	mw_->insertHeading("General", 0);

	SXRMBHVControlView *hvControlView = new SXRMBHVControlView(SXRMBBeamline::sxrmb()->beamlineHVControlSet(), false);
	CLSCrossHairGeneratorControlView *crossHairView = new CLSCrossHairGeneratorControlView(SXRMBBeamline::sxrmb()->crossHairGenerator());
	SXRMBCrystalChangeView *crystalChangeView = new SXRMBCrystalChangeView(SXRMBBeamline::sxrmb()->crystalSelection());
	AMSlitsView *jjSlitsView = new AMSlitsView(SXRMBBeamline::sxrmb()->jjSlits());

	mw_->addPane(createTopFrameSqueezeContent(hvControlView, "HV Controls"), "General", "HV Controls", ":/system-search.png");
	mw_->addPane(createTopFrameSqueezeContent(crossHairView, "Video Cross hairs"), "General", "Cross Hairs", ":/system-search.png", true);
	mw_->addPane(createTopFrameSqueezeContent(crystalChangeView, "Crystal Selection"), "General", "Crystal Change", ":/system-search.png", true);
	mw_->addPane(createTopFrameSqueezeContent(jjSlitsView, "Crystal Selection"), "General", "Slit View", ":/system-search.png", true);

	// Detectors heading
	mw_->insertHeading("Detectors", 1);

	SXRMBBrukerDetectorView *brukerView = new SXRMBBrukerDetectorView(SXRMBBeamline::sxrmb()->brukerDetector());
	brukerView->buildDetectorView();
	brukerView->setEnergyRange(1700, 10000);
	brukerView->addEmissionLineNameFilter(QRegExp("1"));
	brukerView->addPileUpPeakNameFilter(QRegExp("(K.1|L.1|Ma1)"));
	brukerView->addCombinationPileUpPeakNameFilter(QRegExp("(Ka1|La1|Ma1)"));
	brukerView->enableDeadTimeDisplay();
	mw_->addPane(brukerView, "Detectors", "Bruker", ":/system-search.png");

	SXRMBFourElementVortexDetectorView *fourElementVortexView = new SXRMBFourElementVortexDetectorView(SXRMBBeamline::sxrmb()->fourElementVortexDetector());
	fourElementVortexView->buildDetectorView();
	fourElementVortexView->setEnergyRange(1700, 10000);
	fourElementVortexView->addEmissionLineNameFilter(QRegExp("1"));
	fourElementVortexView->addPileUpPeakNameFilter(QRegExp("(K.1|L.1|Ma1)"));
	fourElementVortexView->addCombinationPileUpPeakNameFilter(QRegExp("(Ka1|La1|Ma1)"));

	mw_->addPane(fourElementVortexView, "Detectors", "4-el Vortex", ":/system-search.png");

	// Scans heading
	mw_->insertHeading("Scans", 2);
}

void SXRMBAppController::makeConnections()
{
	SXRMBBeamline *sxrmbBL = SXRMBBeamline::sxrmb();

	connect(this, SIGNAL(scanEditorCreated(AMGenericScanEditor*)), this, SLOT(onScanEditorCreated(AMGenericScanEditor*)));

	connect(sxrmbBL, SIGNAL(connected(bool)), this, SLOT(onBeamlineConnected(bool)));
	connect(sxrmbBL, SIGNAL(beamlineControlShuttersTimeout()), this, SLOT(onBeamControlShuttersTimeout()));
	connect(sxrmbBL, SIGNAL(endstationChanged(SXRMB::Endstation, SXRMB::Endstation)), this, SLOT(onBeamlineEndstationSwitched(SXRMB::Endstation, SXRMB::Endstation)));
	connect(sxrmbBL->scaler(), SIGNAL(connectedChanged(bool)), this, SLOT(onScalerConnected(bool)));

	onBeamlineConnected(sxrmbBL->isConnected());
}

QGroupBox* SXRMBAppController::createTopFrameSqueezeContent(QWidget *widget, QString topFrameTitle)
{
	QHBoxLayout *horizontalSqueezeLayout = new QHBoxLayout;
	horizontalSqueezeLayout->addStretch();
	horizontalSqueezeLayout->addWidget(widget);
	horizontalSqueezeLayout->addStretch();

	QVBoxLayout *verticalSqueezeLayout = new QVBoxLayout;
	verticalSqueezeLayout->addWidget(new AMTopFrame(topFrameTitle));
	verticalSqueezeLayout->addStretch();
	verticalSqueezeLayout->addLayout(horizontalSqueezeLayout);
	verticalSqueezeLayout->addStretch();

	QGroupBox *controlGroupBox = new QGroupBox;
	controlGroupBox->setFlat(true);
	controlGroupBox->setLayout(verticalSqueezeLayout);

	return 	controlGroupBox;
}


void SXRMBAppController::onCurrentScanActionStartedImplementation(AMScanAction *action)
{
	Q_UNUSED(action)

	// start to listen to the beamAvaliability signal for scan auto-pause purpose
	connect(SXRMBBeamline::sxrmb(), SIGNAL(beamAvaliability(bool)), this, SLOT(onBeamAvailabilityChanged(bool)));

	userConfiguration_->storeToDb(AMDatabase::database("user"));
}

void SXRMBAppController::onCurrentScanActionFinishedImplementation(AMScanAction *action)
{
	Q_UNUSED(action)

	// stop listening to the beamAvaliability signal for scan auto-pause purpose
	disconnect(SXRMBBeamline::sxrmb(), SIGNAL(beamAvaliability(bool)), this, SLOT(onBeamAvailabilityChanged(bool)));

	// Save the current configuration to the database.
	// Being explicit due to the nature of how many casts were necessary.  I could probably explicitly check to ensure each cast is successful, but I'll risk it for now.
	const AMScanActionInfo *actionInfo = qobject_cast<const AMScanActionInfo *>(action->info());
	const SXRMBScanConfiguration *sxrmbScanConfig = dynamic_cast<const SXRMBScanConfiguration *>(actionInfo->configuration());
	SXRMBScanConfigurationDbObject *configDB = qobject_cast<SXRMBScanConfigurationDbObject *>(sxrmbScanConfig->dbObject());

	if (configDB){
		userConfiguration_->setFluorescenceDetector(configDB->fluorescenceDetector());
		userConfiguration_->storeToDb(AMDatabase::database("user"));
	}
}

void SXRMBAppController::onUserConfigurationLoadedFromDb()
{
	AMXRFDetector *detector = SXRMBBeamline::sxrmb()->brukerDetector();

	foreach (AMRegionOfInterest *region, userConfiguration_->regionsOfInterest()){
		detector->addRegionOfInterest(region->createCopy());
		microProbe2DScanConfiguration_->addRegionOfInterest(region);
		exafsScanConfiguration_->addRegionOfInterest(region);
		microProbe2DOxidationScanConfiguration_->addRegionOfInterest(region);
	}

	// This is connected here because we want to listen to the detectors for updates, but don't want to double add regions on startup.
	connect(detector, SIGNAL(addedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestAdded(AMRegionOfInterest*)));
	connect(detector, SIGNAL(removedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestRemoved(AMRegionOfInterest*)));
	connect(detector, SIGNAL(regionOfInterestBoundingRangeChanged(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestBoundingRangeChanged(AMRegionOfInterest*)));
}

void SXRMBAppController::onRegionOfInterestAdded(AMRegionOfInterest *region)
{
	userConfiguration_->addRegionOfInterest(region);
	microProbe2DScanConfiguration_->addRegionOfInterest(region);
	exafsScanConfiguration_->addRegionOfInterest(region);
	microProbe2DOxidationScanConfiguration_->addRegionOfInterest(region);
}

void SXRMBAppController::onRegionOfInterestRemoved(AMRegionOfInterest *region)
{
	userConfiguration_->removeRegionOfInterest(region);
	microProbe2DScanConfiguration_->removeRegionOfInterest(region);
	exafsScanConfiguration_->removeRegionOfInterest(region);
	microProbe2DOxidationScanConfiguration_->removeRegionOfInterest(region);
}

void SXRMBAppController::onRegionOfInterestBoundingRangeChanged(AMRegionOfInterest *region)
{
	userConfiguration_->setRegionOfInterestBoundingRange(region);
	microProbe2DScanConfiguration_->setRegionOfInterestBoundingRange(region);
	exafsScanConfiguration_->setRegionOfInterestBoundingRange(region);
	microProbe2DOxidationScanConfiguration_->setRegionOfInterestBoundingRange(region);
}

void SXRMBAppController::onShowAmbiantSampleStageMotorsTriggered()
{
	if (!ambiantSampleStageMotorGroupView_) {
		QString motorGroupName = SXRMBBeamline::sxrmb()->ambiantWithoutGasChamberSampleStageMotorGroupObject()->name();

		AMExtendedControlEditor * ambiantTableHeightControlEditor = new AMExtendedControlEditor(SXRMBBeamline::sxrmb()->ambiantTableHeight());
		ambiantTableHeightControlEditor->setControlFormat('f', 2);
		ambiantTableHeightControlEditor->hideBorder();

		QToolButton * stopButton = new QToolButton;
		stopButton->setIcon(QIcon(":/stop.png"));
		connect(stopButton, SIGNAL(clicked()), SXRMBBeamline::sxrmb()->ambiantTableHeight(), SLOT(stop()));

		QHBoxLayout *tableHeightLayout = new QHBoxLayout;
		tableHeightLayout->addWidget(new QLabel("Table Height"));
		tableHeightLayout->addWidget(ambiantTableHeightControlEditor);
		tableHeightLayout->addWidget(stopButton);

		ambiantSampleStageMotorGroupView_ = new AMMotorGroupView(SXRMBBeamline::sxrmb()->motorGroup(), AMMotorGroupView::CompactView);
		ambiantSampleStageMotorGroupView_->setSelectedGroupObject(motorGroupName);
		ambiantSampleStageMotorGroupView_->hideMotorGroupSelection();

		QVBoxLayout* motorGroupViewLayout = qobject_cast<QVBoxLayout *> (ambiantSampleStageMotorGroupView_->layout());
		motorGroupViewLayout->addLayout(tableHeightLayout);
	}

	ambiantSampleStageMotorGroupView_->raise();
	ambiantSampleStageMotorGroupView_->showNormal();
}

void SXRMBAppController::onSwitchBeamlineEndstationTriggered()
{
	QStringList endstations;
	endstations.append(SXRMB::sxrmbEndstationName(SXRMB::SolidState));
	endstations.append(SXRMB::sxrmbEndstationName(SXRMB::AmbiantWithGasChamber));
	endstations.append(SXRMB::sxrmbEndstationName(SXRMB::AmbiantWithoutGasChamber));
	endstations.append(SXRMB::sxrmbEndstationName(SXRMB::Microprobe));

	QComboBox *availableBeamlineEndstations = new QComboBox;
	availableBeamlineEndstations->addItems(endstations);
	availableBeamlineEndstations->setCurrentIndex(SXRMBBeamline::sxrmb()->currentEndstation() - 1);

	AMDialog *switchEndstationDialog = new AMDialog("Switch SXRMB Endstation");
	switchEndstationDialog->layoutDialogContent(availableBeamlineEndstations);

	if (switchEndstationDialog->exec()) {

		SXRMB::Endstation newEndstation = SXRMB::Endstation(availableBeamlineEndstations->currentIndex() + 1);
		SXRMBBeamline::sxrmb()->switchEndstation(newEndstation);
	}
}

void SXRMBAppController::onScanEditorCreated(AMGenericScanEditor *editor)
{
	connect(editor, SIGNAL(scanAdded(AMGenericScanEditor*,AMScan*)), this, SLOT(onScanAddedToEditor(AMGenericScanEditor*,AMScan*)));
	editor->setEnergyRange(1700, 10000);

	if (editor->using2DScanView())
		connect(editor, SIGNAL(dataPositionChanged(AMGenericScanEditor*,QPoint)), this, SLOT(onDataPositionChanged(AMGenericScanEditor*,QPoint)));
}

void SXRMBAppController::onScanAddedToEditor(AMGenericScanEditor *editor, AMScan *scan)
{
	QString exclusiveName = QString();

	for (int i = 0, count = scan->analyzedDataSourceCount(); i < count && exclusiveName.isNull(); i++){

		AMDataSource *source = scan->analyzedDataSources()->at(i);

		if (source->name().contains("norm_") && !source->name().contains("norm_PFY") && !source->hiddenFromUsers())
			exclusiveName = source->name();
	}

	if (!exclusiveName.isNull())
		editor->setExclusiveDataSourceByName(exclusiveName);

	else if (editor->scanAt(0)->analyzedDataSourceCount())
		editor->setExclusiveDataSourceByName(editor->scanAt(0)->analyzedDataSources()->at(editor->scanAt(0)->analyzedDataSourceCount()-1)->name());

	configureSingleSpectrumView(editor, scan);
}

void SXRMBAppController::configureSingleSpectrumView(AMGenericScanEditor *editor, AMScan *scan)
{
	int scanRank = scan->scanRank();
	QStringList spectraNames;

	for (int i = 0, size = scan->dataSourceCount(); i < size; i++)
		if (scan->dataSourceAt(i)->rank()-scanRank == 1)
			spectraNames << scan->dataSourceAt(i)->name();

	if (!spectraNames.isEmpty())
		editor->setSingleSpectrumViewDataSourceName(spectraNames.first());

	editor->setEnergyRange(1700, 10000);
	editor->addSingleSpectrumEmissionLineNameFilter(QRegExp("1"));
	editor->addSingleSpectrumPileUpPeakNameFilter(QRegExp("(K.1|L.1|Ma1)"));
	editor->addSingleSpectrumCombinationPileUpPeakNameFilter(QRegExp("(Ka1|La1|Ma1)"));
}

void SXRMBAppController::onDataPositionChanged(AMGenericScanEditor *editor, const QPoint &pos)
{
	// This should always succeed because the only way to get into this function is using the 2D scan view which currently only is accessed by 2D scans.
	SXRMB2DMapScanConfiguration *config = qobject_cast<SXRMB2DMapScanConfiguration *>(editor->currentScan()->scanConfiguration());

	if (!config)
		return;

	QString text;

	text = QString("Setup at (H,V,N): (%1 mm, %2 mm, %3 mm)")
			.arg(editor->dataPosition().x(), 0, 'f', 3)
			.arg(editor->dataPosition().y(), 0, 'f', 3)
			.arg(config->y());

	QMenu popup(text, editor);
	QAction *temp = popup.addAction(text);
	popup.addSeparator();
	popup.addAction("XANES scan");
	popup.addAction("EXAFS scan");
	popup.addSeparator();
	popup.addAction("Go to immediately");
	popup.addSeparator();
	temp = popup.addAction("2D XRF Scan");
	temp->setDisabled(editor->selectedRect().isNull());

	temp = popup.exec(pos);

	if (temp){

		if (temp->text() == "XANES scan")
			setupXASScan(editor, false);

		else if (temp->text() == "EXAFS scan")
			setupXASScan(editor, true);

		else if (temp->text() == "Go to immediately")
			moveImmediately(editor);

		else if (temp->text() == "2D XRF Scan")
			setup2DXRFScan(editor);
	}
}

void SXRMBAppController::moveImmediately(const AMGenericScanEditor *editor)
{
	// This should always succeed because the only way to get into this function is using the 2D scan view which currently only is accessed by 2D scans.
	SXRMB2DMapScanConfiguration *config = qobject_cast<SXRMB2DMapScanConfiguration *>(editor->currentScan()->scanConfiguration());

	if (!config)
		return;

	moveImmediatelyAction_ = new AMListAction3(new AMListActionInfo3("Move immediately", "Moves sample stage to given coordinates."), AMListAction3::Sequential);

	moveImmediatelyAction_->addSubAction(SXRMBBeamline::sxrmb()->microprobeSampleStageMotorGroupObject()->horizontalAxis()->createTranslateMoveAction(editor->dataPosition().x()));
	moveImmediatelyAction_->addSubAction(SXRMBBeamline::sxrmb()->microprobeSampleStageMotorGroupObject()->verticalAxis()->createTranslateMoveAction(editor->dataPosition().y()));
	moveImmediatelyAction_->addSubAction(SXRMBBeamline::sxrmb()->microprobeSampleStageMotorGroupObject()->normalAxis()->createTranslateMoveAction(config->y()));


	connect(moveImmediatelyAction_, SIGNAL(succeeded()), this, SLOT(onMoveImmediatelySuccess()));
	connect(moveImmediatelyAction_, SIGNAL(failed()), this, SLOT(onMoveImmediatelyFailure()));
	moveImmediatelyAction_->start();
}

void SXRMBAppController::onMoveImmediatelySuccess()
{
	cleanMoveImmediatelyAction();
}

void SXRMBAppController::onMoveImmediatelyFailure()
{
	cleanMoveImmediatelyAction();
	QMessageBox::warning(mw_, "Sample Stage Error", "The sample stage was unable to complete the desired movement.");
}

void SXRMBAppController::cleanMoveImmediatelyAction()
{
	if (moveImmediatelyAction_ == 0)
		return;

	// Disconnect all signals and return all memory.
	moveImmediatelyAction_->disconnect();
	moveImmediatelyAction_->deleteLater();
	moveImmediatelyAction_ = 0;
}

void SXRMBAppController::setupXASScan(const AMGenericScanEditor *editor, bool setupEXAFS)
{
	// THIS NEEDS REAL COORDINATES.

	QString edge = editor->exclusiveDataSourceName();
	edge = edge.remove("norm_");
	edge.chop(2);
	edge.insert(edge.size()-1, " ");

	if (edge.at(edge.size()-1) == 'L')
		edge.append("1");

	exafsScanConfiguration_->setEdge(edge);
	exafsScanConfiguration_->setX(editor->dataPosition().x());
	exafsScanConfiguration_->setZ(editor->dataPosition().y());

	qDebug() << "Config " << exafsScanConfiguration_->x() << exafsScanConfiguration_->z() << " editor " << editor->dataPosition().x() << editor->dataPosition().y();

	// This should always succeed because the only way to get into this function is using the 2D scan view which currently only is accessed by 2D scans.
	SXRMB2DMapScanConfiguration *configuration = qobject_cast<SXRMB2DMapScanConfiguration *>(editor->currentScan()->scanConfiguration());
	if (configuration){

		exafsScanConfiguration_->setName(configuration->name());
		exafsScanConfiguration_->setY(configuration->y());
	}

	if (setupEXAFS)
		exafsScanConfigurationView_->setupDefaultEXAFSScanRegions();

	else
		exafsScanConfigurationView_->setupDefaultXANESScanRegions();

	mw_->undock(exafsScanConfigurationViewHolder_);
}

void SXRMBAppController::setup2DXRFScan(const AMGenericScanEditor *editor)
{
	QRectF mapRect = editor->selectedRect();

	// This should always succeed because the only way to get into this function is using the 2D scan view which currently only is accessed by 2D scans.
	SXRMB2DMapScanConfiguration *config = qobject_cast<SXRMB2DMapScanConfiguration *>(editor->currentScan()->scanConfiguration());

	if (config){

		microProbe2DScanConfiguration_->setName(config->name());
		microProbe2DScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStart(mapRect.left());
		microProbe2DScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStep(0.01);
		microProbe2DScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionEnd(mapRect.right());
		microProbe2DScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionTime(config->scanAxisAt(0)->regionAt(0)->regionTime());
		microProbe2DScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionStart(mapRect.bottom());
		microProbe2DScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionStep(0.01);
		microProbe2DScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionEnd(mapRect.top());
		microProbe2DScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionTime(config->scanAxisAt(1)->regionAt(0)->regionTime());
		microProbe2DScanConfiguration_->setY(config->y());
		microProbe2DScanConfigurationView_->updateMapInfo();
	}

	mw_->undock(microProbe2DScanConfigurationViewHolder_);
}
