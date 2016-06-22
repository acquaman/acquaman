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


#include "VESPERSAppController.h"

#include "beamline/CLS/CLSStorageRing.h"
#include "beamline/VESPERS/VESPERSBeamline.h"

#include "dataman/AMLineScan.h"

#include "actions3/AMActionRunner3.h"
#include "actions3/AMListAction3.h"
#include "actions3/actions/AMScanAction.h"
#include "actions3/actions/AMWaitAction.h"
#include "acquaman/AMScanActionController.h"
#include "acquaman/VESPERS/VESPERSScanConfiguration.h"

#include "ui/AMMainWindow.h"
#include "ui/acquaman/AMScanConfigurationViewHolder3.h"
#include "ui/dataman/AMGenericScanEditor.h"
#include "ui/util/AMChooseDataFolderDialog.h"

#include "ui/VESPERS/VESPERSEndstationView.h"
#include "ui/VESPERS/VESPERSPersistentView.h"
#include "ui/VESPERS/VESPERSDeviceStatusView.h"
#include "ui/VESPERS/VESPERSEXAFSScanConfigurationView.h"
#include "ui/VESPERS/VESPERSCCDDetectorView.h"
#include "ui/VESPERS/VESPERSPilatusCCDDetectorView.h"
#include "ui/VESPERS/VESPERS2DScanConfigurationView.h"
#include "ui/VESPERS/VESPERSSpatialLineScanConfigurationView.h"
#include "ui/VESPERS/VESPERSEnergyScanConfigurationView.h"
#include "ui/VESPERS/VESPERS3DScanConfigurationView.h"

#include "application/AMAppControllerSupport.h"

#include "dataman/AMScanEditorModelItem.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/VESPERS/VESPERSDbUpgrade1Pt1.h"
#include "dataman/VESPERS/VESPERSDbUpgrade1Pt2.h"
#include "dataman/VESPERS/VESPERSDbUpgrade1Pt3.h"
#include "dataman/VESPERS/VESPERSDbUpgrade1Pt4.h"
#include "dataman/VESPERS/VESPERSDbUpgrade1Pt5.h"
#include "dataman/VESPERS/VESPERSDbUpgrade1Pt6.h"

#include "dataman/export/AMExportController.h"
#include "dataman/export/AMExporterOptionSMAK.h"
#include "dataman/export/AMExporterOptionGeneralAscii.h"
#include "dataman/export/AMExporterGeneralAscii.h"
#include "dataman/export/AMExporterAthena.h"
#include "dataman/export/VESPERS/VESPERSExporter3DAscii.h"
#include "dataman/export/VESPERS/VESPERSExporter2DAscii.h"
#include "dataman/export/VESPERS/VESPERSExporterSMAK.h"
#include "dataman/export/VESPERS/VESPERSExporterLineScanAscii.h"

#include "dataman/VESPERS/VESPERSUserConfiguration.h"
#include "actions3/VESPERS/VESPERSSetStringAction.h"

#include <QFileDialog>
#include <QMessageBox>

#include "dataman/AMRun.h"
#include "ui/VESPERS/VESPERSScalerView.h"

#include "beamline/VESPERS/VESPERSSingleElementVortexDetector.h"
#include "beamline/VESPERS/VESPERSFourElementVortexDetector.h"
#include "ui/beamline/AMXRFBaseDetectorView.h"
#include "ui/beamline/AMXRFDetailedDetectorView.h"
#include "ui/VESPERS/VESPERSSingleElementVortexDetectorView.h"
#include "ui/VESPERS/VESPERSFourElementVortexDetectorView.h"
#include "ui/VESPERS/VESPERS13ElementGeDetectorView.h"

#include "acquaman/VESPERS/VESPERSTimeScanConfiguration.h"
#include "ui/VESPERS/VESPERSTimeScanConfigurationView.h"
#include "acquaman/VESPERS/VESPERSTimedLineScanConfiguration.h"
#include "ui/VESPERS/VESPERSTimedLineScanConfigurationView.h"

VESPERSAppController::VESPERSAppController(QObject *parent) :
	CLSAppController("VESPERS", parent)
{
	moveImmediatelyAction_ = 0;

	roperCCDStartup_ = false;
	marCCDStartup_ = false;
	pilatusCCDStartup_ = false;

	setDefaultUseLocalStorage(true);

	userConfiguration_ = new VESPERSUserConfiguration(this);

	// Remember!!!!  Every upgrade needs to be done to the user AND actions databases!
	////////////////////////////////////////////////////////////////////////////////////////
	AMDbUpgrade *vespers1Pt1UserDb = new VESPERSDbUpgrade1Pt1("user", this);
	appendDatabaseUpgrade(vespers1Pt1UserDb);
	AMDbUpgrade *vespers1Pt1ActionDb = new VESPERSDbUpgrade1Pt1("actions", this);
	appendDatabaseUpgrade(vespers1Pt1ActionDb);
	AMDbUpgrade *vespers1P2UserDb = new VESPERSDbUpgrade1Pt2("user", this);
	appendDatabaseUpgrade(vespers1P2UserDb);
	AMDbUpgrade *vespers1P2ActionDb = new VESPERSDbUpgrade1Pt2("actions", this);
	appendDatabaseUpgrade(vespers1P2ActionDb);
	AMDbUpgrade *vespers1P3UserDb = new VESPERSDbUpgrade1Pt3("user", this);
	appendDatabaseUpgrade(vespers1P3UserDb);
	AMDbUpgrade *vespers1P3ActionDb = new VESPERSDbUpgrade1Pt3("actions", this);
	appendDatabaseUpgrade(vespers1P3ActionDb);
	AMDbUpgrade *vespers1P4UserDb = new VESPERSDbUpgrade1Pt4("user", this);
	appendDatabaseUpgrade(vespers1P4UserDb);
	AMDbUpgrade *vespers1P4ActionDb = new VESPERSDbUpgrade1Pt4("actions", this);
	appendDatabaseUpgrade(vespers1P4ActionDb);
	AMDbUpgrade *vespers1P5UserDb = new VESPERSDbUpgrade1Pt5("user", this);
	appendDatabaseUpgrade(vespers1P5UserDb);
	AMDbUpgrade *vespers1P5ActionDb = new VESPERSDbUpgrade1Pt5("actions", this);
	appendDatabaseUpgrade(vespers1P5ActionDb);
	AMDbUpgrade *vespers1P6UserDb = new VESPERSDbUpgrade1Pt6("user", this);
	appendDatabaseUpgrade(vespers1P6UserDb);
	AMDbUpgrade *vespers1P6ActionDb = new VESPERSDbUpgrade1Pt6("actions", this);
	appendDatabaseUpgrade(vespers1P6ActionDb);
}

