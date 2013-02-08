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


#ifndef SGMAPPCONTROLLER_H
#define SGMAPPCONTROLLER_H

#include "application/AMAppController.h"

class SGMSampleTransferView;
class AMSampleManagementWidget;
class CLSSIS3820ScalerView;
class CLSSynchronizedDwellTimeView;
class AMDetectorView;
class AMScanConfigurationViewHolder3;
class SGMXASScanConfigurationView;
class SGMFastScanConfigurationView;
class SGMSidebar;
class SGMSettingsMasterView;
class AMGithubManager;
class AMDetector;
class AMScanAction;
class CLSProcServManager;
class CLSProcServManagerView;
class SGMAdvancedControlsView;

#define SGMAPPCONTROLLER_COULD_NOT_RESET_FINISHED_SIGNAL 290301

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

	/// When the SGM Beamline object first connects the XAS and Fast scan panes are created. They are enabled or disabled as the connection status changes.
	void onSGMBeamlineConnected();
	/// When the SGM's scaler connects the view is created and added as a pane to the main window.
	void onSGMScalerConnected(bool connected);
	/// When the SGM's synchronized dwell time app connects the view is created and added as a pane to the main window
	void onSGMSynchronizedDwellTimeConnected(bool connected);
	/// When the SGM's PGT Sahara SDD connects the view is created and added as a pane to the main window.
	void onSGMPGTSDDConnected(bool connected);
	/// When the SGM's OceanOptics Optical Spectrometer connects the view is created and added as a pane to the main window
	void onSGMOceanOpticsSpectrometerConnected(bool connected);
	/// When the SGM's first amptek SDD connects the view is created and added as a pane to the main window.
	void onSGMAmptekSDD1Connected(bool connected);
	/// When the SGM's second amptek SDD connects the view is created and added as a pane to the main window.
	void onSGMAmptekSDD2Connected(bool connected);

	/// CURRENTLY UNUSED
	void onCurrentScanControllerCreated();
	/// CURRENTLY UNUSED
	void onCurrentScanControllerDestroyed();

	/// Creates the SGM settings view if necessary and shows it
	void onActionSGMSettings();
	/// Creates the SGM proc serv manager view if necessary and shows it
	void onActionProcServManager();

	/// Used during startup to display a list of detectors that the beamline is still looking for
	void onSGMBeamlineDetectorAvailabilityChanged(AMDetector *detector, bool isAvailable);

protected:
	/// When a scan starts in the Workflow3 system, a scan editor is opened and the default data source is set as the viewed source
	virtual void onCurrentScanActionStartedImplementation(AMScanAction *action);
	/// When a scan finishes in the Workflow3 system, the progress bar is disconnected
	virtual void onCurrentScanActionFinishedImplementation(AMScanAction *action);
	/// Installs the menu options for the settings manager and proc serv manager
	bool startupSGMInstallActions();
	/// Grabs the dacq configuration file locations
	bool setupSGMConfigurationFiles();
	/// Determines the plugin locations for file loaders
	bool setupSGMPlugins();
	/// Either creates, retrieves, or updates the exporter options for the provided and auto- export options
	bool setupSGMExporterOptions();
	/// Either creates or retrieves the SGM periodic table information
	bool setupSGMPeriodicTable();

	/// Creates views and makes connections. Some views have to wait for signals from the SGM Beamline object
	bool setupSGMViews();

protected:
	/// View to manage transfer of samples (NOT IN USE CURRENTLY)
	SGMSampleTransferView *sampleTransferView_;
	/// View to manage the sample positioner and the sample plates
	AMSampleManagementWidget *samplePositionView_;
	/// View for controlling the SGM scaler
	CLSSIS3820ScalerView *sgmScalerView_;
	/// View for controlling the synchronized dwell time application
	CLSSynchronizedDwellTimeView *sgmSynchronizedDwellTimeView_;
	/// View for controlling the PGT Sahara SDD
	AMDetectorView *pgtSDDView_;
	/// View for controlling the Ocean Optics Optical Spectrometer
	AMDetectorView *oceanOpticsSpectrometerView_;
	/// View for controlling the first SGM amptek SDD
	AMDetectorView *amptekSDD1View_;
	/// View for controlling the second SGM amptek SDD
	AMDetectorView *amptekSDD2View_;
	/// View for the SGM's XAS scan configurations
	SGMXASScanConfigurationView *xasScanConfigurationView_;
	/// View for the SGM's Fast scan configurations
	SGMFastScanConfigurationView *fastScanConfigurationView_;
	/// View holder for XAS
	AMScanConfigurationViewHolder3 *xasScanConfigurationHolder3_;
	/// View holder for Fast scans
	AMScanConfigurationViewHolder3 *fastScanConfigurationHolder3_;
	/// Persistent sidebar for beamline control
	SGMSidebar *sgmSidebar_;
	/// Pane for SGM's advanced controls
	SGMAdvancedControlsView *SGMAdvancedControls_;

	/// Persistent view for SGMSettings
	SGMSettingsMasterView *sgmSettingsMasterView_;

	/// List of procServs we might want to fiddle with
	QList<CLSProcServManager*> procServs_;
	/// View for the proc serv manager
	CLSProcServManagerView *procServsView_;

	/// Updating list of detectors we have been waiting for on startup
	QString lastWaitingDetectors_;

	/// Tracks whether a check has been done regarding bad startup settings
	bool checkedBadStartupSettings_;
};

#endif // SGMAPPCONTROLLER_H
