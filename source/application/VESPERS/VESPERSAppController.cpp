/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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

#include "beamline/VESPERS/VESPERSBeamline.h"
#include "ui/VESPERS/VESPERSEndstationView.h"
#include "ui/AMMainWindow.h"
#include "ui/AMBottomBar.h"
#include "ui/acquaman/AMScanConfigurationViewHolder3.h"
#include "ui/acquaman/VESPERS/VESPERSScanConfigurationViewHolder3.h"

#include "dataman/AMLineScan.h"

#include "actions3/AMActionRunner3.h"
#include "actions3/actions/AMScanAction.h"

#include "ui/VESPERS/XRFDetectorView.h"
#include "ui/VESPERS/VESPERSXRFFreeRunView.h"
#include "ui/VESPERS/VESPERSPersistentView.h"
#include "dataman/VESPERS/AMXRFScan.h"
#include "util/AMPeriodicTable.h"
#include "ui/VESPERS/VESPERSDeviceStatusView.h"
#include "ui/VESPERS/VESPERSEXAFSScanConfigurationView.h"
#include "ui/VESPERS/VESPERSCCDDetectorView.h"
#include "ui/VESPERS/VESPERSPilatusCCDDetectorView.h"
#include "ui/VESPERS/VESPERS2DScanConfigurationView.h"
#include "ui/VESPERS/VESPERSSpatialLineScanConfigurationView.h"
#include "ui/VESPERS/VESPERSEnergyScanConfigurationView.h"
#include "acquaman/VESPERS/VESPERSScanConfiguration.h"
#include "ui/VESPERS/VESPERSChooseDataFolderDialog.h"
#include "ui/VESPERS/VESPERS3DScanConfigurationView.h"

#include "dataman/AMScanEditorModelItem.h"
#include "ui/dataman/AMGenericScanEditor.h"

#include "dataman/database/AMDbObjectSupport.h"
#include "application/AMAppControllerSupport.h"
#include "dataman/VESPERS/VESPERSDbUpgrade1Pt1.h"
#include "dataman/VESPERS/VESPERSDbUpgrade1Pt2.h"
#include "dataman/VESPERS/VESPERSDbUpgrade1Pt3.h"
#include "dataman/VESPERS/VESPERSDbUpgrade1Pt4.h"
#include "dataman/VESPERS/VESPERSDbUpgrade1Pt5.h"

#include "dataman/export/AMExportController.h"
#include "dataman/export/AMExporterOptionGeneralAscii.h"
#include "dataman/export/AMExporterGeneralAscii.h"
#include "dataman/export/AMExporterAthena.h"
#include "dataman/export/VESPERS/VESPERSExporter3DAscii.h"
#include "dataman/export/VESPERS/VESPERSExporter2DAscii.h"
#include "dataman/export/VESPERS/VESPERSExporterSMAK.h"
#include "dataman/export/VESPERS/VESPERSExporterLineScanAscii.h"

#include <QFileDialog>
#include <QMessageBox>

#include "dataman/AMRun.h"

// For database registration:
#include "dataman/VESPERS/XRFDetectorInfo.h"

VESPERSAppController::VESPERSAppController(QObject *parent) :
	AMAppController(parent)
{
	moveImmediatelyAction_ = 0;

	roperCCDStartup_ = false;
	marCCDStartup_ = false;
	pilatusCCDStartup_ = false;

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
}

bool VESPERSAppController::startup() {

	// Get a destination folder.
	if (!VESPERSChooseDataFolderDialog::getDataFolder())
		return false;

	// Start up the main program.
	if(AMAppController::startup()) {

		// Initialize central beamline object
		VESPERSBeamline::vespers();
		// Initialize the periodic table object.
		AMPeriodicTable::table();

		registerClasses();

		// Ensuring we automatically switch scan editors for new scans.
		setAutomaticBringScanEditorToFront(true);

		// Testing and making the first run in the database, if there isn't one already.  Make this it's own function if you think startup() is getting too big ; )
		////////////////////////////////////////

		AMRun existingRun;
		if(!existingRun.loadFromDb(AMDatabase::database("user"), 1)) {
			// no run yet... let's create one.
			AMRun firstRun("VESPERS", 4);	/// \todo For now, we know that 4 is the ID of the VESPERS facility, but this is a hardcoded hack.
			firstRun.storeToDb(AMDatabase::database("user"));
		}

		if (!ensureProgramStructure())
			return false;

		setupExporterOptions();
		setupUserInterface();
		makeConnections();

		// Github setup for adding VESPERS specific comment.
		additionalIssueTypesAndAssignees_.append("I think it's a VESPERS specific issue", "dretrex");

		// THIS IS HERE TO PASS ALONG THE INFORMATION TO THE SUM AND CORRECTEDSUM PVS IN THE FOUR ELEMENT DETECTOR.
		roiHelper_ = new ROIHelper;
		attoHack_ = new VESPERSAttoCubeHack(VESPERSBeamline::vespers()->attoStageRz());

		return true;
	}
	else
		return false;
}