bool VESPERSAppController::startup()
{
	// Start up the main program.
	if(CLSAppController::startup()) {

		if (!ensureProgramStructure())
			return false;

		// Github setup for adding VESPERS specific comment.
		additionalIssueTypesAndAssignees_.append("I think it's a VESPERS specific issue", "dretrex");

		return true;
	}
	else
		return false;
}

bool VESPERSAppController::ensureProgramStructure()
{
	if (!QFileInfo("/nas/vespers").exists()){

		AMErrorMon::error(this, VESPERSAPPCONTROLLER_AURORA_PATH_NOT_FOUND, "Path to aurora not found.  Notify beamline staff.");
		return false;
	}

	// This one is not critical to the running of the application.
	if (!QFileInfo("/nas/pilatus").exists())
		AMErrorMon::alert(this, VESPERSAPPCONTROLLER_PILATUS_PATH_NOT_FOUND, "The path to the Pilatus CCD was not found.  Many special features for the Pilatus are now unavailable.");

	return true;
}

bool VESPERSAppController::setupDataFolder()
{
	// Get a destination folder.
	return AMChooseDataFolderDialog::getDataFolder("/AcquamanLocalData/vespers",   //local directory
												   "/nas/vespers",                 //remote directory
												   "users",                        //data directory
												   QStringList() << "XRD Images"); //extra data directory
}

void VESPERSAppController::initializeBeamline()
{
	// Initialize central beamline object
	VESPERSBeamline* vespersBL = VESPERSBeamline::vespers();

	// Startup connections for the CCD detectors.
	connect(vespersBL->roperCCD(), SIGNAL(connected(bool)), this, SLOT(onRoperCCDConnected(bool)));
	connect(vespersBL->marCCD(), SIGNAL(connected(bool)), this, SLOT(onMarCCDConnected(bool)));
	connect(vespersBL->vespersPilatusAreaDetector(), SIGNAL(connected(bool)), this, SLOT(onPilatusCCDConnected(bool)));
}

void VESPERSAppController::registerDBClasses()
{
	AMDbObjectSupport::s()->registerClass<VESPERSEXAFSScanConfiguration>();
	AMDbObjectSupport::s()->registerClass<VESPERS2DScanConfiguration>();
	AMDbObjectSupport::s()->registerClass<VESPERSSpatialLineScanConfiguration>();
	AMDbObjectSupport::s()->registerClass<VESPERSEnergyScanConfiguration>();
	AMDbObjectSupport::s()->registerClass<AMLineScan>();
	AMDbObjectSupport::s()->registerClass<VESPERSScanConfigurationDbObject>();
	AMDbObjectSupport::s()->registerClass<VESPERS3DScanConfiguration>();
	AMDbObjectSupport::s()->registerClass<VESPERSUserConfiguration>();
	AMDbObjectSupport::s()->registerClass<VESPERSTimeScanConfiguration>();
	AMDbObjectSupport::s()->registerClass<VESPERSSetStringActionInfo>();
	AMDbObjectSupport::s()->registerClass<VESPERSTimedLineScanConfiguration>();

	AMOldDetectorViewSupport::registerClass<VESPERSCCDDetectorView, VESPERSRoperCCDDetector>();
	AMOldDetectorViewSupport::registerClass<VESPERSCCDDetectorView, VESPERSMarCCDDetector>();
	AMOldDetectorViewSupport::registerClass<VESPERSPilatusCCDDetectorView, VESPERSPilatusCCDDetector>();

	AMExportController::unregisterExporter<AMSMAKExporter>();
	AMExportController::unregisterExporter<AMExporter2DAscii>();

	AMExportController::registerExporter<VESPERSExporter3DAscii>();
	AMExportController::registerExporter<VESPERSExporter2DAscii>();
	AMExportController::registerExporter<VESPERSExporterSMAK>();
	AMExportController::registerExporter<VESPERSExporterLineScanAscii>();
}

void VESPERSAppController::registerExporterOptions()
{
	AMExporterOptionGeneralAscii *vespersDefault = VESPERS::buildStandardExporterOption("VESPERSDefault", true, true, true, true);
	if(vespersDefault->id() > 0)
		AMAppControllerSupport::registerClass<VESPERSEXAFSScanConfiguration, AMExporterAthena, AMExporterOptionGeneralAscii>(vespersDefault->id());

	vespersDefault = VESPERS::buildStandardExporterOption("VESPERS2DDefault", true, false, false, true);
	if(vespersDefault->id() > 0)
		AMAppControllerSupport::registerClass<VESPERS2DScanConfiguration, VESPERSExporter2DAscii, AMExporterOptionGeneralAscii>(vespersDefault->id());

	vespersDefault = VESPERS::buildStandardExporterOption("VESPERS3DDefault", true, false, false, true);
	if(vespersDefault->id() > 0)
		AMAppControllerSupport::registerClass<VESPERS3DScanConfiguration, VESPERSExporter3DAscii, AMExporterOptionGeneralAscii>(vespersDefault->id());

	vespersDefault = VESPERS::buildStandardExporterOption("VESPERSLineScanDefault", true, false, false, true);
	if(vespersDefault->id() > 0)
		AMAppControllerSupport::registerClass<VESPERSSpatialLineScanConfiguration, VESPERSExporterLineScanAscii, AMExporterOptionGeneralAscii>(vespersDefault->id());

	vespersDefault = VESPERS::buildStandardExporterOption("VESPERSEnergyScanDefault", true, true, true, true);
	if(vespersDefault->id() > 0)
		AMAppControllerSupport::registerClass<VESPERSEnergyScanConfiguration, VESPERSExporterLineScanAscii, AMExporterOptionGeneralAscii>(vespersDefault->id());

	vespersDefault = VESPERS::buildStandardExporterOption("VESPERSTimeScanDefault", true, false, false, true);
	if(vespersDefault->id() > 0)
		AMAppControllerSupport::registerClass<VESPERSTimeScanConfiguration, VESPERSExporterLineScanAscii, AMExporterOptionGeneralAscii>(vespersDefault->id());

	vespersDefault = VESPERS::buildStandardExporterOption("VESPERSTimeLineScanDefault", true, false, false, true);
	if(vespersDefault->id() > 0)
		AMAppControllerSupport::registerClass<VESPERSTimedLineScanConfiguration, VESPERSExporter2DAscii, AMExporterOptionGeneralAscii>(vespersDefault->id());

	VESPERS::buildSMAKExporterOption("VESPERS2DSMAKDefault", true, false, false, true);
//	AMExporterOptionSMAK *vespersSMAKDefault = VESPERS::buildSMAKExporterOption("VESPERS2DSMAKDefault", true, false, false, true);
//	if(vespersSMAKDefault->id() > 0)
//		AMAppControllerSupport::registerClass<VESPERS2DScanConfiguration, VESPERSExporterSMAK, AMExporterOptionSMAK>(vespersSMAKDefault->id());
}

