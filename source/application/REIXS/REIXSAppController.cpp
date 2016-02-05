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


#include "REIXSAppController.h"

#include <QMessageBox>

#include "actions3/AMActionRunner3.h"
#include "actions3/AMActionRegistry3.h"
#include "actions3/actions/AMSamplePlatePre2013MoveAction.h"
#include "actions3/editors/AMSamplePlatePre2013MoveActionEditor.h"
#include "application/AMAppControllerSupport.h"
#include "beamline/CLS/CLSStorageRing.h"

#include "dataman/AMRun.h"
#include "dataman/AMScan.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/export/AMExporterAthena.h"
#include "dataman/export/AMExporterOptionGeneralAscii.h"

#include "util/AMErrorMonitor.h"

#include "ui/AMMainWindow.h"
#include "ui/AMBeamlineCameraWidgetWithSourceTabs.h"
#include "ui/acquaman/AMScanConfigurationView.h"
#include "ui/acquaman/AMScanConfigurationViewHolder3.h"
#include "ui/dataman/AMGenericScanEditor.h"
#include "ui/REIXS/REIXSSampleManagementPre2013Widget.h"
#include "ui/util/AMChooseDataFolderDialog.h"

#include "acquaman/AMGenericStepScanConfiguration.h"
#include "acquaman/REIXS/REIXSXESScanConfiguration.h"
#include "acquaman/REIXS/REIXSXASScanConfiguration.h"
#include "analysis/REIXS/REIXSXESImageAB.h"
#include "analysis/REIXS/REIXSXESImageInterpolationAB.h"
#include "application/REIXS/REIXS.h"
#include "dataman/REIXS/REIXSXESCalibration.h"
#include "dataman/REIXS/REIXSXESMCPDetectorInfo.h"
#include "beamline/REIXS/REIXSBeamline.h"
#include "beamline/REIXS/REIXSSampleManipulator.h"

#include "ui/acquaman/AMGenericStepScanConfigurationView.h"
#include "ui/REIXS/REIXSSidebar.h"
#include "ui/REIXS/REIXSXESScanConfigurationDetailedView.h"
#include "ui/REIXS/REIXSXASScanConfigurationView.h"
#include "ui/REIXS/REIXSRIXSScanConfigurationView.h"
#include "ui/REIXS/REIXSXESSpectrometerControlPanel.h"
#include "ui/REIXS/REIXSXESHexapodControlEditor.h"
#include "ui/REIXS/REIXSXESSpectrometerControlEditor.h"
#include "ui/REIXS/REIXSSampleChamberButtonPanel.h"
#include "ui/REIXS/REIXSAppBottomPanel.h"


REIXSAppController::REIXSAppController(QObject *parent) :
	CLSAppController(CLSAppController::REIXSBeamlineId, parent)
{
	setDefaultUseLocalStorage(true);
}

bool REIXSAppController::startup()
{

	if (!AMChooseDataFolderDialog::getDataFolder("/AcquamanLocalData/reixs", "/home/reixs", "users"))
		return false;

	if(CLSAppController::startup()) {

		setupUserInterface();
		makeConnections();

		return true;
	}

	else
		return false;

}

void REIXSAppController::shutdown() {

	// Make sure we release/clean-up the beamline interface
	AMBeamline::releaseBl();
	AMAppController::shutdown();
}

void REIXSAppController::onScanEditorCreated(AMGenericScanEditor *editor)
{
	connect(editor, SIGNAL(scanAdded(AMGenericScanEditor*,AMScan*)), this, SLOT(onScanAddedToEditor(AMGenericScanEditor*,AMScan*)));
}

void REIXSAppController::onCurrentScanActionStartedImplementation(AMScanAction *action)
{
	Q_UNUSED(action);
	connect(CLSStorageRing::sr1(), SIGNAL(beamAvaliability(bool)), this, SLOT(onBeamAvailabilityChanged(bool)));
}

void REIXSAppController::onCurrentScanActionFinishedImplementation(AMScanAction *action)
{
	Q_UNUSED(action);
	disconnect(CLSStorageRing::sr1(), SIGNAL(beamAvaliability(bool)), this, SLOT(onBeamAvailabilityChanged(bool)));
}

void REIXSAppController::onBeamAvailabilityChanged(bool beamAvailable)
{
	if (!beamAvailable && !AMActionRunner3::workflow()->pauseCurrentAction())
		AMActionRunner3::workflow()->setQueuePaused(true);

	// On REIXS, we don't like having the scan restart on it's own.
	else if (beamAvailable && AMActionRunner3::workflow()->queuedActionCount() > 0)
		AMActionRunner3::workflow()->setQueuePaused(false);
}