bool VESPERSAppController::ensureProgramStructure()
{
	QString homeDir = VESPERS::getHomeDirectory();

	if (!QFileInfo(homeDir % "/acquaman/devConfigurationFiles/VESPERS").exists()){

		QDir temp = QDir();
		temp.cd(homeDir % "/acquaman/devConfigurationFiles");

		if (!temp.mkdir("VESPERS")){

			AMErrorMon::error(this, VESPERSAPPCONTROLLER_COULD_NOT_CREATE_VESPERS_FOLDER, "Could not create the VESPERS config folder.  Notify beamline staff.");
			return false;
		}
	}

	if (!QFileInfo("/nas/vespers").exists()){

		AMErrorMon::error(this, VESPERSAPPCONTROLLER_AURORA_PATH_NOT_FOUND, "Path to aurora not found.  Notify beamline staff.");
		return false;
	}

	// This one is not critical to the running of the application.
	if (!QFileInfo("/nas/pilatus").exists())
		AMErrorMon::alert(this, VESPERSAPPCONTROLLER_PILATUS_PATH_NOT_FOUND, "The path to the Pilatus CCD was not found.  Many special features for the Pilatus are now unavailable.");

	return true;
}

void VESPERSAppController::shutdown() {
	// Make sure we release/clean-up the beamline interface
	delete roiHelper_;
	delete attoHack_;
	AMBeamline::releaseBl();
	AMAppController::shutdown();
}

void VESPERSAppController::registerClasses()
{
	AMDbObjectSupport::s()->registerClass<XRFDetectorInfo>();
	AMDbObjectSupport::s()->registerClass<VESPERSXRFScanConfiguration>();
	AMDbObjectSupport::s()->registerClass<AMXRFScan>();
	AMDbObjectSupport::s()->registerClass<VESPERSEXAFSScanConfiguration>();
	AMDbObjectSupport::s()->registerClass<VESPERS2DScanConfiguration>();
	AMDbObjectSupport::s()->registerClass<VESPERSSpatialLineScanConfiguration>();
	AMDbObjectSupport::s()->registerClass<VESPERSEnergyScanConfiguration>();
	AMDbObjectSupport::s()->registerClass<AMLineScan>();
	AMDbObjectSupport::s()->registerClass<VESPERSScanConfigurationDbObject>();
	AMDbObjectSupport::s()->registerClass<VESPERS3DScanConfiguration>();

	AMOldDetectorViewSupport::registerClass<XRFBriefDetectorView, XRFDetector>();
	AMOldDetectorViewSupport::registerClass<XRFDetailedDetectorView, XRFDetector>();
	AMOldDetectorViewSupport::registerClass<VESPERSCCDDetectorView, VESPERSRoperCCDDetector>();
	AMOldDetectorViewSupport::registerClass<VESPERSCCDDetectorView, VESPERSMarCCDDetector>();
	AMOldDetectorViewSupport::registerClass<VESPERSPilatusCCDDetectorView, VESPERSPilatusCCDDetector>();

	AMExportController::registerExporter<VESPERSExporter3DAscii>();
	AMExportController::registerExporter<VESPERSExporter2DAscii>();
	AMExportController::registerExporter<VESPERSExporterSMAK>();
	AMExportController::registerExporter<VESPERSExporterLineScanAscii>();
}

void VESPERSAppController::setupExporterOptions()
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
}