void VESPERSAppController::setupScanConfigurations()
{
	// Setup XAS for the beamline.  Builds the config
	exafsScanConfiguration_ = new VESPERSEXAFSScanConfiguration();

	// Setup 2D maps for the beamline.  Builds the config
	mapScanConfiguration_ = new VESPERS2DScanConfiguration();
	mapScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStart(0.0);
	mapScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStep(0.005);
	mapScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionEnd(1.0);
	mapScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionTime(1.0);
	mapScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionStart(0.0);
	mapScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionStep(0.005);
	mapScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionEnd(1.0);
	mapScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionTime(1.0);

	// Setup 2D maps for the beamline.  Builds the config
	map3DScanConfiguration_ = new VESPERS3DScanConfiguration();
	map3DScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStart(0.0);
	map3DScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStep(0.005);
	map3DScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionEnd(1.0);
	map3DScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionTime(1.0);
	map3DScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionStart(0.0);
	map3DScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionStep(0.005);
	map3DScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionEnd(1.0);
	map3DScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionTime(1.0);
	map3DScanConfiguration_->scanAxisAt(2)->regionAt(0)->setRegionStart(0.0);
	map3DScanConfiguration_->scanAxisAt(2)->regionAt(0)->setRegionStep(0.005);
	map3DScanConfiguration_->scanAxisAt(2)->regionAt(0)->setRegionEnd(1.0);
	map3DScanConfiguration_->scanAxisAt(2)->regionAt(0)->setRegionTime(1.0);

	// Setup line scans for the beamline.  Builds the config
	lineScanConfiguration_ = new VESPERSSpatialLineScanConfiguration();
	lineScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStart(0.0);
	lineScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStep(0.005);
	lineScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionEnd(1.0);
	lineScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionTime(1.0);

	// Setup energy scans for the beamline.  Builds the config
	energyScanConfiguration_ = new VESPERSEnergyScanConfiguration();
	energyScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStart(10000);
	energyScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStep(1000);
	energyScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionEnd(20000);
	energyScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionTime(1);

	// Setup time scan configuration
	timeScanConfiguration_ = new VESPERSTimeScanConfiguration;

	// Setup time line scan configuration
	timedLineScanConfiguration_ = new VESPERSTimedLineScanConfiguration;
	timedLineScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStart(0.0);
	timedLineScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStep(0.005);
	timedLineScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionEnd(1.0);
	timedLineScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionTime(1.0);
}

void VESPERSAppController::createPersistentView()
{
	// This is the right hand panel that is always visible.  Has important information such as shutter status and overall controls status.  Also controls the sample stage.
	persistentView_ = new VESPERSPersistentView;
	mw_->addRightWidget(persistentView_);
	// It is sufficient to only connect the user configuration to the single element because the single element and four element are synchronized together.
	connect(persistentView_, SIGNAL(currentSampleStageChanged(QString)), this, SLOT(onSampleStageChoiceChanged(QString)));
	connect(persistentView_, SIGNAL(statusButtonClicked(QString)), statusPage_, SLOT(showDiagnosticsPage(QString)));
	connect(persistentView_, SIGNAL(statusButtonClicked(QString)), this, SLOT(onStatusViewRequrested()));
}

void VESPERSAppController::createGeneralPanes()
{
	// Setup the general endstation control view.
	endstationView_ = new VESPERSEndstationView(VESPERSBeamline::vespers()->endstation());
	mw_->addPane(endstationView_, generalPaneCategeryName_, "Endstation", generalPaneIcon_);

	// Setup the general status page.
	statusPage_ = new VESPERSDeviceStatusView;
	mw_->addPane(statusPage_, generalPaneCategeryName_, "Device Status", generalPaneIcon_);

	// Show the endstation control view first.
	mw_->setCurrentPane(endstationView_);
}

