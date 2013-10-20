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


#include "REIXSAppController.h"

#include "beamline/REIXS/REIXSBeamline.h"
#include "beamline/REIXS/REIXSSampleManipulator.h"

#include "ui/acquaman/AMScanConfigurationView.h"
#include "ui/REIXS/REIXSXESScanConfigurationDetailedView.h"
#include "ui/REIXS/REIXSScanConfigurationViewHolder.h"
#include "ui/REIXS/REIXSXASScanConfigurationView.h"
#include "acquaman/REIXS/REIXSXASScanConfiguration.h"

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

#include "ui/dataman/AMSampleManagementWidget.h"	/// \todo This doesn't belong in dataman
#include "ui/AMBeamlineCameraWidgetWithSourceTabs.h"
#include "dataman/AMRun.h"

#include "actions2/AMActionRegistry.h"

//#include "actions2/actions/REIXS/REIXSControlMoveAction.h"
//#include "actions2/actions/REIXS/REIXSXESScanAction.h"
//#include "actions2/actions/REIXS/REIXSSampleMoveAction.h"
//#include "actions2/actions/REIXS/REIXSMoveToSampleTransferPositionAction.h"
//#include "actions2/actions/REIXS/REIXSBeamOnOffAction.h"
//#include "actions2/editors/REIXS/REIXSXESScanActionEditor.h"
//#include "actions2/editors/REIXS/REIXSControlMoveActionEditor.h"
//#include "actions2/editors/REIXS/REIXSBeamOnOffActionEditor.h"
//#include "actions2/editors/REIXS/REIXSSampleMoveActionEditor.h"
//#include "actions2/actions/REIXS/REIXSXASScanAction.h"

#include "analysis/REIXS/REIXSXESImageAB.h"

#include "ui/REIXS/REIXSSidebar.h"

#include <QMessageBox>

#ifdef Q_OS_LINUX
#include "util/AMGenericLinuxJoystick.h"
#include "ui/util/AMJoystickTestView.h"
#endif

REIXSAppController::REIXSAppController(QObject *parent) :
	AMAppController(parent)
{
}

// Re-implemented to create the REIXSBeamline object
bool REIXSAppController::startupBeforeAnything() {
	if(!AMAppController::startupBeforeAnything()) return false;

	// If a command-line option has been specified to choose a new userDataFolder:
	if(qApp->arguments().contains("--ChooseUserDataFolder"))
		getUserDataFolderFromDialog();

	// Initialize the central beamline object
	REIXSBeamline::bl();

	return true;
}

// Re-implemented to register REIXS-specific database classes
bool REIXSAppController::startupRegisterDatabases() {
	if(!AMAppController::startupRegisterDatabases()) return false;

	AMDbObjectSupport::s()->registerClass<REIXSXESScanConfiguration>();
	AMDbObjectSupport::s()->registerClass<REIXSXASScanConfiguration>();
	AMDbObjectSupport::s()->registerClass<REIXSXESMCPDetectorInfo>();
	AMDbObjectSupport::s()->registerClass<REIXSXESCalibration>();
	AMDbObjectSupport::s()->registerClass<REIXSXESImageAB>();

	//AMDbObjectSupport::s()->registerClass<REIXSControlMoveActionInfo>();
	//AMDbObjectSupport::s()->registerClass<REIXSXESScanActionInfo>();
	//AMDbObjectSupport::s()->registerClass<REIXSXASScanActionInfo>();
	//AMDbObjectSupport::s()->registerClass<REIXSSampleMoveActionInfo>();
	//AMDbObjectSupport::s()->registerClass<REIXSBeamOnOffActionInfo>();

	// Register Actions:
	////////////////////////////////

	/// \todo Move common ones to main app controller.
	//AMActionRegistry::s()->registerInfoAndAction<REIXSControlMoveActionInfo, REIXSControlMoveAction>("Move Control", "This action moves any REIXS beamline control to a target position.\n\nYou can specify an absolute or a relative move.", ":/system-run.png");
	//AMActionRegistry::s()->registerInfoAndAction<REIXSXESScanActionInfo, REIXSXESScanAction>("XES Scan", "This action conducts a single XES scan at a given detector energy.", ":/utilities-system-monitor.png");
	//AMActionRegistry::s()->registerInfoAndAction<REIXSXASScanActionInfo, REIXSXASScanAction>("XAS Scan", "This action conducts an XAS scan over an incident energy range.", ":/utilities-system-monitor.png");
	//AMActionRegistry::s()->registerInfoAndAction<REIXSSampleMoveActionInfo, REIXSSampleMoveAction>("Move Sample", "This action moves the REIXS sample manipulator to a defined position, or switches between samples on the current sample plate.", ":/32x32/gnome-display-properties.png");
	//AMActionRegistry::s()->registerInfoAndAction<REIXSMoveToSampleTransferPositionActionInfo, REIXSMoveToSampleTransferPositionAction>("Move to Transfer", "This action moves the REIXS sample manipulator to the sample transfer position.", ":/32x32/media-eject.png");
	//AMActionRegistry::s()->registerInfoAndAction<REIXSBeamOnOffActionInfo, REIXSBeamOnOffAction>("Beam On/Off", "This action turns the beam on or off.");


	//AMActionRegistry::s()->registerInfoAndEditor<REIXSXESScanActionInfo, REIXSXESScanActionEditor>();
	/// \todo Editor for XAS Scan actions...
	//AMActionRegistry::s()->registerInfoAndEditor<REIXSControlMoveActionInfo, REIXSControlMoveActionEditor>();
	//AMActionRegistry::s()->registerInfoAndEditor<REIXSBeamOnOffActionInfo, REIXSBeamOnOffActionEditor>();
	//AMActionRegistry::s()->registerInfoAndEditor<REIXSSampleMoveActionInfo, REIXSSampleMoveActionEditor>();

	return true;
}