void VESPERSAppController::setupUserInterface()
{
	// Create panes in the main window:
	////////////////////////////////////

	// Setup the general endstation control view.
	endstationView_ = new VESPERSEndstationView(VESPERSBeamline::vespers()->endstation());
	// Setup the general status page.
	VESPERSDeviceStatusView *statusPage = new VESPERSDeviceStatusView;

	mw_->insertHeading("General", 0);
	mw_->addPane(endstationView_, "General", "Endstation", ":/system-software-update.png");
	mw_->addPane(statusPage, "General", "Device Status", ":/system-software-update.png");

	// Setup the XRF views for the single element vortex and the four element vortex detectors.  Since they have scans that are added to the workflow, it gets the workflow manager view passed into it as well.
	// This means that the FreeRunView kind of doubles as a regular detector view and a configuration view holder.
	xrf1ElFreeRun_ = new XRFFreeRun(VESPERSBeamline::vespers()->vortexXRF1E());
	xrf1EFreeRunView_ = new VESPERSXRFFreeRunView(xrf1ElFreeRun_);
	xrf4ElFreeRun_ = new XRFFreeRun(VESPERSBeamline::vespers()->vortexXRF4E());
	xrf4EFreeRunView_ = new VESPERSXRFFreeRunView(xrf4ElFreeRun_);

	roperCCDView_ = new VESPERSCCDDetectorView(VESPERSBeamline::vespers()->roperCCD());
	marCCDView_ = new VESPERSCCDDetectorView(VESPERSBeamline::vespers()->marCCD());
	pilatusView_ = new VESPERSPilatusCCDDetectorView(VESPERSBeamline::vespers()->pilatusCCD());

	mw_->insertHeading("Detectors", 1);
	mw_->addPane(xrf1EFreeRunView_, "Detectors", "Fluorescence - 1-el", ":/system-search.png");
	mw_->addPane(xrf4EFreeRunView_, "Detectors", "Fluorescence - 4-el", ":/system-search.png");
//	mw_->addPane(roperCCDView_, "Detectors", "Area - Roper", ":/system-search.png");
//	mw_->addPane(marCCDView_, "Detectors", "Area - Mar", ":/system-search.png");
	mw_->addPane(pilatusView_, "Detectors", "Area - Pilatus", ":/system-search.png");

	// Setup XAS for the beamline.  Builds the config, view, and view holder.
	exafsScanConfig_ = new VESPERSEXAFSScanConfiguration();
	exafsScanConfig_->addRegion(0, -30, 0.5, 40, 1);
	exafsConfigView_ = new VESPERSEXAFSScanConfigurationView(exafsScanConfig_);
	exafsConfigViewHolder3_ = new VESPERSScanConfigurationViewHolder3(exafsConfigView_);
	connect(exafsConfigView_, SIGNAL(configureDetector(QString)), this, SLOT(onConfigureDetectorRequested(QString)));

	// Setup 2D maps for the beamline.  Builds the config, view, and view holder.
	mapScanConfiguration_ = new VESPERS2DScanConfiguration();
	mapScanConfiguration_->setStepSize(0.005, 0.005);
	mapScanConfiguration_->setTimeStep(1);
	mapScanConfigurationView_ = new VESPERS2DScanConfigurationView(mapScanConfiguration_);
	mapScanConfigurationViewHolder3_ = new AMScanConfigurationViewHolder3(mapScanConfigurationView_);
	connect(mapScanConfigurationView_, SIGNAL(configureDetector(QString)), this, SLOT(onConfigureDetectorRequested(QString)));

	// Setup 2D maps for the beamline.  Builds the config, view, and view holder.
	map3DScanConfiguration_ = new VESPERS3DScanConfiguration();
	map3DScanConfiguration_->setStepSize(0.005, 0.005, 0.005);
	map3DScanConfiguration_->setTimeStep(1);
	map3DScanConfigurationView_ = new VESPERS3DScanConfigurationView(map3DScanConfiguration_);
	map3DScanConfigurationViewHolder3_ = new AMScanConfigurationViewHolder3(map3DScanConfigurationView_);
	connect(map3DScanConfigurationView_, SIGNAL(configureDetector(QString)), this, SLOT(onConfigureDetectorRequested(QString)));

	// Setup line scans for the beamline.  Builds the config, view, and view holder.
	lineScanConfiguration_ = new VESPERSSpatialLineScanConfiguration();
	lineScanConfiguration_->addRegion(0, 0, 0.005, 1, 1);
	lineScanConfiguration_->regions()->setUnits(0, "mm");
	lineScanConfiguration_->regions()->setTimeUnits(0, "s");
	lineScanConfigurationView_ = new VESPERSSpatialLineScanConfigurationView(lineScanConfiguration_);
	lineScanConfigurationViewHolder3_ = new AMScanConfigurationViewHolder3(lineScanConfigurationView_);
	connect(lineScanConfigurationView_, SIGNAL(configureDetector(QString)), this, SLOT(onConfigureDetectorRequested(QString)));

	// Setup energy scans for the beamline.  Builds the config, view, and view holder.
	energyScanConfiguration_ = new VESPERSEnergyScanConfiguration();
	energyScanConfiguration_->addRegion(0, 10000, 1000, 20000, 1);
	energyScanConfiguration_->regions()->setUnits(0, " eV");
	energyScanConfiguration_->regions()->setTimeUnits(0, " s");
	energyScanConfigurationView_ = new VESPERSEnergyScanConfigurationView(energyScanConfiguration_);
	energyScanConfigurationViewHolder3_ = new AMScanConfigurationViewHolder3(energyScanConfigurationView_);
	connect(energyScanConfigurationView_, SIGNAL(configureDetector(QString)), this, SLOT(onConfigureDetectorRequested(QString)));

	mw_->insertHeading("Scans", 2);
	mw_->addPane(exafsConfigViewHolder3_, "Scans", "XAS", ":/utilities-system-monitor.png");
	mw_->addPane(mapScanConfigurationViewHolder3_, "Scans", "2D Maps", ":/utilities-system-monitor.png");
	mw_->addPane(lineScanConfigurationViewHolder3_, "Scans", "Line Scan", ":/utilities-system-monitor.png");
	mw_->addPane(energyScanConfigurationViewHolder3_, "Scans", "XRD Energy Scan", ":/utilities-system-monitor.png");
	mw_->addPane(map3DScanConfigurationViewHolder3_, "Scans", "3D Maps", ":/utilities-system-monitor.png");

	// This is the right hand panel that is always visible.  Has important information such as shutter status and overall controls status.  Also controls the sample stage.
	persistentView_ = new VESPERSPersistentView;
	mw_->addRightWidget(persistentView_);

	// Show the endstation control view first.
	mw_->setCurrentPane(endstationView_);
}

