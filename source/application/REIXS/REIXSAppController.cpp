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

#include "beamline/REIXS/REIXSBeamline.h"
#include "beamline/REIXS/REIXSSampleManipulator.h"

#include "ui/dataman/AMGenericScanEditor.h"
#include "dataman/AMScan.h"
#include "ui/acquaman/AMScanConfigurationView.h"
#include "ui/REIXS/REIXSXESScanConfigurationDetailedView.h"
#include "ui/REIXS/REIXSXASScanConfigurationView.h"
#include "ui/REIXS/REIXSRIXSScanConfigurationView.h"
#include "acquaman/REIXS/REIXSXASScanConfiguration.h"
#include "ui/REIXS/REIXSXESSpectrometerControlPanel.h"
#include "ui/acquaman/AMScanConfigurationViewHolder3.h"
#include "ui/AMMainWindow.h"

#include "util/AMErrorMonitor.h"
#include "dataman/database/AMDbObjectSupport.h"

// For database registration:
#include "dataman/REIXS/REIXSXESCalibration.h"
#include "dataman/REIXS/REIXSXESMCPDetectorInfo.h"
#include "acquaman/REIXS/REIXSXESScanConfiguration.h"
#include "acquaman/REIXS/REIXSXASScanConfiguration.h"

#include "ui/REIXS/REIXSXESHexapodControlEditor.h"
#include "ui/REIXS/REIXSXESSpectrometerControlEditor.h"
#include "ui/REIXS/REIXSSampleChamberButtonPanel.h"

#include "ui/dataman/AMSampleManagementPre2013Widget.h"	/// \todo This doesn't belong in dataman
#include "ui/AMBeamlineCameraWidgetWithSourceTabs.h"
#include "dataman/AMRun.h"

#include "analysis/REIXS/REIXSXESImageAB.h"
#include "analysis/REIXS/REIXSXESImageInterpolationAB.h"

#include "actions3/actions/AMSamplePlatePre2013MoveAction.h"
#include "actions3/editors/AMSamplePlatePre2013MoveActionEditor.h"

#include "ui/REIXS/REIXSSidebar.h"


#include <QMessageBox>

#include "actions3/AMActionRegistry3.h"

REIXSAppController::REIXSAppController(QObject *parent) :
	AMAppController(parent)
{
}

bool REIXSAppController::startup()
{
	bool success = true;

	success &= AMAppController::startup();
	success &= AMActionRegistry3::s()->registerInfoAndAction<AMSamplePlatePre2013MoveActionInfo, AMSamplePlatePre2013MoveAction>("Move Sample Position", "Move to a different marked sample position", ":system-run.png");
	success &= AMActionRegistry3::s()->registerInfoAndEditor<AMSamplePlatePre2013MoveActionInfo, AMSamplePlatePre2013MoveActionEditor>();

	return success;
}

// Re-implemented to create the REIXSBeamline object
bool REIXSAppController::startupBeforeAnything() {
	if(!AMAppController::startupBeforeAnything()) return false;

	// If a command-line option has been specified to choose a new userDataFolder:
	getUserDataFolderFromDialog();

	// Initialize the central beamline object
	REIXSBeamline::bl();
	connect(REIXSBeamline::bl()->spectrometer(), SIGNAL(connected(bool)), REIXSBeamline::bl()->spectrometer(), SLOT(updateGrating()));

	return true;
}

// Re-implemented to register REIXS-specific database classes
bool REIXSAppController::startupRegisterDatabases()
{
	if(!AMAppController::startupRegisterDatabases())
		return false;

	AMDbObjectSupport::s()->registerClass<REIXSXESScanConfiguration>();
	AMDbObjectSupport::s()->registerClass<REIXSXASScanConfiguration>();
	AMDbObjectSupport::s()->registerClass<REIXSXESMCPDetectorInfo>();
	AMDbObjectSupport::s()->registerClass<REIXSXESCalibration>();
	AMDbObjectSupport::s()->registerClass<REIXSXESImageAB>();
	AMDbObjectSupport::s()->registerClass<REIXSXESImageInterpolationAB>();

	return true;
}

// Re-implemented to add REIXS-specific user interfaces
bool REIXSAppController::startupCreateUserInterface() {
	if(!AMAppController::startupCreateUserInterface()) return false;

	// Create panes in the main window:
	////////////////////////////////////


	mw_->insertHeading("Experiment Setup", 1);
	//////////
	AMScanConfigurationViewHolder3* scanConfigurationHolder;

	xesScanConfigurationView_ = new REIXSXESScanConfigurationDetailedView(REIXSBeamline::bl()->mcpDetector());
	scanConfigurationHolder = new AMScanConfigurationViewHolder3(xesScanConfigurationView_);
	mw_->addPane(scanConfigurationHolder, "Experiment Setup", "Emission Scan", ":/utilities-system-monitor.png");
	connect(scanConfigurationHolder, SIGNAL(showWorkflowRequested()), this, SLOT(goToWorkflow()));

	REIXSRIXSScanConfigurationView* rixsConfigView = new REIXSRIXSScanConfigurationView();
	scanConfigurationHolder = new AMScanConfigurationViewHolder3(rixsConfigView);
	mw_->addPane(scanConfigurationHolder, "Experiment Setup", "RIXS Scan", ":/utilities-system-monitor.png");
	connect(scanConfigurationHolder, SIGNAL(showWorkflowRequested()), this, SLOT(goToWorkflow()));


	REIXSXASScanConfiguration *xasScanConfiguration = new REIXSXASScanConfiguration();

	REIXSXASScanConfigurationView* xasConfigView = new REIXSXASScanConfigurationView(xasScanConfiguration);
	scanConfigurationHolder = new AMScanConfigurationViewHolder3(xasConfigView);
	mw_->addPane(scanConfigurationHolder, "Experiment Setup", "Absorption Scan", ":/utilities-system-monitor.png");
	connect(scanConfigurationHolder, SIGNAL(showWorkflowRequested()), this, SLOT(goToWorkflow()));

	REIXSXESSpectrometerControlPanel* spectrometerPanel = new REIXSXESSpectrometerControlPanel(REIXSBeamline::bl()->mcpDetector(), 0);
	mw_->addPane(spectrometerPanel, "Experiment Setup", "Spectromter Setup", ":/22x22/gnome-display-properties.png");


	REIXSSampleChamberButtonPanel* buttonPanel = new REIXSSampleChamberButtonPanel();
	AMSampleManagementPre2013Widget* sampleManagementPane = new AMSampleManagementPre2013Widget(buttonPanel,
																				  QUrl("http://v2e1610-401.clsi.ca/mjpg/1/video.mjpg"),
																				  "Sample Camera: down beam path",
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
	REIXSSidebar* sidebar = new REIXSSidebar();
	mw_->addRightWidget(sidebar);

	return true;
}




bool REIXSAppController::startupAfterEverything() {
	if(!AMAppController::startupAfterEverything()) return false;

	// Checking for and making the first run in the database, if there isn't one already.
	////////////////////////////////////////
	AMRun existingRun;
	if(!existingRun.loadFromDb(AMDatabase::database("user"), 1)) {
		// no run yet... let's create one.
		AMRun firstRun("REIXS", 5);	/// \todo For now, we know that 5 is the ID of the REIXS facility, but this is a hardcoded hack. See AMFirstTimeController::onFirstTime() for where the facilities are created.
		firstRun.storeToDb(AMDatabase::database("user"));
	}

	connect(this, SIGNAL(scanEditorCreated(AMGenericScanEditor*)), this, SLOT(onScanEditorCreated(AMGenericScanEditor*)));

	return true;
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
