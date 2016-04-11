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


#ifndef AMAPPCONTROLLER_H
#define AMAPPCONTROLLER_H

#include <QObject>

#include "application/AMDatamanAppControllerForActions3.h"
#include <QHash>

class AMWorkflowView3;
class AMMainWindow;
class AMScan;
class AMScanConfiguration;
class AMExporter;
class AMExporterOption;
class AMScanAction;
class AMFacility;

/// This class extends the base dataman app controller class by adding the workflow.  This is the base class for all beamline acquisition app controllers.  The reason for the distinction between this class and the dataman version is a result for the desire to be able to take the dataman version home with the user whereas this version is meant to reside on beamlines that always have access to beamline components and controls.
class AMAppController : public AMDatamanAppControllerForActions3
{
Q_OBJECT
public:
	/// This constructor is empty. Call AMAppController::startup() to create all of the application windows, widgets, and data objects that are needed on program startup.
	explicit AMAppController(QObject *parent = 0);
	/// The destructor is empty.  Call AMAppController::shutdown() to delete all objects and clean up.
	virtual ~AMAppController();

	/// create and setup all of the application windows, widgets, communication connections, and data objects that are needed on program startup. Returns true on success.  If reimplementing, must call the base-class startup() as the first thing it does.
	virtual bool startup();

	/// Re-implemented from AMDatamanAppController to add the workflow pane, and show the run selection dialog / splash screen.
	virtual bool startupCreateUserInterface();
	/// Re-implemented from AMDatamanAppController to provide a menu action for changing the current run.
	virtual bool startupInstallActions();

	/// destroy all of the windows, widgets, and data objects created by applicationStartup(). Only call this if startup() has ran successfully.  If reimplementing, must call the base-class shutdown() as the last thing it does.
	virtual void shutdown();

signals:

public slots:

	/// This function can be used to "hand-off" a scan to the app controller (for example, a new scan from a scan controller). An editor will be opened for the scan. (Thanks to AMScan's retain()/release() mechanism, the scan will be deleted automatically when all scan controllers and editors are done with it.)
	/*! By default, the new scan editor will be brought to the front of the main window and presented to the user. You can suppress this by setting \c bringEditorToFront to false.

If \c openInExistingEditor is set to true, and if there is an existing editor, the scan will be appended inside that editor. (If there is more than one open editor, the scan will be added to the most recently-created one.)  By default, a new editor window is created.*/
	void openScanInEditor(AMScan* scan, bool bringEditorToFront = true, bool openInExistingEditor = false);

	/// Bring the Workflow view to the front.
	virtual void goToWorkflow();

	/// Opens a single scan configuration from a given database URL.  Reimplemented to put the scan into a config view holder to possibly add it to the workflow.
	virtual void launchScanConfigurationFromDb(const QUrl &url);

	/// Displays a dialog for changing the current run, if a user wants to do that while the app is still open.
	void showChooseRunDialog();

	/// Displays the view for the scanActions or brings it to the front
	void showScanActionsView();

	///////////////////////////////////

protected slots:
	/// Helper slot that builds a generic scan editor for the XAS scan.  \todo this seems like something that should be higher up in the framework.
	void onCurrentScanActionStarted(AMScanAction *action);
	/// Helper slot that handles disconnecting the current scan controller from the progress bar when it's done.
	void onCurrentScanActionFinished(AMScanAction *action);
	/// Slot that changes the state of the scanEditorModelItem state when the scan action state changes.
	void updateScanEditorModelItem();

	/// Handles the menu action for toggling whether we will automatically open new scans in editors
	void onActionAutomaticLaunchScanEditorToggled(bool toggled);

protected:
	/// Implementation method that individual applications can flesh out if extra setup is required when a scan action is started.  This is not pure virtual because there is no requirement to do anything to scan actions.
	virtual void onCurrentScanActionStartedImplementation(AMScanAction *action) { Q_UNUSED(action); }
	/// Implementation method that individual applications can flesh out if extra cleanup is required when a scan action finishes.  This is not pure virtual because there is no requirement to do anything to scan actions.
	virtual void onCurrentScanActionFinishedImplementation(AMScanAction *action) { Q_UNUSED(action); }

	/// Re-implementing the build bottom bar method to use the bottom bar built for the app controller that includes a mini workflow view.
	virtual void addBottomPanel();

	/// Filters the closeEvent on the main window, in case there's any reason why we can't quit directly. (ie: scans modified and still open, or an action is still running)
	virtual bool eventFilter(QObject *, QEvent *);

	/// Checks to make sure no actions are currently running in the AMActionRunner. If there are, asks user if they want to cancel them, but still returns false.
	bool canCloseActionRunner();

	/// Sets the flag that automatically brings new scan editors to the front on the stacked widget stack when the scan is running.
	void setAutomaticBringScanEditorToFront(bool flag) { automaticBringScanEditorToFrontWithRunningScans_ = flag; }
	/// Returns the flag that automatically brings new scan editors to the front on the stacked widget stack when the scan is running.
	bool automaticBringScanEditorToFrontWithRunningScans() const { return automaticBringScanEditorToFrontWithRunningScans_; }

	/// Sets the flag that automatically launches new scan editors
	void setAutomaticLaunchScanEditor(bool automaticLaunchScanEditor) { automaticLaunchScanEditor_ = automaticLaunchScanEditor; }
	/// Returns the flag that automatically launches new scan editors
	bool automaticLaunchScanEditor() const { return automaticLaunchScanEditor_; }

	/// Returns the facility of the application.
	virtual AMFacility facility() const = 0;
	/// Creates a persistent instance of AMAppController::facility to be used.
	void createFacility();
	/// Loads a run for initial startup.  Requires a valid, already persistent facility (i.e.: valid ID).
	void loadRun(const AMFacility &facility);

	/// Set whether the action runner cancel prompt should be shown.
	void setActionRunnerCancelPromptVisibility(bool showPrompt);

	/// Top-level panes in the main window
	AMWorkflowView3* workflowView_;
	AMWorkflowView3 *scanActionRunnerView_;
	/// Flag holding whether the AMGenericScanEditor's automatically are switched to when they have a running scan.  The default is true.
	bool automaticBringScanEditorToFrontWithRunningScans_;
	/// Flag hodling whether the scans will automatically get launched into an AMGenericScanEditor
	bool automaticLaunchScanEditor_;
};

#endif // AMAPPCONTROLLER_H