void VESPERSAppController::makeConnections()
{
	connect(this, SIGNAL(scanEditorCreated(AMGenericScanEditor*)), this, SLOT(onScanEditorCreated(AMGenericScanEditor*)));
	connect(persistentView_, SIGNAL(currentSampleStageChanged(QString)), this, SLOT(onSampleStageChoiceChanged(QString)));
	// copy ROIs from one detector to another.
	connect(xrf1ElFreeRun_, SIGNAL(copyRoisRequested(const XRFFreeRun*)), xrf4ElFreeRun_, SLOT(setFromXRFFreeRun(const XRFFreeRun*)));
	connect(xrf4ElFreeRun_, SIGNAL(copyRoisRequested(const XRFFreeRun*)), xrf1ElFreeRun_, SLOT(setFromXRFFreeRun(const XRFFreeRun*)));

	// Startup connections for the CCD detectors.
	connect(VESPERSBeamline::vespers()->roperCCD(), SIGNAL(connected(bool)), this, SLOT(onRoperCCDConnected(bool)));
	connect(VESPERSBeamline::vespers()->marCCD(), SIGNAL(connected(bool)), this, SLOT(onMarCCDConnected(bool)));
	connect(VESPERSBeamline::vespers()->pilatusCCD(), SIGNAL(connected(bool)), this, SLOT(onPilatusCCDConnected(bool)));
}

void VESPERSAppController::onConfigureDetectorRequested(const QString &detector)
{
	if (detector == "Single Element")
		mw_->setCurrentPane(xrf1EFreeRunView_);
	else if (detector == "Four Element")
		mw_->setCurrentPane(xrf4EFreeRunView_);
	else if (detector == "Roper CCD")
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

	connect(VESPERSBeamline::vespers(), SIGNAL(beamDumped()), this, SLOT(onBeamDump()));
}

void VESPERSAppController::onCurrentScanActionFinishedImplementation(AMScanAction *action)
{
	QString fileFormat(action->controller()->scan()->fileFormat());

	if (fileFormat == "vespersXRF" || fileFormat == "vespers2011XRF")
		return;

	disconnect(VESPERSBeamline::vespers(), SIGNAL(beamDumped()), this, SLOT(onBeamDump()));
}

void VESPERSAppController::onBeamDump()
{
	AMAction3 *action = AMActionRunner3::workflow()->currentAction();

	if (action && action->canPause() && action->state() == AMAction3::Running)
		action->pause();
}

void VESPERSAppController::onPauseScanIssued()
{
	AMAction3 *action = AMActionRunner3::workflow()->currentAction();

	if (action && action->canPause() && action->state() == AMAction3::Running)
		action->pause();

	else if (action && action->canPause() && action->state() == AMAction3::Paused)
		action->resume();
}

void VESPERSAppController::onCancelScanIssued()
{
	AMAction3 *action = AMActionRunner3::workflow()->currentAction();

	if (action)
		action->cancel();
}