void REIXSAppController::onScanAddedToEditor(AMGenericScanEditor *editor, AMScan *scan)
{
	QString exclusiveName = QString();

	for (int i = 0, count = scan->analyzedDataSourceCount(); i < count && exclusiveName.isNull(); i++){

		AMDataSource *source = scan->analyzedDataSources()->at(i);

		if (source->name().contains("TEYNorm") && !source->hiddenFromUsers())
			exclusiveName = source->name();

		if (source->name().contains("xesSpectrum") && !source->hiddenFromUsers())
			exclusiveName = source->name();
	}

	if (!exclusiveName.isNull())
		editor->setExclusiveDataSourceByName(exclusiveName);

	else if (editor->scanAt(0)->analyzedDataSourceCount())
		editor->setExclusiveDataSourceByName(editor->scanAt(0)->analyzedDataSources()->at(editor->scanAt(0)->analyzedDataSourceCount()-1)->name());
}

void REIXSAppController::initializeBeamline()
{
	// Initialize central beamline object
	REIXSBeamline::bl();
}

void REIXSAppController::registerBeamlineDBClasses()
{
	AMDbObjectSupport::s()->registerClass<REIXSXESScanConfiguration>();
	AMDbObjectSupport::s()->registerClass<REIXSXASScanConfiguration>();
	AMDbObjectSupport::s()->registerClass<REIXSXESMCPDetectorInfo>();
	AMDbObjectSupport::s()->registerClass<REIXSXESCalibration>();
	AMDbObjectSupport::s()->registerClass<REIXSXESImageAB>();
	AMDbObjectSupport::s()->registerClass<REIXSXESImageInterpolationAB>();
	AMActionRegistry3::s()->registerInfoAndAction<AMSamplePlatePre2013MoveActionInfo, AMSamplePlatePre2013MoveAction>("Move Sample Position", "Move to a different marked sample position", ":system-run.png");
	AMActionRegistry3::s()->registerInfoAndEditor<AMSamplePlatePre2013MoveActionInfo, AMSamplePlatePre2013MoveActionEditor>();
}

