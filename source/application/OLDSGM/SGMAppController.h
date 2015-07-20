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


#ifndef SGMAPPCONTROLLER_H
#define SGMAPPCONTROLLER_H

#include "application/AMAppController.h"

class CLSSIS3820ScalerView;
class CLSSynchronizedDwellTimeView;
class AMDetectorGeneralDetailedView;
class AMXRFDetailedDetectorView;

class AMScanAction;
class AMScanController;

class AMSampleManagementWidget;
class SGMSidebar;

class AMScanConfigurationViewHolder3;
class SGMXASScanConfiguration2013View;
class SGMFastScanConfiguration2013View;

class AMDetector;
class AMDetectorSelector;
class AMDetectorSelectorView;

class SGMSettingsMasterView;
class AMGithubManager;

class AMSampleCameraBrowserView;
class AMSamplePlateView;
class AMBeamlineSampleManagementView;
class SGMAdvancedControlsView;
class SGMAdvancedMirrorView;
class AMAction3;

#define SGMAPPCONTROLLER_COULD_NOT_RESET_FINISHED_SIGNAL 290301
#define SGMAPPCONTROLLER_COULD_NOT_CREATE_SGM_DATABASE 290302
#define SGMAPPCONTROLLER_COULD_NOT_CREATE_SGM_PUBLIC_DATABASE 290303
#define SGMAPPCONTROLLER_COULD_NOT_REGISTER_SGM_DATABASE 290304
#define SGMAPPCONTROLLER_COULD_NOT_REGISTER_SGM_PUBLIC_DATABASE 290305

class SGMAppController : public AMAppController {
	Q_OBJECT

public:
	/// This constructor is used to reset the finished signal and determine which database upgrades should be run.
	explicit SGMAppController(QObject* parent = 0);
	/// Destructor
	virtual ~SGMAppController() {}

	/// Create and setup all of the application windows, widgets, communication connections, and data objects that are needed on program startup. Returns true on success.  If reimplementing, must call the base-class startup() as the first thing it does.
	virtual bool startup();
	/// Destroy all of the windows, widgets, and data objects created by applicationStartup(). Only call this if startup() has ran successfully.  If reimplementing, must call the base-class shutdown() as the last thing it does.
	virtual void shutdown();

public slots:
	/// Reimplemented to call the parent and create the SGM beamline database if necessary (not already created)
	virtual bool startupCreateDatabases();

	/// Re-implemented from AMAppController to create and register the SGM database with the database system.
	virtual bool startupRegisterDatabases();
	/// Re-implemented to call the parent and setup the SGM configuration files, plugins, periodic table, and exporter options
	virtual bool startupPopulateNewDatabase();
	/// Re-implemented to call the parent and setup the SGM configuration files, plugins, periodic table, and exporter options
	virtual bool startupLoadFromExistingDatabase();
	/// Re-implemented to catch bad database directories and throw out a warning message
	virtual bool startupAfterEverything();

protected slots:
	/// This slot catches changes in the current widget of the AMMainWindow. \c pane is the new current widget.  Re-implement to catch any widget-specific responses that you need here.
	/*! \note This only applies to panes that are currently docked within the main window.  If a pane has been undocked, no notification will be received when it becomes raised or activated by the user.
	  */
	virtual void onCurrentPaneChanged(QWidget* pane);

	/// Helper slot that handles the workflow pausing/resuming when the beam dumps or is restored.
	void onBeamAvailabilityChanged(bool beamAvailable);
	/// When the SGM Beamline object first connects the XAS and Fast scan panes are created. They are enabled or disabled as the connection status changes.
	void onSGMBeamlineConnected();

	/// When the SGM's scaler connects the view is created and added as a pane to the main window.
	void onSGMScalerConnected(bool connected);
	/// When the SGM's synchronized dwell time app connects the view is created and added as a pane to the main window
	void onSGMSynchronizedDwellTimeConnected(bool connected);

	/// When the SGM's first (new) amptek SDD connects the view is created adn added as a pane to the main window.
	void onSGMNewAmptekSDD1Connected(bool connected);
	void onSGMNewAmptekSDD2Connected(bool connected);
	void onSGMNewAmptekSDD3Connected(bool connected);
	void onSGMNewAmptekSDD4Connected(bool connected);
	void onSGMNewAmptekSDD5Connected(bool connected);
	void onSGMNewPGTDetectorConnected(bool connected);
	void onSGMNewQE65000DetectorConnected(bool connected);
	void onSGMNewTEYDetectorConnected(bool connected);