void VESPERSAppController::onScanEditorCreated(AMGenericScanEditor *editor)
{
	connect(editor, SIGNAL(scanAdded(AMGenericScanEditor*,AMScan*)), this, SLOT(onScanAddedToEditor(AMGenericScanEditor*,AMScan*)));
	editor->setPlotRange(AMPeriodicTable::table()->elementBySymbol("K")->Kalpha().second.toDouble(), 20480);

	if (editor->using2DScanView())
		connect(editor, SIGNAL(dataPositionChanged(AMGenericScanEditor*,QPoint)), this, SLOT(onDataPositionChanged(AMGenericScanEditor*,QPoint)));
}

void VESPERSAppController::onScanAddedToEditor(AMGenericScanEditor *editor, AMScan *scan)
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

	editor->setPlotRange(AMPeriodicTable::table()->elementBySymbol("K")->Kalpha().second.toDouble(), 20480);
}

void VESPERSAppController::onDataPositionChanged(AMGenericScanEditor *editor, const QPoint &pos)
{
	// This should always succeed because the only way to get into this function is using the 2D scan view which currently only is accessed by 2D scans.
	VESPERS2DScanConfiguration *config = qobject_cast<VESPERS2DScanConfiguration *>(editor->currentScan()->scanConfiguration());

	if (!config)
		return;

	QString text = "Setup at (H,V,N): (";
	text.append(QString::number(editor->dataPosition().x(), 'f', 3));
	text.append(" mm, ");
	text.append(QString::number(editor->dataPosition().y(), 'f', 3));
	text.append(" mm, ");
	text.append(QString::number(config->normalPosition(), 'f', 3));
	text.append(" mm)");

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
	cleanMoveImmediatelyAction();	// Clean up the action just in case.

	// This should always succeed because the only way to get into this function is using the 2D scan view which currently only is accessed by 2D scans.
	VESPERS2DScanConfiguration *config = qobject_cast<VESPERS2DScanConfiguration *>(editor->currentScan()->scanConfiguration());

	if (!config)
		return;

	if (config->motor() == (VESPERS::H | VESPERS::V)){

		AMBeamlineParallelActionsList *moveImmediatelyList = new AMBeamlineParallelActionsList;
		moveImmediatelyAction_ = new AMBeamlineListAction(moveImmediatelyList);
		moveImmediatelyList->appendStage(new QList<AMBeamlineActionItem *>());
		moveImmediatelyList->appendAction(0, VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->createHorizontalMoveAction(editor->dataPosition().x()));
		moveImmediatelyList->appendStage(new QList<AMBeamlineActionItem *>());
		moveImmediatelyList->appendAction(1, VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->createVerticalMoveAction(editor->dataPosition().y()));

		if (config->normalPosition() != 888888.88){

			moveImmediatelyList->appendStage(new QList<AMBeamlineActionItem *>());
			moveImmediatelyList->appendAction(2, VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->createNormalMoveAction(config->normalPosition()));
		}

		connect(moveImmediatelyAction_, SIGNAL(succeeded()), this, SLOT(onMoveImmediatelySuccess()));
		connect(moveImmediatelyAction_, SIGNAL(failed(int)), this, SLOT(onMoveImmediatelyFailure()));
		moveImmediatelyAction_->start();
	}

	else if (config->motor() == (VESPERS::X | VESPERS::Z)){

		AMBeamlineParallelActionsList *moveImmediatelyList = new AMBeamlineParallelActionsList;
		moveImmediatelyAction_ = new AMBeamlineListAction(moveImmediatelyList);
		moveImmediatelyList->appendStage(new QList<AMBeamlineActionItem *>());
		moveImmediatelyList->appendAction(0, VESPERSBeamline::vespers()->realSampleStageMotorGroupObject()->createHorizontalMoveAction(editor->dataPosition().x()));
		moveImmediatelyList->appendStage(new QList<AMBeamlineActionItem *>());
		moveImmediatelyList->appendAction(1, VESPERSBeamline::vespers()->realSampleStageMotorGroupObject()->createVerticalMoveAction(editor->dataPosition().y()));

		if (config->normalPosition() != 888888.88){

			moveImmediatelyList->appendStage(new QList<AMBeamlineActionItem *>());
			moveImmediatelyList->appendAction(2, VESPERSBeamline::vespers()->realSampleStageMotorGroupObject()->createNormalMoveAction(config->normalPosition()));
		}

		connect(moveImmediatelyAction_, SIGNAL(succeeded()), this, SLOT(onMoveImmediatelySuccess()));
		connect(moveImmediatelyAction_, SIGNAL(failed(int)), this, SLOT(onMoveImmediatelyFailure()));
		moveImmediatelyAction_->start();
	}

	else if (config->motor() == (VESPERS::BigBeamX | VESPERS::BigBeamZ)){

		AMBeamlineParallelActionsList *moveImmediatelyList = new AMBeamlineParallelActionsList;
		moveImmediatelyAction_ = new AMBeamlineListAction(moveImmediatelyList);
		moveImmediatelyList->appendStage(new QList<AMBeamlineActionItem *>());
		moveImmediatelyList->appendAction(0, VESPERSBeamline::vespers()->bigBeamMotorGroupObject()->createHorizontalMoveAction(editor->dataPosition().x()));
		moveImmediatelyList->appendStage(new QList<AMBeamlineActionItem *>());
		moveImmediatelyList->appendAction(1, VESPERSBeamline::vespers()->bigBeamMotorGroupObject()->createVerticalMoveAction(editor->dataPosition().y()));

		connect(moveImmediatelyAction_, SIGNAL(succeeded()), this, SLOT(onMoveImmediatelySuccess()));
		connect(moveImmediatelyAction_, SIGNAL(failed(int)), this, SLOT(onMoveImmediatelyFailure()));
		moveImmediatelyAction_->start();
	}
}

