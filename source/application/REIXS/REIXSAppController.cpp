/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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

#include "ui/AMMainWindow.h"

#include "util/AMErrorMonitor.h"
#include "dataman/database/AMDbObjectSupport.h"

// For database registration:
#include "dataman/REIXS/REIXSXESCalibration.h"
#include "dataman/REIXS/REIXSXESMCPDetectorInfo.h"
#include "acquaman/REIXS/REIXSXESScanConfiguration.h"

#include "ui/REIXS/REIXSXESHexapodControlEditor.h"
#include "ui/REIXS/REIXSXESSpectrometerControlEditor.h"
#include "ui/REIXS/REIXSSampleChamberButtonPanel.h"

#include "ui/dataman/AMSampleManagementWidget.h"	/// \todo This doesn't belong in dataman
#include "ui/AMBeamlineCameraWidgetWithSourceTabs.h"
#include "dataman/AMRun.h"
#include "ui/AMStartScreen.h"

// old workflow manager view: (to be removed)
#include "ui/AMWorkflowManagerView.h"

#include "ui/actions2/AMWorkflowView.h"
#include "actions2/AMActionRunner.h"
#include "actions2/AMActionRegistry.h"
#include "actions2/AMLoopAction.h"
#include "actions2/actions/AMWaitAction.h"
#include "actions2/actions/REIXS/REIXSControlMoveAction.h"
#include "actions2/actions/AMScanControllerAction.h"
#include "actions2/actions/AMInternalControlMoveAction.h"
#include "actions2/actions/REIXS/REIXSXESScanAction.h"
#include "actions2/actions/REIXS/REIXSSampleMoveAction.h"
#include "actions2/actions/REIXS/REIXSMoveToSampleTransferPositionAction.h"
#include "actions2/editors/AMWaitActionEditor.h"
#include "actions2/editors/AMLoopActionEditor.h"
#include "actions2/editors/REIXS/REIXSXESScanActionEditor.h"
#include "actions2/editors/REIXS/REIXSControlMoveActionEditor.h"

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