	/// Creates the SGM settings view if necessary and shows it
	void onActionSGMSettings();

	void onAdvancedCameraOptionsRequested();
	/// Create the SGM Advanced mirror veiw if necessary and shows it
	void onActionMirrorVeiw();

	/// Used during startup to display a list of detectors that the beamline is still looking for
	void onSGMBeamlineDetectorAvailabilityChanged(AMDetector *detector, bool isAvailable);

	/// Handles listening to size changes from the XRFDetectorViews, which can expand the main window significantly
	void onXRFDetectorViewResized();
	/// Actually handles the resize for the above function some time later. Looks like one of the widgets or layouts takes quite a while to recalculate its minimumSizeHint() or its minimumSize()
	void resizeToMinimum();

	void resizeAfterStartup();

	void onWorkflowActionAddedFromDialog(AMAction3 *action);

protected:
	/// When a scan starts in the Workflow3 system, a scan editor is opened and the default data source is set as the viewed source
	virtual void onCurrentScanActionStartedImplementation(AMScanAction *action);
	/// When a scan finishes in the Workflow3 system, the progress bar is disconnected
	virtual void onCurrentScanActionFinishedImplementation(AMScanAction *action);
	/// Installs the menu options for the settings manager and proc serv manager
	bool startupSGMInstallActions();
	/// Determines the plugin locations for file loaders
	bool setupSGMPlugins();
	/// Either creates, retrieves, or updates the exporter options for the provided and auto- export options
	bool setupSGMExporterOptions();
	/// Either creates, retrieves, or updates the exporter options for XASScans
	bool setupSGMXASExporterOptions(AMDatabase* dbSGM);
	/// Either creates, retrieves, or updates the exporter options for FastScans
	bool setupSGMFastScanExporterOptions(AMDatabase* dbSGM);
	/// Either creates or retrieves the SGM periodic table information
	bool setupSGMPeriodicTable();

	/// Creates views and makes connections. Some views have to wait for signals from the SGM Beamline object
	bool setupSGMViews();

protected:
	AMSamplePlateView *samplePlateView_;
	AMBeamlineSampleManagementView *sampleManagementView_;

	/// View for controlling the SGM scaler
	CLSSIS3820ScalerView *sgmScalerView_;
	/// View for controlling the synchronized dwell time application
	CLSSynchronizedDwellTimeView *sgmSynchronizedDwellTimeView_;

	/// View for controlling the new SGM amptek SDD (first)
	AMXRFDetailedDetectorView *amptekSDD1XRFView_;
	AMXRFDetailedDetectorView *amptekSDD2XRFView_;
	AMXRFDetailedDetectorView *amptekSDD3XRFView_;
	AMXRFDetailedDetectorView *amptekSDD4XRFView_;
	AMXRFDetailedDetectorView *amptekSDD5XRFView_;
	AMDetectorGeneralDetailedView *newPGTDetectorView_;
	AMDetectorGeneralDetailedView *newQE65000DetectorView_;
	AMDetectorGeneralDetailedView *newTEYDetectorView_;

	/// View for the SGM's XAS scan configurations
	SGMXASScanConfiguration2013View *xasScanConfiguration2013View_;
	/// View for the SGM's Fast scan configurations
	SGMFastScanConfiguration2013View *fastScanConfiguration2013View_;
	AMDetectorSelector *xasDetectorSelector_;
	AMDetectorSelector *fastDetectorSelector_;
	/// View holder for XAS
	AMScanConfigurationViewHolder3 *xasScanConfiguration2013Holder3_;
	/// View holder for Fast scans
	AMScanConfigurationViewHolder3 *fastScanConfiguration2013Holder3_;
	/// Persistent sidebar for beamline control
	SGMSidebar *sgmSidebar_;
	/// Pane for SGM's advanced controls
	SGMAdvancedControlsView *SGMAdvancedControls_;
	/// Window for SGM mirrors
	SGMAdvancedMirrorView *SGMAdvancedMirror_;

	/// Persistent view for SGMSettings
	SGMSettingsMasterView *sgmSettingsMasterView_;

	/// Updating list of detectors we have been waiting for on startup
	QString lastWaitingDetectors_;

	/// Tracks whether a check has been done regarding bad startup settings
	bool checkedBadStartupSettings_;
};

#endif // SGMAPPCONTROLLER_H