void VESPERSAppController::onMoveImmediatelySuccess()
{
}

void VESPERSAppController::onMoveImmediatelyFailure()
{
	QMessageBox::warning(mw_, "Sample Stage Error", "The sample stage was unable to complete the desired movement.");
}

void VESPERSAppController::cleanMoveImmediatelyAction()
{
	if (moveImmediatelyAction_ == 0)
		return;

	// Disconnect all signals and return all memory.
	moveImmediatelyAction_->disconnect();
	AMBeamlineParallelActionsList *actionList = moveImmediatelyAction_->list();

	for (int i = 0; i < actionList->stageCount(); i++){

		while (actionList->stage(i)->size())
			actionList->stage(i)->takeAt(0)->deleteLater();
	}

	moveImmediatelyAction_->deleteLater();
	moveImmediatelyAction_ = 0;
}

void VESPERSAppController::setupXASScan(const AMGenericScanEditor *editor, bool setupEXAFS)
{
	exafsScanConfig_->setGoToPosition(true);
	exafsScanConfig_->setX(editor->dataPosition().x());
	exafsScanConfig_->setY(editor->dataPosition().y());

	QString edge = editor->exclusiveDataSourceName();
	edge = edge.remove("norm_");
	edge.chop(2);
	edge.insert(edge.size()-1, " ");

	if (edge.at(edge.size()-1) == 'L')
		edge.append("1");

	exafsScanConfig_->setEdge(edge);

	// This should always succeed because the only way to get into this function is using the 2D scan view which currently only is accessed by 2D scans.
	VESPERS2DScanConfiguration *config = qobject_cast<VESPERS2DScanConfiguration *>(editor->currentScan()->scanConfiguration());
	if (config){

		exafsScanConfig_->setName(config->name());
		exafsScanConfig_->setFluorescenceDetector(config->fluorescenceDetector());
		exafsScanConfig_->setIncomingChoice(config->incomingChoice());
	}

	while (exafsScanConfig_->regionCount() != 1)
		exafsScanConfig_->deleteRegion(0);

	if (setupEXAFS){

		exafsScanConfig_->exafsRegions()->setType(0, AMEXAFSRegion::Energy);
		exafsScanConfig_->setRegionStart(0, -200);
		exafsScanConfig_->setRegionDelta(0, 10);
		exafsScanConfig_->setRegionEnd(0, -30);
		exafsScanConfig_->setRegionTime(0, 1);

		exafsScanConfig_->regions()->addRegion(1, -30, 0.5, 40, 1);
		exafsScanConfig_->exafsRegions()->setType(1, AMEXAFSRegion::Energy);

		exafsScanConfig_->regions()->addRegion(2, 40, 0.05, 857.4627, 10); // 857.4627 = 15k
		exafsScanConfig_->exafsRegions()->setType(2, AMEXAFSRegion::kSpace);
		exafsScanConfig_->exafsRegions()->setEndByType(2, 15, AMEXAFSRegion::kSpace);
	}

	else {

		exafsScanConfig_->setRegionStart(0, -30);
		exafsScanConfig_->setRegionDelta(0, 0.5);
		exafsScanConfig_->setRegionEnd(0, 40);
		exafsScanConfig_->setRegionTime(0, 1);
		exafsScanConfig_->exafsRegions()->setType(0, AMEXAFSRegion::Energy);
	}

	mw_->undock(exafsConfigViewHolder3_);
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
		mapScanConfiguration_->setFastAxis(config->fastAxis());
		mapScanConfiguration_->setXRange(mapRect.left(), mapRect.right());
		mapScanConfiguration_->setYRange(mapRect.bottom(), mapRect.top());
		mapScanConfiguration_->setStepSize(config->steps());
		mapScanConfiguration_->setTimeStep(config->timeStep());
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
		exafsScanConfig_->setMotor(motor);

	if (energyScanMotorAcceptable(motor))
		energyScanConfiguration_->setMotor(motor);

	if (mapMotorAcceptable(motor))
		mapScanConfiguration_->setMotor(motor);

	if (lineScanMotorAcceptable(motor))
		lineScanConfiguration_->setMotor(convertSampleStageMotorToIndividualMotor(motor));
}