void VESPERSAppController::createDetectorPanes()
{
	/*
	roperCCDView_ = new VESPERSCCDDetectorView(VESPERSBeamline::vespers()->roperCCD());
	mw_->addPane(roperCCDView_, detectorPaneCategoryName_, "Area - Roper", detectorsPaneIcon_);

	marCCDView_ = new VESPERSCCDDetectorView(VESPERSBeamline::vespers()->marCCD());
	mw_->addPane(marCCDView_, detectorPaneCategoryName_, "Area - Mar", detectorsPaneIcon_);
	*/

	VESPERSSingleElementVortexDetectorView *singleElementVortexView = new VESPERSSingleElementVortexDetectorView(VESPERSBeamline::vespers()->vespersSingleElementVortexDetector());
	singleElementVortexView->buildDetectorView();
	singleElementVortexView->setEnergyRange(2000, 20480);
	singleElementVortexView->addEmissionLineNameFilter(QRegExp("1"));
	singleElementVortexView->addPileUpPeakNameFilter(QRegExp("(K.1|L.1|Ma1)"));
	singleElementVortexView->addCombinationPileUpPeakNameFilter(QRegExp("(Ka1|La1|Ma1)"));
	mw_->addPane(singleElementVortexView, detectorPaneCategoryName_, "1-el Vortex", detectorPaneIcon_);

	VESPERSFourElementVortexDetectorView *fourElementVortexView = new VESPERSFourElementVortexDetectorView(VESPERSBeamline::vespers()->vespersFourElementVortexDetector());
	fourElementVortexView->buildDetectorView();
	fourElementVortexView->setEnergyRange(2000, 20480);
	fourElementVortexView->addEmissionLineNameFilter(QRegExp("1"));
	fourElementVortexView->addPileUpPeakNameFilter(QRegExp("(K.1|L.1|Ma1)"));
	fourElementVortexView->addCombinationPileUpPeakNameFilter(QRegExp("(Ka1|La1|Ma1)"));
	mw_->addPane(fourElementVortexView, detectorPaneCategoryName_, "4-el Vortex", detectorPaneIcon_);

	VESPERS13ElementGeDetectorView *ge13ElementDetectorView = new VESPERS13ElementGeDetectorView(VESPERSBeamline::vespers()->vespersGe13ElementDetector());
	ge13ElementDetectorView->buildDetectorView();
	ge13ElementDetectorView->setEnergyRange(2000, 20480);
	ge13ElementDetectorView->addEmissionLineNameFilter(QRegExp("1"));
	ge13ElementDetectorView->addPileUpPeakNameFilter(QRegExp("(K.1|L.1|Ma1)"));
	ge13ElementDetectorView->addCombinationPileUpPeakNameFilter(QRegExp("(Ka1|La1|Ma1)"));
	mw_->addPane(ge13ElementDetectorView, detectorPaneCategoryName_, "13-el Ge", detectorPaneIcon_);

	pilatusView_ = new VESPERSPilatusCCDDetectorView(VESPERSBeamline::vespers()->vespersPilatusAreaDetector());
	mw_->addPane(pilatusView_, detectorPaneCategoryName_, "Area - Pilatus", detectorPaneIcon_);
}

void VESPERSAppController::createScanConfigurationPanes()
{
	// Setup XAS for the beamline.  Builds the view, and view holder.
	exafsConfigurationView_ = new VESPERSEXAFSScanConfigurationView(exafsScanConfiguration_);
	exafsConfigurationView_->setupDefaultXANESScanRegions();
	exafsConfigurationViewHolder3_ = new AMScanConfigurationViewHolder3(exafsConfigurationView_, true);
	mw_->addPane(exafsConfigurationViewHolder3_, scanPaneCategoryName_, "XAS", scanPaneIcon_);

	// Setup 2D maps for the beamline.  Builds the view, and view holder.
	mapScanConfigurationView_ = new VESPERS2DScanConfigurationView(mapScanConfiguration_);
	mapScanConfigurationViewHolder3_ = new AMScanConfigurationViewHolder3(mapScanConfigurationView_);
	mw_->addPane(mapScanConfigurationViewHolder3_, scanPaneCategoryName_, "2D Maps", scanPaneIcon_);

	// Setup 3D maps for the beamline.  Builds the view, and view holder.
	map3DScanConfigurationView_ = new VESPERS3DScanConfigurationView(map3DScanConfiguration_);
	map3DScanConfigurationViewHolder3_ = new AMScanConfigurationViewHolder3(map3DScanConfigurationView_);
	mw_->addPane(map3DScanConfigurationViewHolder3_, scanPaneCategoryName_, "3D Maps", scanPaneIcon_);

	// Setup line scans for the beamline.  Builds the view, and view holder.
	lineScanConfigurationView_ = new VESPERSSpatialLineScanConfigurationView(lineScanConfiguration_);
	lineScanConfigurationViewHolder3_ = new AMScanConfigurationViewHolder3(lineScanConfigurationView_);
	mw_->addPane(lineScanConfigurationViewHolder3_, scanPaneCategoryName_, "Line Scan", scanPaneIcon_);

	// Setup energy scans for the beamline.  Builds the view, and view holder.
	energyScanConfigurationView_ = new VESPERSEnergyScanConfigurationView(energyScanConfiguration_);
	energyScanConfigurationViewHolder3_ = new AMScanConfigurationViewHolder3(energyScanConfigurationView_);
	mw_->addPane(energyScanConfigurationViewHolder3_, scanPaneCategoryName_, "XRD Energy Scan", scanPaneIcon_);

	// Setup time scans for the beamline.  Builds the view, and view holder.
	timeScanConfigurationView_ = new VESPERSTimeScanConfigurationView(timeScanConfiguration_);
	timeScanConfigurationViewHolder3_ = new AMScanConfigurationViewHolder3(timeScanConfigurationView_);
	mw_->addPane(timeScanConfigurationViewHolder3_, scanPaneCategoryName_, "Timed Scan", scanPaneIcon_);

	// Setup time line scans for the beamline.  Builds the view, and view holder.
	timedLineScanConfigurationView_ = new VESPERSTimedLineScanConfigurationView(timedLineScanConfiguration_);
	timedLineScanConfigurationViewHolder3_ = new AMScanConfigurationViewHolder3(timedLineScanConfigurationView_);
	mw_->addPane(timedLineScanConfigurationViewHolder3_, scanPaneCategoryName_, "Timed Line Scan", scanPaneIcon_);

	// setup the signal/slot connections for the views
	connect(exafsConfigurationView_, SIGNAL(configureDetector(QString)), this, SLOT(onConfigureDetectorRequested(QString)));
	connect(mapScanConfigurationView_, SIGNAL(configureDetector(QString)), this, SLOT(onConfigureDetectorRequested(QString)));
	connect(map3DScanConfigurationView_, SIGNAL(configureDetector(QString)), this, SLOT(onConfigureDetectorRequested(QString)));
	connect(lineScanConfigurationView_, SIGNAL(configureDetector(QString)), this, SLOT(onConfigureDetectorRequested(QString)));
	connect(energyScanConfigurationView_, SIGNAL(configureDetector(QString)), this, SLOT(onConfigureDetectorRequested(QString)));

	connect(exafsScanConfiguration_, SIGNAL(totalTimeChanged(double)), exafsConfigurationViewHolder3_, SLOT(updateOverallScanTime(double)));
	exafsConfigurationViewHolder3_->updateOverallScanTime(exafsScanConfiguration_->totalTime());
}