// Re-implemented to add REIXS-specific user interfaces
bool REIXSAppController::startupCreateUserInterface() {
	if(!AMAppController::startupCreateUserInterface()) return false;

	// Create panes in the main window:
	////////////////////////////////////


	mw_->insertHeading("Experiment Setup", 1);
	//////////
	REIXSScanConfigurationViewHolder* scanConfigurationHolder;

	xesScanConfigurationView_ = new REIXSXESScanConfigurationDetailedView(REIXSBeamline::bl()->mcpDetector());
	scanConfigurationHolder = new REIXSScanConfigurationViewHolder(xesScanConfigurationView_);
	mw_->addPane(scanConfigurationHolder, "Experiment Setup", "Emission Scan", ":/utilities-system-monitor.png");
	connect(scanConfigurationHolder, SIGNAL(showWorkflowRequested()), this, SLOT(goToWorkflow()));


	REIXSXASScanConfigurationView* xasConfigView = new REIXSXASScanConfigurationView();
	scanConfigurationHolder = new REIXSScanConfigurationViewHolder(xasConfigView);
	mw_->addPane(scanConfigurationHolder, "Experiment Setup", "Absorption Scan", ":/utilities-system-monitor.png");
	connect(scanConfigurationHolder, SIGNAL(showWorkflowRequested()), this, SLOT(goToWorkflow()));


	REIXSSampleChamberButtonPanel* buttonPanel = new REIXSSampleChamberButtonPanel();
	AMSampleManagementWidget* sampleManagementPane = new AMSampleManagementWidget(buttonPanel,
																				  QUrl("http://v2e1610-101.clsi.ca/mjpg/1/video.mjpg"),
																				  "Sample Camera: down beam path",
																				  REIXSBeamline::bl()->samplePlate(),
																				  new REIXSSampleManipulator(),
																				  0);

	mw_->addPane(sampleManagementPane, "Experiment Setup", "Sample Positions", ":/22x22/gnome-display-properties.png");
	// Testing Joystick:
#ifdef Q_OS_LINUX
	 AMJoystick* joystick = new AMGenericLinuxJoystick("/dev/input/js0", this);
	 connect(joystick, SIGNAL(buttonChanged(int,bool,quint32)), buttonPanel, SLOT(onJoystickButtonChanged(int,bool)));
//         AMJoystickTestView* testView = new AMJoystickTestView(joystick);
//         testView->show();
#endif

	////////////////// Temporary testing/commissioning widgets ////////////////////
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

	return true;
}

void REIXSAppController::shutdown() {

	// Make sure we release/clean-up the beamline interface
	AMBeamline::releaseBl();
	AMAppController::shutdown();
}

#include "dataman/AMScan.h"
void REIXSAppController::launchScanConfigurationFromDb(const QUrl &url)
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
	// need to create a copy of the config so we can delete the scan (and hence the config instance owned by the scan). The view will take ownership of the copy.
	config = config->createCopy();
	scan->release();
	if(!config)
		return;

	AMScanConfigurationView *view = config->createView();
	if(!view) {
		delete config;
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -401, "Unable to create view from the scan configuration loaded from the database.  Please report this problem to the beamline's software developers."));
		return;
	}

	REIXSScanConfigurationViewHolder* holder = new REIXSScanConfigurationViewHolder(view);
	holder->setAttribute(Qt::WA_DeleteOnClose, true);
	holder->show();
}





