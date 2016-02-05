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


#include "application/CLS/CLSAppController.h"

class CLSSIS3820ScalerView;
class CLSSynchronizedDwellTimeView;
class AMDetectorGeneralDetailedView;
class AMXRFDetailedDetectorView;

class AMScanAction;
class AMScanController;

class AMGenericStepScanConfiguration;
class AMGenericStepScanConfigurationView;
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

#define SGMAPPCONTROLLER_WRONG_USER 563321
#define SGMAPPCONTROLLER_COULD_NOT_RESET_FINISHED_SIGNAL 290301
#define SGMAPPCONTROLLER_COULD_NOT_CREATE_SGM_DATABASE 290302
#define SGMAPPCONTROLLER_COULD_NOT_CREATE_SGM_PUBLIC_DATABASE 290303
#define SGMAPPCONTROLLER_COULD_NOT_REGISTER_SGM_DATABASE 290304
#define SGMAPPCONTROLLER_COULD_NOT_REGISTER_SGM_PUBLIC_DATABASE 290305

class SGMAppController : public CLSAppController {
	Q_OBJECT

public:

	/*!
	  * Creates an instance of an SGMAppController.
	  */
	explicit SGMAppController(QObject* parent = 0);

	/*!
	  * Virtual desctructor for an SGMAppController.
	  */
	virtual ~SGMAppController() {}

	/*!
	  * Performs the initialization routine for all elements required by the
	  * SGMAppController.
	  * \returns True if startup was performed successfully, false otherwise.
	  */
	virtual bool startup();

	/*!
	  * Performs all the required cleanup operations for destroying the resources
	  * created and owned by the app controller.
	  */
	virtual void shutdown();

public slots:

protected slots:
	/*!
	  * Initializes the ui file for the amptek. Will only run once either on initialization
	  * of the app controller or, if not connected at app controller initialization,
	  * the first time the amptek is connected.
	  */
	void initializeAmptekView();

	/*!
	  * Handles a resize of the amptek detector view.
	  */
	void onAmptekDetectorViewResized();

	/*!
	  * Resizes the main window to its minimum size hint
	  */
	void resizeToMinimum();

	/// Handles setting up all the necessary settings based on the loaded user configuration.
	void onUserConfigurationLoadedFromDb();
	/// Handles adding regions of interest to all the configurations that would care.
	void onRegionOfInterestAdded(AMRegionOfInterest *region);
	/// Handles removing regions of interest from all the configurations that would care.
	void onRegionOfInterestRemoved(AMRegionOfInterest *region);

	/// helper function to initialize the AcquamanDataServer
	/// ideally, this should be called in super class when Acquaman Data server is a generalized feature for all BLs using Acquaman
	void connectAMDSServers();

	void onAMDSServerConnected(const QString &hostIdentifier);

protected:
	/// Initializes the beamline object.
	virtual void initializeBeamline();
	/// Registers all of the necessary DB classes that are SGM-specific.
	virtual void registerBeamlineDBClasses();
	/// Sets up all of the exporter options for the various scan types.
	virtual void setupBeamlineExporterOptions();
	/// Sets up the user interface by specifying the extra pieces that will be added to the main window.
	virtual void setupAcquamanUserInterface();
	/// Sets up all of the connections.
	virtual void setupBeamlineSignalConnections();

	/*!
	  * Initializes the Acquaman Data Server client app scontroller.
	  */
	void setupAMDSClientAppController();

	/*!
	  * Handles cases where a scan action has started in the Workflow3 system.
	  * Opens a scan editor and sets the default data source as the viewed source.
	  * \param action ~ The scan action which started.
	  */
	virtual void onCurrentScanActionStartedImplementation(AMScanAction *action);

	/*!
	  * Handles cases where a scan action has finished in the Workflow3 system.
	  * Disconnects the progress bar from updating the finished scan.
	  * \param action ~ The scan action which finished.
	  */
	virtual void onCurrentScanActionFinishedImplementation(AMScanAction *action);

	// Startup functions
	////////////////////

	/*!
	  * Registers all the SGM specific classes.
	  */
	void registerClasses();

	/*!
	  * Sets up all of the exporter options for the various scan types.
	  */
	void setupExporterOptions();

	/*!
	  * Adds the additional user interface elements specific to SGM to the
	  * application main window.
	  */
	void setupUserInterface();

	/*!
	  * Makes all the connections between the classes contained within the
	  * SGMAppController.
	  */
	void makeConnections();


	QMap<QString, AMDSServerConfiguration> AMDSServerDefs_;

	/// Commissioning step scan configuration.
	AMGenericStepScanConfiguration* commissioningStepConfiguration_;
	/// Commissioning step scan configuration view.
	AMGenericStepScanConfigurationView* commissioningStepConfigurationView_;
	/// Commissioning step scan configuration view holder.
	AMScanConfigurationViewHolder3* commissioningStepConfigurationViewHolder_;

	/// XAS continuous scan configuration.
	SGMXASScanConfiguration *xasScanConfiguration_;
	/// XAS continuous scan configuration view.
	SGMXASScanConfigurationView *xasScanConfigurationView_;
	/// XAS continuous scan configuration view holder.
	AMScanConfigurationViewHolder3 *xasScanConfigurationViewHolder_;

	/// Continuous line scan configuration.
	SGMLineScanConfiguration *lineScanConfiguration_;
	/// Continuous line scan configuration view.
	SGMLineScanConfigurationView *lineScanConfigurationView_;
	/// Continuous line scan configuration view holder.
	AMScanConfigurationViewHolder3 *lineScanConfigurationViewHolder_;

	/// Continuous map scan configuration.
	SGMMapScanConfiguration *mapScanConfiguration_;
	/// Continuous map scan configuration view.
	SGMMapScanConfigurationView *mapScanConfigurationView_;
	/// Continuous map scan configuration view holder.
	AMScanConfigurationViewHolder3 *mapScanConfigurationViewHolder_;

	CLSAmptekDetailedDetectorView* amptek1DetectorView_;
	CLSAmptekDetailedDetectorView* amptek2DetectorView_;
	CLSAmptekDetailedDetectorView* amptek3DetectorView_;
	CLSAmptekDetailedDetectorView* amptek4DetectorView_;

	AMDetectorGeneralDetailedView* qe65000DetectorView_;

	/// Holds the user configuration used for automatically setting up some simple aspects of the user interface.
	SGMUserConfiguration *userConfiguration_;
};

#endif // SGMAPPCONTROLLER_H