void VESPERSAppController::onStatusViewRequrested()
{
	mw_->setCurrentPane(statusPage_);
}

void VESPERSAppController::onConfigureDetectorRequested(const QString &detector)
{
	if (detector == "Roper CCD")
		mw_->setCurrentPane(roperCCDView_);
	else if (detector == "Mar CCD")
		mw_->setCurrentPane(marCCDView_);
	else if (detector == "Pilatus")
		mw_->setCurrentPane(pilatusView_);
}

void VESPERSAppController::onCurrentScanActionStartedImplementation(AMScanAction *action)
{
	QString fileFormat(action->controller()->scan()->fileFormat());

	if (fileFormat == "vespersXRF" || fileFormat == "vespers2011XRF")
		return;

	CLSAppController::onCurrentScanActionStartedImplementation(action);
}

void VESPERSAppController::onCurrentScanActionFinishedImplementation(AMScanAction *action)
{
	QString fileFormat(action->controller()->scan()->fileFormat());

	if (fileFormat == "vespersXRF" || fileFormat == "vespers2011XRF")
		return;

	// Save the current configuration to the database.
	// Being explicit due to the nature of how many casts were necessary.  I could probably explicitly check to ensure each cast is successful, but I'll risk it for now.
	const AMScanActionInfo *actionInfo = qobject_cast<const AMScanActionInfo *>(action->info());
	const VESPERSScanConfiguration *vespersConfig = dynamic_cast<const VESPERSScanConfiguration *>(actionInfo->configuration());
	VESPERSScanConfigurationDbObject *config = qobject_cast<VESPERSScanConfigurationDbObject *>(vespersConfig->dbObject());

	if (config){
		VESPERSUserConfiguration *vespersUserConfiguration = qobject_cast<VESPERSUserConfiguration *>(userConfiguration_);

		vespersUserConfiguration->setIncomingChoice(config->incomingChoice());
		vespersUserConfiguration->setTransmissionChoice(config->transmissionChoice());
		vespersUserConfiguration->setFluorescenceDetector(config->fluorescenceDetector());
		vespersUserConfiguration->setCCDDetector(config->ccdDetector());
		vespersUserConfiguration->setMotor(config->motor());
	}

	// perfrom the general actions when scan action finished
	CLSAppController::onCurrentScanActionFinishedImplementation(action);
}

void VESPERSAppController::onBeamAvailabilityChanged(bool beamAvailable)
{
	if (!beamAvailable && !AMActionRunner3::workflow()->pauseCurrentAction())
		AMActionRunner3::workflow()->setQueuePaused(true);

	// On VESPERS, we don't like having the scan restart on it's own.
	else if (beamAvailable && AMActionRunner3::workflow()->queuedActionCount() > 0)
		AMActionRunner3::workflow()->setQueuePaused(false);
}

void VESPERSAppController::onScanEditorCreatedImplementation(AMGenericScanEditor *editor)
{
	editor->setEnergyRange(AMPeriodicTable::table()->elementBySymbol("K")->Kalpha().energy(), 20480);

	connect(editor, SIGNAL(scanAdded(AMGenericScanEditor*,AMScan*)), this, SLOT(onScanAddedToEditor(AMGenericScanEditor*,AMScan*)));
	if (editor->using2DScanView())
		connect(editor, SIGNAL(dataPositionChanged(AMGenericScanEditor*,QPoint)), this, SLOT(onDataPositionChanged(AMGenericScanEditor*,QPoint)));
}

void VESPERSAppController::onScanAddedToEditor(AMGenericScanEditor *editor, AMScan *scan)
{
	QString exclusiveName = QString();

	if (scan->scanRank() < 3){

		for (int i = 0, count = scan->analyzedDataSourceCount(); i < count && exclusiveName.isNull(); i++){

			AMDataSource *source = scan->analyzedDataSources()->at(i);

			if (source->name().contains("norm_") && !source->name().contains("norm_PFY") && !source->hiddenFromUsers())
				exclusiveName = source->name();
		}
	}

	else {

		for (int i = 0, count = scan->analyzedDataSourceCount(); i < count && exclusiveName.isNull(); i++){

			AMDataSource *source = scan->analyzedDataSources()->at(i);

			if (source->name().contains("reduced_") && !source->hiddenFromUsers())
				exclusiveName = source->name();
		}
	}

	if (!exclusiveName.isNull())
		editor->setExclusiveDataSourceByName(exclusiveName);

	else if (editor->scanAt(0)->analyzedDataSourceCount())
		editor->setExclusiveDataSourceByName(editor->scanAt(0)->analyzedDataSources()->at(editor->scanAt(0)->analyzedDataSourceCount()-1)->name());

	configureSingleSpectrumView(editor, scan);
}