void REIXSAppController::setupUserInterface()
{
	// Create panes in the main window:
	////////////////////////////////////


	mw_->insertHeading("Experiment Setup", 1);
	//////////


	xesScanConfigurationView_ = new REIXSXESScanConfigurationDetailedView(REIXSBeamline::bl()->mcpDetector());
	xesScanConfigurationViewHolder_ = new AMScanConfigurationViewHolder3(xesScanConfigurationView_);
	mw_->addPane(xesScanConfigurationViewHolder_, "Experiment Setup", "Emission Scan", ":/utilities-system-monitor.png");


	REIXSRIXSScanConfigurationView* rixsConfigView = new REIXSRIXSScanConfigurationView();
	rixsScanConfigurationViewHolder_ = new AMScanConfigurationViewHolder3(rixsConfigView);
	mw_->addPane(rixsScanConfigurationViewHolder_, "Experiment Setup", "RIXS Scan", ":/utilities-system-monitor.png");


	REIXSXASScanConfiguration *xasScanConfiguration = new REIXSXASScanConfiguration();

	REIXSXASScanConfigurationView* xasConfigView = new REIXSXASScanConfigurationView(xasScanConfiguration);
	xasScanConfigurationViewHolder_ = new AMScanConfigurationViewHolder3(xasConfigView, true);
	mw_->addPane(xasScanConfigurationViewHolder_, "Experiment Setup", "Absorption Scan", ":/utilities-system-monitor.png");

	connect(xasScanConfiguration, SIGNAL(totalTimeChanged(double)), xasScanConfigurationViewHolder_, SLOT(updateOverallScanTime(double)));
	xasScanConfigurationViewHolder_->updateOverallScanTime(xasScanConfiguration->totalTime());

	genericScanConfiguration_ = new AMGenericStepScanConfiguration;
	genericScanConfigurationView_ = new AMGenericStepScanConfigurationView(genericScanConfiguration_,
									       AMBeamline::bl()->exposedControls(),
									       AMBeamline::bl()->exposedDetectors());
	genericScanConfigurationViewHolder_ = new AMScanConfigurationViewHolder3("Generic Scan", false, true, genericScanConfigurationView_);
	mw_->addPane(genericScanConfigurationViewHolder_, "Experiment Setup", "Generic Scan", ":/utilities-system-monitor.png");

	spectrometerPanel_ = new REIXSXESSpectrometerControlPanel(REIXSBeamline::bl()->mcpDetector(), 0);
	mw_->addPane(spectrometerPanel_, "Experiment Setup", "Spectromter Setup", ":/22x22/gnome-display-properties.png");


	sampleChamberButtonPanel_ = new REIXSSampleChamberButtonPanel();
	REIXSSampleManagementPre2013Widget* sampleManagementPane = new REIXSSampleManagementPre2013Widget(sampleChamberButtonPanel_,
																				  QUrl("http://v2e1610-401.clsi.ca/mjpg/1/video.mjpg"),
																				  QUrl("http://v2e1610-401.clsi.ca/mjpg/2/video.mjpg"),
																				  REIXSBeamline::bl()->samplePlate(),
																				  new REIXSSampleManipulator(),
																				  0);

	mw_->addPane(sampleManagementPane, "Experiment Setup", "Sample Positions", ":/22x22/gnome-display-properties.png");

	////////////////// Temporary testing/commissioning widgets ////////////////////
	 /*
	QWidget* spectrometerControlWidget = new QWidget();
	QHBoxLayout* hl = new QHBoxLayout();
	hl->addWidget(new REIXSXESHexapodControlEditor(REIXSBeamline::bl()->spectrometer()->hexapod()));

	QGroupBox* gb = new QGroupBox("Motors");
	QVBoxLayout* vl = new QVBoxLayout();
	vl->addWidget(new QLabel("Spectrometer Rotation"));
	vl->addWidget(new AMControlEditor(REIXSBeamline::bl()->spectrometer()->spectrometerRotationDrive()));

	vl->addWidget(new QLabel("Detector Translation"));
	vl->addWidget(new AMControlEditor(REIXSBeamline::bl()->spectrometer()->detectorTranslation()));

	vl->addWidget(new QLabel("Detector Tilt"));
	vl->addWidget(new AMControlEditor(REIXSBeamline::bl()->spectrometer()->detectorTiltDrive()));

	gb->setLayout(vl);

	hl->addWidget(gb);

	hl->addWidget(new REIXSXESSpectrometerControlEditor(REIXSBeamline::bl()->spectrometer()));

	hl->addStretch(1);

	spectrometerControlWidget->setLayout(hl);
	mw_->addPane(spectrometerControlWidget, "Experiment Setup", "Spectrometer controls", ":/utilities-system-monitor.png");
	*/
	////////////////// End of Temporary testing/commissioning widgets ////////////////////


	// Add the sidebar, for real-time display of the beamline.
	////////////////////////
	sidebar_ = new REIXSSidebar();
	mw_->addRightWidget(sidebar_);
}

void REIXSAppController::setupExporterOptions()
{
	AMExporterOptionGeneralAscii *exportOptions = REIXS::buildStandardExporterOption("REIXSXASDefault", false);
	if(exportOptions->id() > 0)
		AMAppControllerSupport::registerClass<REIXSXASScanConfiguration, AMExporterGeneralAscii, AMExporterOptionGeneralAscii>(exportOptions->id());

	exportOptions = REIXS::buildStandardExporterOption("REIXSXESDefault", true);
	if(exportOptions->id() > 0)
		AMAppControllerSupport::registerClass<REIXSXESScanConfiguration, AMExporterGeneralAscii, AMExporterOptionGeneralAscii>(exportOptions->id());
}

void REIXSAppController::makeConnections()
{
	connect(REIXSBeamline::bl()->spectrometer(), SIGNAL(connected(bool)), REIXSBeamline::bl()->spectrometer(), SLOT(updateGrating()));
	connect(xesScanConfigurationViewHolder_, SIGNAL(showWorkflowRequested()), this, SLOT(goToWorkflow()));
	connect(rixsScanConfigurationViewHolder_, SIGNAL(showWorkflowRequested()), this, SLOT(goToWorkflow()));
	connect(xasScanConfigurationViewHolder_, SIGNAL(showWorkflowRequested()), this, SLOT(goToWorkflow()));
	connect(this, SIGNAL(scanEditorCreated(AMGenericScanEditor*)), this, SLOT(onScanEditorCreated(AMGenericScanEditor*)));
}

void REIXSAppController::addBottomPanel()
{
	REIXSAppBottomPanel *panel = new REIXSAppBottomPanel(AMActionRunner3::workflow());
	mw_->addBottomWidget(panel);
	connect(panel, SIGNAL(addExperimentButtonClicked()), this, SLOT(onAddButtonClicked()));
	bottomPanel_ = panel;
}