bool VESPERSAppController::exafsMotorAcceptable(int motor) const
{
	if (motor == (VESPERS::H | VESPERS::V)
			|| motor == (VESPERS::X | VESPERS::Z)
			|| motor == (VESPERS::AttoH | VESPERS::AttoV)
			|| motor == (VESPERS::AttoX | VESPERS::AttoZ))

		return true;

	return false;
}

bool VESPERSAppController::energyScanMotorAcceptable(int motor) const
{
	if (motor == (VESPERS::H | VESPERS::V)
			|| motor == (VESPERS::X | VESPERS::Z)
			|| motor == (VESPERS::AttoH | VESPERS::AttoV)
			|| motor == (VESPERS::AttoX | VESPERS::AttoZ))

		return true;

	return false;
}

bool VESPERSAppController::mapMotorAcceptable(int motor) const
{
	if (motor == (VESPERS::H | VESPERS::V)
			|| motor == (VESPERS::X | VESPERS::Z)
			|| motor == (VESPERS::AttoH | VESPERS::AttoV)
			|| motor == (VESPERS::AttoX | VESPERS::AttoZ)
			|| motor == (VESPERS::BigBeamX | VESPERS::BigBeamZ))

		return true;

	return false;
}

bool VESPERSAppController::lineScanMotorAcceptable(int motor) const
{
	if (motor == (VESPERS::H | VESPERS::V)
			|| motor == (VESPERS::X | VESPERS::Z)
			|| motor == (VESPERS::AttoH | VESPERS::AttoV)
			|| motor == (VESPERS::AttoX | VESPERS::AttoZ)
			|| motor == VESPERS::AttoRx
			|| motor == VESPERS::AttoRy
			|| motor == VESPERS::AttoRz
			|| motor == (VESPERS::BigBeamX | VESPERS::BigBeamZ))

		return true;

	return false;
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

	// A default that won't cause crashes.
	return VESPERS::H;
}