void VESPERSAppController::configureSingleSpectrumView(AMGenericScanEditor *editor, AMScan *scan)
{
	int scanRank = scan->scanRank();
	QStringList spectraNames;

	for (int i = 0, size = scan->dataSourceCount(); i < size; i++)
		if (scan->dataSourceAt(i)->rank()-scanRank == 1)
			spectraNames << scan->dataSourceAt(i)->name();

	if (spectraNames.contains("sumSpectra-1eland4el"))
		editor->setSingleSpectrumViewDataSourceName("correctedRawSpectra-1el");

	else if (spectraNames.contains("correctedSum-4el"))
		editor->setSingleSpectrumViewDataSourceName("correctedSum-4el");

	else if (spectraNames.contains("correctedRawSpectra-1el"))
		editor->setSingleSpectrumViewDataSourceName("correctedRawSpectra-1el");

	else if (!spectraNames.isEmpty())
		editor->setSingleSpectrumViewDataSourceName(spectraNames.first());

	editor->setEnergyRange(AMPeriodicTable::table()->elementBySymbol("K")->Kalpha().energy(), 20480);
	editor->addSingleSpectrumEmissionLineNameFilter(QRegExp("1"));
	editor->addSingleSpectrumPileUpPeakNameFilter(QRegExp("(K.1|L.1|Ma1)"));
	editor->addSingleSpectrumCombinationPileUpPeakNameFilter(QRegExp("(Ka1|La1|Ma1)"));
}

void VESPERSAppController::onDataPositionChanged(AMGenericScanEditor *editor, const QPoint &pos)
{
	// This should always succeed because the only way to get into this function is using the 2D scan view which currently only is accessed by 2D scans.
	VESPERS2DScanConfiguration *config = qobject_cast<VESPERS2DScanConfiguration *>(editor->currentScan()->scanConfiguration());

	if (!config)
		return;

	QString text;

	if (config->normalPosition() != 888888.88)
		text = QString("Setup at (H,V,N): (%1 mm, %2 mm, %3 mm)")
				.arg(editor->dataPosition().x(), 0, 'f', 3)
				.arg(editor->dataPosition().y(), 0, 'f', 3)
				.arg(config->normalPosition());

	else
		text = QString("Setup at (H,V): (%1 mm, %2 mm)")
				.arg(editor->dataPosition().x(), 0, 'f', 3)
				.arg(editor->dataPosition().y(), 0, 'f', 3);

	QMenu popup(text, editor);
	QAction *temp = popup.addAction(text);
	popup.addSeparator();
	popup.addAction("XANES scan");
	popup.addAction("EXAFS scan");
	popup.addAction("Energy scan");
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

		else if (temp->text() == "Energy scan")
			setupEnergyScan(editor);

		else if (temp->text() == "Go to immediately")
			moveImmediately(editor);

		else if (temp->text() == "2D XRF Scan")
			setup2DXRFScan(editor);
	}
}

void VESPERSAppController::moveImmediately(const AMGenericScanEditor *editor)
{
	// This should always succeed because the only way to get into this function is using the 2D scan view which currently only is accessed by 2D scans.
	VESPERS2DScanConfiguration *config = qobject_cast<VESPERS2DScanConfiguration *>(editor->currentScan()->scanConfiguration());

	if (!config)
		return;

	if (config->motor() == (VESPERS::H | VESPERS::V)){

		moveImmediatelyAction_ = new AMListAction3(new AMListActionInfo3("Move immediately", "Moves sample stage to given coordinates."), AMListAction3::Sequential);
		moveImmediatelyAction_->addSubAction(VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->horizontalAxis()->createTranslateMoveAction(editor->dataPosition().x()));
		moveImmediatelyAction_->addSubAction(VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->verticalAxis()->createTranslateMoveAction(editor->dataPosition().y()));

		if (config->normalPosition() != 888888.88){

			moveImmediatelyAction_->addSubAction(new AMWaitAction(new AMWaitActionInfo(0.01)));
			moveImmediatelyAction_->addSubAction(VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->normalAxis()->createTranslateMoveAction(config->normalPosition()));
		}

		connect(moveImmediatelyAction_, SIGNAL(succeeded()), this, SLOT(onMoveImmediatelySuccess()));
		connect(moveImmediatelyAction_, SIGNAL(failed()), this, SLOT(onMoveImmediatelyFailure()));
		moveImmediatelyAction_->start();
	}

	else if (config->motor() == (VESPERS::X | VESPERS::Z)){

		moveImmediatelyAction_ = new AMListAction3(new AMListActionInfo3("Move immediately", "Moves sample stage to given coordinates."), AMListAction3::Sequential);
		moveImmediatelyAction_->addSubAction(VESPERSBeamline::vespers()->realSampleStageMotorGroupObject()->horizontalAxis()->createTranslateMoveAction(editor->dataPosition().x()));
		moveImmediatelyAction_->addSubAction(VESPERSBeamline::vespers()->realSampleStageMotorGroupObject()->verticalAxis()->createTranslateMoveAction(editor->dataPosition().y()));

		if (config->normalPosition() != 888888.88){

			moveImmediatelyAction_->addSubAction(new AMWaitAction(new AMWaitActionInfo(0.01)));
			moveImmediatelyAction_->addSubAction(VESPERSBeamline::vespers()->realAttocubeStageMotorGroupObject()->normalAxis()->createTranslateMoveAction(config->normalPosition()));
		}

		connect(moveImmediatelyAction_, SIGNAL(succeeded()), this, SLOT(onMoveImmediatelySuccess()));
		connect(moveImmediatelyAction_, SIGNAL(failed()), this, SLOT(onMoveImmediatelyFailure()));
		moveImmediatelyAction_->start();
	}

	else if (config->motor() == (VESPERS::BigBeamX | VESPERS::BigBeamZ)){

		moveImmediatelyAction_ = new AMListAction3(new AMListActionInfo3("Move immediately", "Moves sample stage to given coordinates."), AMListAction3::Sequential);
		moveImmediatelyAction_->addSubAction(VESPERSBeamline::vespers()->bigBeamMotorGroupObject()->horizontalAxis()->createTranslateMoveAction(editor->dataPosition().x()));
		moveImmediatelyAction_->addSubAction(VESPERSBeamline::vespers()->bigBeamMotorGroupObject()->verticalAxis()->createTranslateMoveAction(editor->dataPosition().y()));

		connect(moveImmediatelyAction_, SIGNAL(succeeded()), this, SLOT(onMoveImmediatelySuccess()));
		connect(moveImmediatelyAction_, SIGNAL(failed(int)), this, SLOT(onMoveImmediatelyFailure()));
		moveImmediatelyAction_->start();
	}
}

void VESPERSAppController::onMoveImmediatelySuccess()
{
	cleanMoveImmediatelyAction();
}

