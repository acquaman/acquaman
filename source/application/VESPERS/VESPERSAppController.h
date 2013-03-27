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


#ifndef VESPERSAPPCONTROLLER_H
#define VESPERSAPPCONTROLLER_H

#include "application/AMAppController.h"
#include "application/VESPERS/VESPERS.h"

class VESPERSEndstationView;
class VESPERSXRFFreeRunView;
class XRFFreeRun;
class VESPERSCCDDetectorView;
class VESPERSPersistentView;
class VESPERSEXAFSScanConfiguration;
class VESPERSEXAFSScanConfigurationView;
class AMScanConfigurationViewHolder3;
class VESPERSScanConfigurationViewHolder3;
class VESPERS2DScanConfiguration;
class VESPERS2DScanConfigurationView;
class AM2DScanConfigurationViewHolder;
class AMBeamlineListAction;
class AMScanAction;
class VESPERSSpatialLineScanConfiguration;
class VESPERSSpatialLineScanConfigurationView;
class VESPERSEnergyScanConfiguration;
class VESPERSEnergyScanConfigurationView;
class AMGenericScanEditor;

class VESPERSAppController : public AMAppController {
	Q_OBJECT

public:
	/// This constructor should be empty. Call VESPERSAppController::startup() to create all of the application windows, widgets, and data objects that are needed on program startup.
	explicit VESPERSAppController(QObject* parent = 0);

	/// Destructor
	virtual ~VESPERSAppController() { cleanMoveImmediatelyAction(); }

	/// create and setup all of the application windows, widgets, communication connections, and data objects that are needed on program startup. Returns true on success.  If reimplementing, must call the base-class startup() as the first thing it does.
	virtual bool startup();

	/// destroy all of the windows, widgets, and data objects created by applicationStartup(). Only call this if startup() has ran successfully.  If reimplementing, must call the base-class shutdown() as the last thing it does.
	virtual void shutdown();

protected slots:
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
	/// Helper slot that handles checking out scans when they are added to a scan editor.  For now, all this does is choose which data source is visualized in AMSingleSpectrumView in AM2DScanView.
	void onScanAddedToEditor(AMGenericScanEditor *editor, AMScan *scan);

	/// Slot that handles success for moves using the moveImmediatelyAction.
	void onMoveImmediatelySuccess();
	/// Slot that handles the failure for moves using the moveImmediatelyAction.
	void onMoveImmediatelyFailure();

	/// Slot that handles changing the sample stage from pseudo motors to real motors.
	void onSampleStageChoiceChanged(bool change);

	/// Re-implemented from AMDatamanAppController.  Fixes CDF files that have been corrupted.
	virtual void fixCDF(const QUrl &url);

protected:
	/// Implementation method that individual applications can flesh out if extra setup is required when a scan action is started.  This is not pure virtual because there is no requirement to do anything to scan actions.
	virtual void onCurrentScanActionStartedImplementation(AMScanAction *action);
	/// Implementation method that individual applications can flesh out if extra cleanup is required when a scan action finishes.  This is not pure virtual because there is no requirement to do anything to scan actions.
	virtual void onCurrentScanActionFinishedImplementation(AMScanAction *action);

	/// Sets up a default XAS scan.  It will setup XANES or EXAFS based on the bool \param setupEXAFS using the information from AMGenericScanEditor \param editor.
	void setupXASScan(const AMGenericScanEditor *editor, bool setupEXAFS);
	/// Sets up a default energy scan.  It will setup the scan based on the information provided by AMGenericScanEditor.
	void setupEnergyScan(const AMGenericScanEditor *editor);
	/// Sets up a default 2D XRF scan.  It setup the 2D scan as best as it can based on the information provided by AMGenericScanEditor.
	void setup2DXRFScan(const AMGenericScanEditor *editor);
	/// Sets up and moves the motors based on the "Go to immediately" action from a 2D map.
	void moveImmediately(const AMGenericScanEditor *editor);
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

	/// XRF free run model for the single element detector.
	XRFFreeRun *xrf1ElFreeRun_;
	/// XRF free run view for the single element detector.
	VESPERSXRFFreeRunView *xrf1EFreeRunView_;
	/// XRF free run model for the four element detector.
	XRFFreeRun *xrf4ElFreeRun_;
	/// XRF free run view for the four element detector.
	VESPERSXRFFreeRunView *xrf4EFreeRunView_;
	/// Roper CCD detector view.
	VESPERSCCDDetectorView *roperCCDView_;
	/// Mar CCD detector view.
	VESPERSCCDDetectorView *marCCDView_;

	/// Pointer to the XAS scan configuration.
	VESPERSEXAFSScanConfiguration *exafsScanConfig_;
	/// Pointer to the XAS scan configuration view.
	VESPERSEXAFSScanConfigurationView *exafsConfigView_;
	/// The (new) holder for the XAS scan configuration.
	VESPERSScanConfigurationViewHolder3 *exafsConfigViewHolder3_;

	/// Pointer to the 2D scan configuration.
	VESPERS2DScanConfiguration *mapScanConfiguration_;
	/// The 2D scan configuration view.
	VESPERS2DScanConfigurationView *mapScanConfigurationView_;
	/// The (new) holder for the 2D scan configuration.
	AMScanConfigurationViewHolder3 *mapScanConfigurationViewHolder3_;

	/// The line scan configuration.
	VESPERSSpatialLineScanConfiguration *lineScanConfiguration_;
	/// The line scan configuration view.
	VESPERSSpatialLineScanConfigurationView *lineScanConfigurationView_;
	/// The (new holder for the line scan configuration.
	AMScanConfigurationViewHolder3 *lineScanConfigurationViewHolder3_;

	/// The energy scan configuration.
	VESPERSEnergyScanConfiguration *energyScanConfiguration_;
	/// The energy scan configuration view.
	VESPERSEnergyScanConfigurationView *energyScanConfigurationView_;
	/// The (new holder for the energy scan configuration.
	AMScanConfigurationViewHolder3 *energyScanConfigurationViewHolder3_;

	/// Pointer to the list action that is used to move the sample stage.
	AMBeamlineListAction *moveImmediatelyAction_;

	/// Pointer to the persistent view.
	VESPERSPersistentView *persistentView_;
	/// Pointer to the endstation view.
	VESPERSEndstationView *endstationView_;
};

#endif // VESPERSAPPCONTROLLER_H