bool REIXSAppController::startup() {

    // Initialize central beamline object
    REIXSBeamline::bl();

    if(AMAppController::startup()) {

        AMDbObjectSupport::s()->registerClass<REIXSXESScanConfiguration>();
        AMDbObjectSupport::s()->registerClass<REIXSXESMCPDetectorInfo>();
        AMDbObjectSupport::s()->registerClass<REIXSXESCalibration>();

        AMDbObjectSupport::s()->registerClass<REIXSControlMoveActionInfo>();
        AMDbObjectSupport::s()->registerClass<REIXSXESScanActionInfo>();
        AMDbObjectSupport::s()->registerClass<REIXSSampleMoveActionInfo>();

        // Testing and making the first run in the database, if there isn't one already.  Make this it's own function if you think startup() is getting too big ; )
        ////////////////////////////////////////

        AMRun existingRun;
        if(!existingRun.loadFromDb(AMDatabase::database("user"), 1)) {
            // no run yet... let's create one.
            AMRun firstRun("REIXS", 5);	/// \todo For now, we know that 5 is the ID of the REIXS facility, but this is a hardcoded hack. See AMFirstTimeController::onFirstTime() for where the facilities are created.
            firstRun.storeToDb(AMDatabase::database("user"));
        }

        // Show the splash screen, to let the user pick their current run. (It will delete itself when closed)
        AMStartScreen* startScreen = new AMStartScreen(0);
        startScreen->show();

        // Testing Joystick:
#ifdef Q_OS_LINUX
        // AMJoystick* joystick = new AMGenericLinuxJoystick("/dev/input/js0", this);
        // AMJoystickTestView* testView = new AMJoystickTestView(joystick);
        // testView->show();
#endif

		// Register Actions:
		////////////////////////////////

        /// \todo Move common ones to main app controller.
        AMActionRegistry::s()->registerInfoAndAction<AMWaitActionInfo, AMWaitAction>("Wait", "This action simply waits for a specified amount of time.", ":/user-away.png");
		AMActionRegistry::s()->registerInfoAndAction<REIXSControlMoveActionInfo, REIXSControlMoveAction>("Move Control", "This action moves any REIXS beamline control to a target position.\n\nYou can specify an absolute or a relative move.", ":/system-run.png");
        AMActionRegistry::s()->registerInfoAndAction<REIXSXESScanActionInfo, REIXSXESScanAction>("XES Scan", "This action conducts a single XES scan at a given detector energy.", ":/utilities-system-monitor.png");
        AMActionRegistry::s()->registerInfoAndAction<AMLoopActionInfo, AMLoopAction>("Loop", "This action repeats a set of sub-actions a specific number of times.\n\nAfter adding it, you can drag-and-drop other actions inside it.", ":/32x32/media-playlist-repeat.png");
		AMActionRegistry::s()->registerInfoAndAction<REIXSSampleMoveActionInfo, REIXSSampleMoveAction>("Move Sample: Measure", "This action moves the REIXS sample manipulator to the default measurement position.", ":/32x32/gnome-display-properties.png");
		AMActionRegistry::s()->registerInfoAndAction<REIXSMoveToSampleTransferPositionActionInfo, REIXSMoveToSampleTransferPositionAction>("Move Sample: Transfer", "This action moves the REIXS sample manipulator to the sample transfer position.", ":/32x32/media-eject.png");

        AMActionRegistry::s()->registerInfoAndEditor<AMWaitActionInfo, AMWaitActionEditor>();
        AMActionRegistry::s()->registerInfoAndEditor<AMLoopActionInfo, AMLoopActionEditor>();
        AMActionRegistry::s()->registerInfoAndEditor<REIXSXESScanActionInfo, REIXSXESScanActionEditor>();
        AMActionRegistry::s()->registerInfoAndEditor<REIXSControlMoveActionInfo, REIXSControlMoveActionEditor>();


        // Create panes in the main window:
        ////////////////////////////////////

        // add the workflow control UI
        workflowView_ = new AMWorkflowView();
        mw_->addPane(workflowView_, "Experiment Tools", "Workflow", ":/user-away.png");
        // remove the old one:
        mw_->removePane(workflowManagerView_);
        workflowManagerView_->hide();


        mw_->insertHeading("Experiment Setup", 1);
        //////////
        xesScanConfigurationView_ = new REIXSXESScanConfigurationDetailedView(REIXSBeamline::bl()->mcpDetector());
        REIXSScanConfigurationViewHolder* scanConfigurationHolder = new REIXSScanConfigurationViewHolder(xesScanConfigurationView_);
        mw_->addPane(scanConfigurationHolder, "Experiment Setup", "Emission Scan", ":/utilities-system-monitor.png");
        connect(scanConfigurationHolder, SIGNAL(showWorkflowRequested()), this, SLOT(goToWorkflow()));
        AMSampleManagementWidget* sampleManagementPane = new AMSampleManagementWidget(new REIXSSampleChamberButtonPanel(),
                                                                                      QUrl("http://v2e1610-101.clsi.ca/mjpg/1/video.mjpg"),
                                                                                      "Sample Camera: down beam path",
                                                                                      0,
                                                                                      new REIXSSampleManipulator(),
                                                                                      0);

        mw_->addPane(sampleManagementPane, "Experiment Setup", "Sample Positions", ":/22x22/gnome-display-properties.png");

        ////////////////// Testing junk; move somewhere clean ////////////////////
        QWidget* spectrometerControlWidget = new QWidget();
        QHBoxLayout* hl = new QHBoxLayout();
        hl->addWidget(new REIXSXESHexapodControlEditor(REIXSBeamline::bl()->spectrometer()->hexapod()));

        QGroupBox* gb = new QGroupBox("Motors");
        QVBoxLayout* vl = new QVBoxLayout();
        vl->addWidget(new QLabel("Spectrometer Rotation"));
        vl->addWidget(new AMBasicControlEditor(REIXSBeamline::bl()->spectrometer()->spectrometerRotationDrive()));

        vl->addWidget(new QLabel("Detector Translation"));
        vl->addWidget(new AMBasicControlEditor(REIXSBeamline::bl()->spectrometer()->detectorTranslation()));

        vl->addWidget(new QLabel("Detector Tilt"));
        vl->addWidget(new AMBasicControlEditor(REIXSBeamline::bl()->spectrometer()->detectorTiltDrive()));

        gb->setLayout(vl);

        hl->addWidget(gb);

        hl->addWidget(new REIXSXESSpectrometerControlEditor(REIXSBeamline::bl()->spectrometer()));

        hl->addStretch(1);

        spectrometerControlWidget->setLayout(hl);
        mw_->addPane(spectrometerControlWidget, "Experiment Setup", "Spectrometer controls", ":/utilities-system-monitor.png");
        ////////////////// End of testing junk; move somewhere clean ////////////////////




		// Add the sidebar, for real-time display of the beamline.
		////////////////////////
		REIXSSidebar* sidebar = new REIXSSidebar();
		mw_->addRightWidget(sidebar);

		// Make connections
		//////////////////////////
		/// \todo Eventually move into AMAppController
        connect(AMActionRunner::s(), SIGNAL(currentActionStateChanged(int,int)), this, SLOT(onCurrentActionStateChanged(int,int)));



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

//void REIXSAppController::tempAddLoopActions()
//{
//	AMLoopAction* loopAction = qobject_cast<AMLoopAction*>(AMActionRunner::s()->queuedActionAt(0));

//	loopAction->insertSubAction(new AMWaitAction(10), -1);
//	loopAction->insertSubAction(new AMWaitAction(11), -1);
//	AMLoopAction* inner1 = new AMLoopAction(2);
//	loopAction->insertSubAction(inner1, -1);

//	inner1->insertSubAction(new AMWaitAction(12), -1);

//	// inner1->insertSubAction(new AMWaitAction(13), -1);
//}

void REIXSAppController::goToWorkflow()
{
    mw_->setCurrentPane(workflowView_);
}

#include "acquaman/AMScanController.h"

void REIXSAppController::onCurrentActionStateChanged(int newState, int oldState)
{
    Q_UNUSED(oldState)

    if(newState == AMAction::Running) {
        AMAction* action = AMActionRunner::s()->currentAction();

        AMScanControllerAction* scanAction = qobject_cast<AMScanControllerAction*>(action);
        if(scanAction) {
            AMScanController* scanController = scanAction->scanController();
            if(scanController) {
                openScanInEditorAndTakeOwnership(scanController->scan());
            }
        }
    }
}




