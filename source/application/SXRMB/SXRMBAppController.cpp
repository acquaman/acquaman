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
#include "ui/CLS/CLSHVControlGroupView.h"

#include "ui/acquaman/SXRMB/SXRMBOxidationMapScanConfigurationViewHolder.h"
#include "ui/SXRMB/SXRMBBrukerDetectorView.h"
#include "ui/SXRMB/SXRMBFourElementVortexDetectorView.h"
#include "ui/SXRMB/SXRMBPersistentView.h"
#include "ui/SXRMB/SXRMBEXAFSScanConfigurationView.h"
#include "ui/SXRMB/SXRMB2DMapScanConfigurationView.h"
#include "ui/SXRMB/SXRMB2DOxidationMapScanConfigurationView.h"
#include "ui/SXRMB/SXRMBCrystalChangeView.h"

#include "util/AMErrorMonitor.h"

SXRMBAppController::SXRMBAppController(QObject *parent)
	: CLSAppController("SXRMB", parent)
{
	// initialize the system configuration
	setDefaultUseLocalStorage(true);

	// Remember!!!!  Every upgrade needs to be done to the user AND actions databases!
	////////////////////////////////////////////////////////////////////////////////////////
	AMDbUpgrade *sxrmb1Pt1UserDb = new SXRMBDbUpgrade1pt1("user", this);
	appendDatabaseUpgrade(sxrmb1Pt1UserDb);
	AMDbUpgrade *sxrmb1Pt1ActionDb = new SXRMBDbUpgrade1pt1("actions", this);
	appendDatabaseUpgrade(sxrmb1Pt1ActionDb);

	// initialize the class properties
	userConfiguration_ = new SXRMBUserConfiguration(this);
	moveImmediatelyAction_ = 0;

	ambiantSampleStageMotorGroupView_ = 0;
	sxrmbPersistentView_ = 0;

	exafsScanConfiguration_ = 0; //NULL
	exafsScanConfigurationView_ = 0; //NULL
	exafsScanConfigurationViewHolder_ = 0; //NULL

	microProbe2DScanConfiguration_ = 0; //NULL
	microProbe2DScanConfigurationView_ = 0; //NULL
	microProbe2DScanConfigurationViewHolder_ = 0; //NULL

	microProbe2DOxidationScanConfiguration_ = 0; //NULL
	microProbe2DOxidationScanConfigurationView_ = 0; //NULL
	microProbe2DOxidationScanConfigurationViewHolder_ = 0; //NULL
}

SXRMBAppController::~SXRMBAppController()
{
	if (ambiantSampleStageMotorGroupView_) {
		ambiantSampleStageMotorGroupView_->deleteLater();
		ambiantSampleStageMotorGroupView_ = 0;
	}
}