void VESPERSAppController::onMoveImmediatelyFailure()
{
	cleanMoveImmediatelyAction();
	QMessageBox::warning(mw_, "Sample Stage Error", "The sample stage was unable to complete the desired movement.");
}

void VESPERSAppController::cleanMoveImmediatelyAction()
{
	if (moveImmediatelyAction_ == 0)
		return;

	// Disconnect all signals and return all memory.
	moveImmediatelyAction_->disconnect();
	moveImmediatelyAction_->deleteLater();
	moveImmediatelyAction_ = 0;
}

void VESPERSAppController::setupXASScan(const AMGenericScanEditor *editor, bool setupEXAFS)
{
	exafsScanConfiguration_->setGoToPosition(true);
	exafsScanConfiguration_->setX(editor->dataPosition().x());
	exafsScanConfiguration_->setY(editor->dataPosition().y());

	QString edge = editor->exclusiveDataSourceName();
	edge = edge.remove("norm_");
	edge.chop(2);
	edge.insert(edge.size()-1, " ");

	if (edge.at(edge.size()-1) == 'L')
		edge.append("1");

	exafsScanConfiguration_->setEdge(edge);

	// This should always succeed because the only way to get into this function is using the 2D scan view which currently only is accessed by 2D scans.
	VESPERS2DScanConfiguration *configuration = qobject_cast<VESPERS2DScanConfiguration *>(editor->currentScan()->scanConfiguration());
	if (configuration){

		exafsScanConfiguration_->setName(configuration->name());
		exafsScanConfiguration_->setFluorescenceDetector(configuration->fluorescenceDetector());
		exafsScanConfiguration_->setIncomingChoice(configuration->incomingChoice());
	}

	if (setupEXAFS)
		exafsConfigurationView_->setupDefaultEXAFSScanRegions();

	else
		exafsConfigurationView_->setupDefaultXANESScanRegions();

	mw_->undock(exafsConfigurationViewHolder3_);
}

void VESPERSAppController::setupEnergyScan(const AMGenericScanEditor *editor)
{
	energyScanConfiguration_->setGoToPosition(true);
	energyScanConfiguration_->setX(editor->dataPosition().x());
	energyScanConfiguration_->setY(editor->dataPosition().y());

	// This should always succeed because the only way to get into this function is using the 2D scan view which currently only is accessed by 2D scans.
	VESPERS2DScanConfiguration *config = qobject_cast<VESPERS2DScanConfiguration *>(editor->currentScan()->scanConfiguration());
	if (config)
		energyScanConfiguration_->setName(config->name());

	mw_->undock(energyScanConfigurationViewHolder3_);
}

void VESPERSAppController::setup2DXRFScan(const AMGenericScanEditor *editor)
{
	QRectF mapRect = editor->selectedRect();

	// This should always succeed because the only way to get into this function is using the 2D scan view which currently only is accessed by 2D scans.
	VESPERS2DScanConfiguration *config = qobject_cast<VESPERS2DScanConfiguration *>(editor->currentScan()->scanConfiguration());

	if (config){

		mapScanConfiguration_->setName(config->name());
		mapScanConfiguration_->setFluorescenceDetector(config->fluorescenceDetector());
		mapScanConfiguration_->setIncomingChoice(config->incomingChoice());
		mapScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStart(mapRect.left());
		mapScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionStep(0.005);
		mapScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionEnd(mapRect.right());
		mapScanConfiguration_->scanAxisAt(0)->regionAt(0)->setRegionTime(config->scanAxisAt(0)->regionAt(0)->regionTime());
		mapScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionStart(mapRect.bottom());
		mapScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionStep(0.005);
		mapScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionEnd(mapRect.top());
		mapScanConfiguration_->scanAxisAt(1)->regionAt(0)->setRegionTime(config->scanAxisAt(1)->regionAt(0)->regionTime());
		mapScanConfiguration_->setMotor(config->motor());
		mapScanConfiguration_->setCCDDetector(config->ccdDetector());
		mapScanConfiguration_->setNormalPosition(config->normalPosition());
		mapScanConfigurationView_->updateMapInfo();
	}

	mw_->undock(mapScanConfigurationViewHolder3_);
}

void VESPERSAppController::onSampleStageChoiceChanged(const QString &name)
{
	int motor = VESPERS::sampleStageNameToEnum(name);

	if (exafsMotorAcceptable(motor))
		exafsScanConfiguration_->setMotor(motor);

	if (energyScanMotorAcceptable(motor))
		energyScanConfiguration_->setMotor(motor);

	if (mapMotorAcceptable(motor))
		mapScanConfiguration_->setMotor(motor);

	if (lineScanMotorAcceptable(motor))
		lineScanConfiguration_->setMotor(convertSampleStageMotorToIndividualMotor(motor));
}

bool VESPERSAppController::exafsMotorAcceptable(int motor) const
{
	return (motor == (VESPERS::H | VESPERS::V)
			|| motor == (VESPERS::X | VESPERS::Z)
			|| motor == (VESPERS::AttoH | VESPERS::AttoV)
			|| motor == (VESPERS::AttoX | VESPERS::AttoZ)
			|| motor == (VESPERS::BigBeamX | VESPERS::BigBeamZ));
}

bool VESPERSAppController::energyScanMotorAcceptable(int motor) const
{
	return (motor == (VESPERS::H | VESPERS::V)
			|| motor == (VESPERS::X | VESPERS::Z)
			|| motor == (VESPERS::AttoH | VESPERS::AttoV)
			|| motor == (VESPERS::AttoX | VESPERS::AttoZ));
}

bool VESPERSAppController::mapMotorAcceptable(int motor) const
{
	return (motor == (VESPERS::H | VESPERS::V)
			|| motor == (VESPERS::X | VESPERS::Z)
			|| motor == (VESPERS::AttoH | VESPERS::AttoV)
			|| motor == (VESPERS::AttoX | VESPERS::AttoZ)
			|| motor == (VESPERS::BigBeamX | VESPERS::BigBeamZ)
			|| motor == (VESPERS::WireH | VESPERS::WireV));
}

