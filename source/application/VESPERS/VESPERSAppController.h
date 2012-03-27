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


#ifndef VESPERSAPPCONTROLLER_H
#define VESPERSAPPCONTROLLER_H

#include "application/AMAppController.h"

class VESPERSEndstationView;
class VESPERSXRFFreeRunView;
class VESPERSRoperCCDDetectorView;
class VESPERSPersistentView;
class VESPERSEXAFSScanConfiguration;
class AMScanConfigurationViewHolder;
class AMBeamlineListAction;

class AMGenericScanEditor;

class VESPERSWorkflowAssistant;

class VESPERSAppController : public AMAppController {
	Q_OBJECT

public:
	/// This constructor should be empty. Call VESPERSAppController::startup() to create all of the application windows, widgets, and data objects that are needed on program startup.
	explicit VESPERSAppController(QObject* parent = 0);

	/// Destructor
	virtual ~VESPERSAppController() {}

	/// create and setup all of the application windows, widgets, communication connections, and data objects that are needed on program startup. Returns true on success.  If reimplementing, must call the base-class startup() as the first thing it does.
	virtual bool startup();

	/// destroy all of the windows, widgets, and data objects created by applicationStartup(). Only call this if startup() has ran successfully.  If reimplementing, must call the base-class shutdown() as the last thing it does.
	virtual void shutdown();

protected slots:
	/// Helper slot that builds a generic scan editor for the XAS scan.  \todo this seems like something that should be higher up in the framework.
	void onCurrentScanControllerStarted();
	/// Helper slot that handles hooking up the progress bar from the bottom bar to the current scan controller progress.
	void onCurrentScanControllerCreated();
	/// Helper slot that handles disconnecting the current scan controller from the progress bar when it's done.
	void onCurrentScanControllerFinished();

	/// Helper slot that pauses scans after the beam has gone down.
	void onBeamDump();
	/// Helper slot that pauses scans using the bottom bar.
	void onPauseScanIssued();
	/// Helper slot that cancels the current scan.
	void onCancelScanIssued();

	/// Helper slot that handles the configureDetector signal from the 2D maps configuration view and goes to the right detector view.
	void onConfigureDetectorRequested(const QString &detector);
	/// Helper slot that pops up a menu to enable easy configuration of an XAS scan.  This slot is only used for 2D scans because AMGenericScanEditor only emits the necessary signal when using AM2DScanView.  The editor is passed so that the app controller knows of which (of the potentially many) scan editor to ask questions.
	void onDataPositionChanged(AMGenericScanEditor *editor, const QPoint &pos);
	/// Helper slot that connects generic scan editors that use the 2D scan view to the app controller so that it can enable quick configuration of scans.
	void onScanEditorCreated(AMGenericScanEditor *editor);

	/// Slot that handles success for moves using the moveImmediatelyAction.
	void onMoveImmediatelySuccess();
	/// Slot that handles the failure for moves using the moveImmediatelyAction.
	void onMoveImmediatelyFailure();

protected:
	/// Sets up a default XAS scan.  It will setup XANES or EXAFS based on the bool \param setupEXAFS using the information from AMGenericScanEditor \param editor.
	void setupXASScan(const AMGenericScanEditor *, bool setupEXAFS);
	/// Cleans up the moveImmediatelyAction after every move to ensure that the list action is always cleaned and is initialized for another move.
	void cleanMoveImmediatelyAction();

	// Things to do on startup.
	/// Registers all of the necessary classes that are VESPERS specific.
	void registerClasses();
	/// Sets up all of the exporter options for the various scan types.
	void setupExporterOptions();
	/// Sets up the user interface by specifying the extra pieces that will be added to the main window.
	void setupUserInterface();
	/// Sets up all of the connections.
	void makeConnections();

	/// Temporary workflow assistant.
	VESPERSWorkflowAssistant *assistant_;

	/// XRF free run view for the single element detector.
	VESPERSXRFFreeRunView *xrf1EFreeRunView_;
	/// XRF free run view for the four element detector.
	VESPERSXRFFreeRunView *xrf4EFreeRunView_;
	/// Roper CCD detector view.
	VESPERSRoperCCDDetectorView *roperCCDView_;

	/// Pointer to the XAS scan configuration.
	VESPERSEXAFSScanConfiguration *exafsScanConfig_;
	/// The holder for the XAS scan configuration.
	AMScanConfigurationViewHolder *exafsConfigViewHolder_;

	/// Pointer to the list action that is used to move the sample stage.
	AMBeamlineListAction *moveImmediatelyAction_;
};

#endif // VESPERSAPPCONTROLLER_H