void VESPERSAppController::fixCDF(const QUrl &url)
{
	// turn off automatic raw-day loading for scans... This will make loading the scan to access it's config much faster.
	bool scanAutoLoadingOn = AMScan::autoLoadData();
	AMScan::setAutoLoadData(false);

	AMScan* scan = AMScan::createFromDatabaseUrl(url, true);

	// restore AMScan's auto-loading of data to whatever it was before.
	AMScan::setAutoLoadData(scanAutoLoadingOn);

	if(!scan) {
		return;
	}

	// Does the scan have a configuration?
	AMScanConfiguration* config = scan->scanConfiguration();
	if(!config) {
		scan->release();
		return;
	}

	// Which scan configuration is it?
	if (qobject_cast<VESPERSEXAFSScanConfiguration *>(config)){

		QString filename = scan->filePath();
		filename.replace(".cdf", ".dat");
		scan->setFileFormat("vespers2011EXAFS");
		scan->setFilePath(filename);

		VESPERSEXAFSScanConfiguration *exafsConfig = qobject_cast<VESPERSEXAFSScanConfiguration *>(config);
		if (exafsConfig->fluorescenceDetector() > 0){

			QString additionalFilename = filename;
			additionalFilename.replace(".dat", "_spectra.dat");
			scan->setAdditionalFilePaths(QStringList() << additionalFilename);
		}
	}

	else if (qobject_cast<VESPERS2DScanConfiguration *>(config)){

		VESPERS2DScanConfiguration *mapConfig = qobject_cast<VESPERS2DScanConfiguration *>(config);
		bool usingCcd = mapConfig->ccdDetector() == VESPERS::Roper ? true : false;
		bool usingSingleElement = (mapConfig->fluorescenceDetector() == VESPERS::SingleElement) || (mapConfig->fluorescenceDetector() == (VESPERS::SingleElement | VESPERS::FourElement));
		bool usingFourElement = (mapConfig->fluorescenceDetector() == VESPERS::FourElement) || (mapConfig->fluorescenceDetector() == (VESPERS::SingleElement | VESPERS::FourElement));

		QString filename = scan->filePath();
		filename.replace(".cdf", ".dat");
		QString additionalFilename = filename;
		additionalFilename.replace(".dat", "_spectra.dat");

		if (usingSingleElement && usingFourElement)
			scan->setFileFormat(QString("vespers2012XRF1Eln4El%1").arg(usingCcd ? "XRD" : ""));

		else if (usingSingleElement)
			scan->setFileFormat(QString("vespers2012XRF1El%1").arg(usingCcd ? "XRD" : ""));

		else if (usingFourElement)
			scan->setFileFormat(QString("vespers2012XRF4El%1").arg(usingCcd ? "XRD" : ""));

		scan->setFilePath(filename);
		scan->setAdditionalFilePaths(QStringList() << additionalFilename);
	}

	else if (qobject_cast<VESPERSSpatialLineScanConfiguration *>(config)){

		VESPERSSpatialLineScanConfiguration *lineConfig = qobject_cast<VESPERSSpatialLineScanConfiguration *>(config);
		bool usingCcd = lineConfig->ccdDetector() == VESPERS::Roper ? true : false;
		bool usingSingleElement = (lineConfig->fluorescenceDetector() == VESPERS::SingleElement) || (lineConfig->fluorescenceDetector() == (VESPERS::SingleElement | VESPERS::FourElement));
		bool usingFourElement = (lineConfig->fluorescenceDetector() == VESPERS::FourElement) || (lineConfig->fluorescenceDetector() == (VESPERS::SingleElement | VESPERS::FourElement));

		QString filename = scan->filePath();
		filename.replace(".cdf", ".dat");
		QString additionalFilename = filename;
		additionalFilename.replace(".dat", "_spectra.dat");

		if (usingSingleElement && usingFourElement)
			scan->setFileFormat(QString("vespers2012LineScanXRF1Eln4El%1").arg(usingCcd ? "XRD" : ""));

		else if (usingSingleElement)
			scan->setFileFormat(QString("vespers2012LineScanXRF1El%1").arg(usingCcd ? "XRD" : ""));

		else if (usingFourElement)
			scan->setFileFormat(QString("vespers2012LineScanXRF4El%1").arg(usingCcd ? "XRD" : ""));

		scan->setFilePath(filename);
		scan->setAdditionalFilePaths(QStringList() << additionalFilename);
	}

	else if (qobject_cast<VESPERSEnergyScanConfiguration *>(config)){

		QString filename = scan->filePath();
		filename.replace(".cdf", ".dat");
		scan->setFileFormat("vespers2012Energy");
		scan->setFilePath(filename);
	}

	else {

		scan->release();
		return;
	}

	// Save the changes to db object.
	scan->storeToDb(AMDatabase::database("user"));
	// Load it from the database to use the appropriate file loader and build a new CDF file.
	scan->loadFromDb(AMDatabase::database("user"), scan->id());
	// Release the object.
	scan->release();
}

void VESPERSAppController::onRoperCCDConnected(bool connected)
{
	if (!roperCCDStartup_ && connected){

		roperCCDStartup_ = true;
		QString proposalNumber = VESPERS::getProposalNumber(AMUserSettings::userDataFolder);

		if (!proposalNumber.isEmpty())
			VESPERSBeamline::vespers()->roperCCD()->setCCDPath(QString("/nas/vespers/%1/CCDImages").arg(proposalNumber));
	}
}

void VESPERSAppController::onMarCCDConnected(bool connected)
{
	if (!marCCDStartup_ && connected){

		marCCDStartup_ = true;
		QString proposalNumber = VESPERS::getProposalNumber(AMUserSettings::userDataFolder);

		if (!proposalNumber.isEmpty())
			VESPERSBeamline::vespers()->marCCD()->setCCDPath(QString("/home/vespers/users/%1/").arg(proposalNumber));
	}
}

void VESPERSAppController::onPilatusCCDConnected(bool connected)
{
	if (!pilatusCCDStartup_ && connected){

		pilatusCCDStartup_ = true;
		QString proposalNumber = VESPERS::getProposalNumber(AMUserSettings::userDataFolder);

		if (!proposalNumber.isEmpty())
			VESPERSBeamline::vespers()->pilatusCCD()->setCCDPath(QString("/ramdisk/%1/").arg(proposalNumber));
	}
}