bool VESPERSAppController::lineScanMotorAcceptable(int motor) const
{
	return (motor == (VESPERS::H | VESPERS::V)
			|| motor == (VESPERS::X | VESPERS::Z)
			|| motor == (VESPERS::AttoH | VESPERS::AttoV)
			|| motor == (VESPERS::AttoX | VESPERS::AttoZ)
			|| motor == VESPERS::AttoRx
			|| motor == VESPERS::AttoRy
			|| motor == VESPERS::AttoRz
			|| motor == (VESPERS::BigBeamX | VESPERS::BigBeamZ)
			|| motor == (VESPERS::WireH | VESPERS::WireV));
}

int VESPERSAppController::convertSampleStageMotorToIndividualMotor(int motor) const
{
	if ((motor & VESPERS::H) == VESPERS::H)
		return VESPERS::H;

	if ((motor & VESPERS::X) == VESPERS::X)
		return VESPERS::X;

	if ((motor & VESPERS::AttoH) == VESPERS::AttoH)
		return VESPERS::AttoH;

	if ((motor & VESPERS::AttoX) == VESPERS::AttoX)
		return VESPERS::AttoX;

	if (motor == VESPERS::AttoRx || motor == VESPERS::AttoRy || motor == VESPERS::AttoRz)
		return motor;

	if ((motor & VESPERS::BigBeamX) == VESPERS::BigBeamX)
		return VESPERS::BigBeamX;

	if ((motor & VESPERS::WireH) == VESPERS::WireH)
		return VESPERS::WireV;

	// A default that won't cause crashes.
	return VESPERS::H;
}

void VESPERSAppController::onRoperCCDConnected(bool connected)
{
	if (!roperCCDStartup_ && connected){

		roperCCDStartup_ = true;
		QString proposalNumber = VESPERS::getProposalNumber(AMUserSettings::userDataFolder);

		if (!proposalNumber.isEmpty())
			VESPERSBeamline::vespers()->vespersRoperCCD()->setCCDFilePath(QString("/nas/vespers/%1/CCDImages").arg(proposalNumber));
	}
}

void VESPERSAppController::onMarCCDConnected(bool connected)
{
	if (!marCCDStartup_ && connected){

		marCCDStartup_ = true;
		QString proposalNumber = VESPERS::getProposalNumber(AMUserSettings::userDataFolder);

		if (!proposalNumber.isEmpty())
			VESPERSBeamline::vespers()->vespersMarCCD()->setCCDFilePath(QString("/home/vespers/users/%1/").arg(proposalNumber));
	}
}

void VESPERSAppController::onPilatusCCDConnected(bool connected)
{
	if (!pilatusCCDStartup_ && connected){

		pilatusCCDStartup_ = true;
		QString proposalNumber = VESPERS::getProposalNumber(AMUserSettings::userDataFolder);

		if (!proposalNumber.isEmpty())
			VESPERSBeamline::vespers()->vespersPilatusAreaDetector()->setCCDFilePath(QString("/ramdisk/%1/").arg(proposalNumber));
	}
}

void VESPERSAppController::onUserConfigurationLoadedFromDb()
{
	CLSAppController::onUserConfigurationLoadedFromDb();

	VESPERSUserConfiguration *vespersUserConfiguration = qobject_cast<VESPERSUserConfiguration *>(userConfiguration_);

	QList<VESPERSScanConfiguration *> configurations = QList<VESPERSScanConfiguration *>()
			<< exafsScanConfiguration_
			<< mapScanConfiguration_
			<< lineScanConfiguration_
			<< energyScanConfiguration_
			<< map3DScanConfiguration_
			<< timeScanConfiguration_
			<< timedLineScanConfiguration_;

	persistentView_->motorGroupView()->setSelectedGroupObject(VESPERSBeamline::vespers()->motorGroupName(vespersUserConfiguration->motor()));

	foreach (VESPERSScanConfiguration *configuration, configurations){

		configuration->setCCDDetector(vespersUserConfiguration->ccdDetector());
		configuration->setFluorescenceDetector(vespersUserConfiguration->fluorescenceDetector());
		configuration->setIncomingChoice(vespersUserConfiguration->incomingChoice());
		configuration->setTransmissionChoice(vespersUserConfiguration->transmissionChoice());
	}
}

void VESPERSAppController::onRegionOfInterestAdded(AMRegionOfInterest *region)
{
	userConfiguration_->addRegionOfInterest(region);
	mapScanConfiguration_->addRegionOfInterest(region);
	map3DScanConfiguration_->addRegionOfInterest(region);
	exafsScanConfiguration_->addRegionOfInterest(region);
	lineScanConfiguration_->addRegionOfInterest(region);
	timeScanConfiguration_->addRegionOfInterest(region);
	timedLineScanConfiguration_->addRegionOfInterest(region);
}

void VESPERSAppController::onRegionOfInterestRemoved(AMRegionOfInterest *region)
{
	userConfiguration_->removeRegionOfInterest(region);
	mapScanConfiguration_->removeRegionOfInterest(region);
	map3DScanConfiguration_->removeRegionOfInterest(region);
	exafsScanConfiguration_->removeRegionOfInterest(region);
	lineScanConfiguration_->removeRegionOfInterest(region);
	timeScanConfiguration_->removeRegionOfInterest(region);
	timedLineScanConfiguration_->removeRegionOfInterest(region);
}

void VESPERSAppController::onRegionOfInterestBoundingRangeChanged(AMRegionOfInterest *region)
{
	userConfiguration_->setRegionOfInterestBoundingRange(region);
	mapScanConfiguration_->setRegionOfInterestBoundingRange(region);
	map3DScanConfiguration_->setRegionOfInterestBoundingRange(region);
	exafsScanConfiguration_->setRegionOfInterestBoundingRange(region);
	lineScanConfiguration_->setRegionOfInterestBoundingRange(region);
	timeScanConfiguration_->setRegionOfInterestBoundingRange(region);
	timedLineScanConfiguration_->setRegionOfInterestBoundingRange(region);
}