bool SXRMBAppController::startupInstallActions()
{
	if(CLSAppController::startupInstallActions()) {
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

void SXRMBAppController::onBeamControlShuttersTimeout()
{
	QString errorMessage = "One (several) Beamline Valve/PSH shutter(s) can't be connected. Please contact beamline staff. This might affect your usage of Acuqaman.";
	AMErrorMon::alert(this, ERR_SXRMB_SHUTTERS_TIMEOUT, errorMessage, true);
}

void SXRMBAppController::onBeamlineEndstationSwitched(SXRMB::Endstation fromEndstation, SXRMB::Endstation toEndstation)
{
	Q_UNUSED(fromEndstation)

	if (toEndstation == SXRMB::UnkownEndstation)
		return;

	if (!microProbe2DScanConfiguration_ || !exafsScanConfiguration_ || !microProbe2DOxidationScanConfiguration_)
		return;

	microProbe2DScanConfiguration_->setEndstation(toEndstation);
	exafsScanConfiguration_->setEndstation(toEndstation);
	microProbe2DOxidationScanConfiguration_->setEndstation(toEndstation);

	if (toEndstation == SXRMB::Microprobe){
		mw_->addPane(microProbe2DScanConfigurationViewHolder_, scanPaneCategoryName_, "2D Scan", scanPaneIcon_);
		mw_->addPane(microProbe2DOxidationScanConfigurationViewHolder_, scanPaneCategoryName_, "Oxidation Map", scanPaneIcon_);
	}else {
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

bool SXRMBAppController::setupDataFolder()
{
	// Get a destination folder.
	return AMChooseDataFolderDialog::getDataFolder("/AcquamanLocalData/sxrmb",  //local directory
												   "/home/sxrmb",               //remote directory
												   "acquamanData",              //data directory
												   QStringList());              //extra data directory
}

void SXRMBAppController::initializeBeamline()
{
	// Initialize central beamline object
	SXRMBBeamline * sxrmbBL = SXRMBBeamline::sxrmb();

	connect(sxrmbBL, SIGNAL(beamlineControlShuttersTimeout()), this, SLOT(onBeamControlShuttersTimeout()));
	connect(sxrmbBL, SIGNAL(endstationChanged(SXRMB::Endstation, SXRMB::Endstation)), this, SLOT(onBeamlineEndstationSwitched(SXRMB::Endstation, SXRMB::Endstation)));
}

void SXRMBAppController::registerDBClasses()
{
	AMDbObjectSupport::s()->registerClass<SXRMBScanConfigurationDbObject>();
	AMDbObjectSupport::s()->registerClass<SXRMBEXAFSScanConfiguration>();
	AMDbObjectSupport::s()->registerClass<SXRMB2DMapScanConfiguration>();
	AMDbObjectSupport::s()->registerClass<SXRMBUserConfiguration>();
}

void SXRMBAppController::registerExporterOptions()
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

void SXRMBAppController::setupScanConfigurations()
{
	SXRMBBeamline *sxrmbBl = SXRMBBeamline::sxrmb();
	SXRMB::Endstation currentEndStation = sxrmbBl->currentEndstation();
	AMPVwStatusControl *sampleStageMotor;

	// initialize the EXAFS scan configuration
	exafsScanConfiguration_ = new SXRMBEXAFSScanConfiguration();
	exafsScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStart(-11);
	exafsScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStep(0.5);
	exafsScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionEnd(9);
	exafsScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionTime(1.0);

	sampleStageMotor = sxrmbBl->endstationSampleStageY(currentEndStation);
	if (sampleStageMotor)
		exafsScanConfiguration_->setY(sampleStageMotor->value());

	sampleStageMotor = sxrmbBl->endstationSampleStageX(currentEndStation);
	if (sampleStageMotor)
		exafsScanConfiguration_->setX(sampleStageMotor->value());

	sampleStageMotor = sxrmbBl->endstationSampleStageZ(currentEndStation);
	if (sampleStageMotor)
		exafsScanConfiguration_->setZ(sampleStageMotor->value());

	sampleStageMotor = sxrmbBl->endstationSampleStageR(currentEndStation);
	if (sampleStageMotor)
		exafsScanConfiguration_->setRotation(sampleStageMotor->value());

	// initialize the Microprobe 2D scan configuration
	microProbe2DScanConfiguration_ = new SXRMB2DMapScanConfiguration();
	microProbe2DScanConfiguration_->setEnergy(sxrmbBl->energy()->value());
	microProbe2DScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStart(0.0);
	microProbe2DScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStep(0.01);
	microProbe2DScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionEnd(0.1);
	microProbe2DScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionStart(0.0);
	microProbe2DScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionStep(0.01);
	microProbe2DScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionEnd(0.1);
	microProbe2DScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionTime(1.0);
	microProbe2DScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionTime(1.0);

	// initialize the Microprobe 2D Oxidation scan configuration
	microProbe2DOxidationScanConfiguration_ = new SXRMB2DMapScanConfiguration();
	microProbe2DOxidationScanConfiguration_->setName("Oxidation Map");
	microProbe2DOxidationScanConfiguration_->setUserScanName("Oxidation Map");
	microProbe2DOxidationScanConfiguration_->setEnergy(sxrmbBl->energy()->value());
	microProbe2DOxidationScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStart(0.0);
	microProbe2DOxidationScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStep(0.01);
	microProbe2DOxidationScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionEnd(0.1);
	microProbe2DOxidationScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionStart(0.0);
	microProbe2DOxidationScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionStep(0.01);
	microProbe2DOxidationScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionEnd(0.1);
	microProbe2DOxidationScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionTime(1.0);
	microProbe2DOxidationScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionTime(1.0);
}

void SXRMBAppController::createPersistentView()
{
	sxrmbPersistentView_ = new SXRMBPersistentView();
	mw_->addRightWidget(sxrmbPersistentView_);
}

void SXRMBAppController::createGeneralPanes()
{
	SXRMBBeamline *sxrmbBl = SXRMBBeamline::sxrmb();
	QWidget * generalPaneWidget;

	CLSHVControlGroupView *hvControlView = new CLSHVControlGroupView(sxrmbBl->beamlineHVControlSet(), false);
	generalPaneWidget = AMMainWindow::buildMainWindowPane("HV Controls", generalPaneIcon_, hvControlView);
	mw_->addPane(generalPaneWidget, generalPaneCategeryName_, "HV Controls", generalPaneIcon_);

	CLSCrossHairGeneratorControlView *crossHairView = new CLSCrossHairGeneratorControlView(sxrmbBl->crossHairGenerator());
	generalPaneWidget = AMMainWindow::buildMainWindowPane("Cross hairs", generalPaneIcon_, crossHairView);
	mw_->addPane(generalPaneWidget, generalPaneCategeryName_, "Cross Hairs", generalPaneIcon_);

	SXRMBCrystalChangeView *crystalChangeView = new SXRMBCrystalChangeView(sxrmbBl->crystalSelection());
	generalPaneWidget = AMMainWindow::buildMainWindowPane("Crystal Change", generalPaneIcon_, crystalChangeView);
	mw_->addPane(generalPaneWidget, generalPaneCategeryName_, "Crystal Change", generalPaneIcon_);

	AMSlitsView *jjSlitsView = new AMSlitsView(sxrmbBl->jjSlits());
	generalPaneWidget = AMMainWindow::buildMainWindowPane("Slit View", generalPaneIcon_, jjSlitsView);
	mw_->addPane(generalPaneWidget, generalPaneCategeryName_, "Slit View", generalPaneIcon_);
}

void SXRMBAppController::createDetectorPanes()
{
	SXRMBBeamline *sxrmbBl = SXRMBBeamline::sxrmb();

	SXRMBBrukerDetectorView *brukerView = new SXRMBBrukerDetectorView(sxrmbBl->brukerDetector());
	brukerView->buildDetectorView();
	brukerView->setEnergyRange(sxrmbBl->beamlineEnergyLowEnd(), sxrmbBl->beamlineEnergyHighEnd());
	brukerView->addEmissionLineNameFilter(QRegExp("1"));
	brukerView->addPileUpPeakNameFilter(QRegExp("(K.1|L.1|Ma1)"));
	brukerView->addCombinationPileUpPeakNameFilter(QRegExp("(Ka1|La1|Ma1)"));
	brukerView->enableDeadTimeDisplay();
	mw_->addPane(brukerView, detectorPaneCategoryName_, "Bruker", detectorPaneIcon_);

	SXRMBFourElementVortexDetectorView *fourElementVortexView = new SXRMBFourElementVortexDetectorView(sxrmbBl->fourElementVortexDetector());
	fourElementVortexView->buildDetectorView();
	fourElementVortexView->setEnergyRange(sxrmbBl->beamlineEnergyLowEnd(), sxrmbBl->beamlineEnergyHighEnd());
	fourElementVortexView->addEmissionLineNameFilter(QRegExp("1"));
	fourElementVortexView->addPileUpPeakNameFilter(QRegExp("(K.1|L.1|Ma1)"));
	fourElementVortexView->addCombinationPileUpPeakNameFilter(QRegExp("(Ka1|La1|Ma1)"));
	mw_->addPane(fourElementVortexView, detectorPaneCategoryName_, "4-el Vortex", detectorPaneIcon_);

	CLSSIS3820ScalerView *scalerView = new CLSSIS3820ScalerView(sxrmbBl->scaler());
	scalerView->setAmplifierViewFormat('g');
	scalerView->setAmplifierViewPrecision(3);
	QWidget * scalerPaneWidget = AMMainWindow::buildMainWindowPane("Scaler", detectorPaneIcon_, scalerView);
	mw_->addPane(scalerPaneWidget, detectorPaneCategoryName_, "Scaler", detectorPaneIcon_, true);
}

void SXRMBAppController::createScanConfigurationPanes()
{
	// create EXAFS scan configuration view and view holder
	exafsScanConfigurationView_ = new SXRMBEXAFSScanConfigurationView(exafsScanConfiguration_);
	exafsScanConfigurationViewHolder_ = new AMScanConfigurationViewHolder3("Configure an EXAFS Scan", true, true, exafsScanConfigurationView_);
	mw_->addPane(exafsScanConfigurationViewHolder_, scanPaneCategoryName_, "EXAFS Scan", scanPaneIcon_);

	// create 2D scan configuration view and view holder
	microProbe2DScanConfigurationView_ = new SXRMB2DMapScanConfigurationView(microProbe2DScanConfiguration_);
	microProbe2DScanConfigurationViewHolder_ = new AMScanConfigurationViewHolder3("2D Map Configuration", true, true, microProbe2DScanConfigurationView_);

	// create 2D Oxidation scan configuration view and view holder
	microProbe2DOxidationScanConfigurationView_ = new SXRMB2DOxidationMapScanConfigurationView(microProbe2DOxidationScanConfiguration_);
	microProbe2DOxidationScanConfigurationViewHolder_ = new SXRMBOxidationMapScanConfigurationViewHolder(microProbe2DOxidationScanConfigurationView_);

	// connection signal / slot for scan configuration and view holder
	connect(exafsScanConfiguration_, SIGNAL(totalTimeChanged(double)), exafsScanConfigurationViewHolder_, SLOT(updateOverallScanTime(double)));
	exafsScanConfigurationViewHolder_->updateOverallScanTime(exafsScanConfiguration_->totalTime());

	connect(microProbe2DScanConfiguration_, SIGNAL(totalTimeChanged(double)), microProbe2DScanConfigurationViewHolder_, SLOT(updateOverallScanTime(double)));
	microProbe2DScanConfigurationViewHolder_->updateOverallScanTime(microProbe2DScanConfiguration_->totalTime());

	connect(microProbe2DOxidationScanConfiguration_, SIGNAL(totalTimeChanged(double)), microProbe2DOxidationScanConfigurationViewHolder_, SLOT(updateOverallScanTime(double)));
	microProbe2DOxidationScanConfigurationViewHolder_->updateOverallScanTime(microProbe2DOxidationScanConfiguration_->totalTime());

	// try to update the current scan configuration views
	SXRMB::Endstation currentEndstation = SXRMBBeamline::sxrmb()->currentEndstation();
	onBeamlineEndstationSwitched(currentEndstation, currentEndstation);
}

void SXRMBAppController::onCurrentScanActionStartedImplementation(AMScanAction *action)
{
	CLSAppController::onCurrentScanActionStartedImplementation(action);

	// start to listen to the beamAvaliability signal for scan auto-pause purpose
	connect(SXRMBBeamline::sxrmb(), SIGNAL(beamAvaliability(bool)), this, SLOT(onBeamAvailabilityChanged(bool)));
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

	SXRMBUserConfiguration * sxrmbUserConfiguration = qobject_cast<SXRMBUserConfiguration *>(userConfiguration_);
	if (configDB){
		sxrmbUserConfiguration->setFluorescenceDetector(configDB->fluorescenceDetector());
		sxrmbUserConfiguration->storeToDb(AMDatabase::database("user"));
	}
}

void SXRMBAppController::onUserConfigurationLoadedFromDb()
{
	CLSAppController::onUserConfigurationLoadedFromDb();

	SXRMBUserConfiguration *sxrmbUserConfiguration = qobject_cast<SXRMBUserConfiguration *>(userConfiguration_);
	if (sxrmbUserConfiguration) {
		SXRMB::FluorescenceDetectors fluorescenceDetectors = sxrmbUserConfiguration->fluorescenceDetector();
		if (exafsScanConfiguration_)
			exafsScanConfiguration_->setFluorescenceDetector(fluorescenceDetectors);
		if (microProbe2DScanConfiguration_)
			microProbe2DScanConfiguration_->setFluorescenceDetector(fluorescenceDetectors);
		if (microProbe2DOxidationScanConfiguration_)
			microProbe2DOxidationScanConfiguration_->setFluorescenceDetector(fluorescenceDetectors);
	}
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

void SXRMBAppController::onScanEditorCreatedImplementation(AMGenericScanEditor *editor)
{
	editor->setEnergyRange(SXRMBBeamline::sxrmb()->beamlineEnergyLowEnd(), SXRMBBeamline::sxrmb()->beamlineEnergyHighEnd());

	connect(editor, SIGNAL(scanAdded(AMGenericScanEditor*,AMScan*)), this, SLOT(onScanAddedToEditor(AMGenericScanEditor*,AMScan*)));
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

	editor->setEnergyRange(SXRMBBeamline::sxrmb()->beamlineEnergyLowEnd(), SXRMBBeamline::sxrmb()->beamlineEnergyHighEnd());
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
