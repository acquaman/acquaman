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

/// acquaman data server
#include "source/appController/AMDSClientAppController.h"
#include "source/Connection/AMDSServer.h"
#include "source/DataElement/AMDSConfigurationDef.h"

class AMGenericStepScanConfiguration;
class AMRegionOfInterest;
class AMGenericStepScanConfigurationView;
class AMScanConfigurationViewHolder3;
class AMDetectorGeneralDetailedView;

class CLSAmptekDetailedDetectorView;

class SGMXASScanConfiguration;
class SGMXASScanConfigurationView;
class SGMLineScanConfiguration;
class SGMLineScanConfigurationView;
class SGMMapScanConfiguration;
class SGMMapScanConfigurationView;

/*!
  * A class which acts as the central application for SGM Acquaman. Holds the
  * main window which is displayed to users, as well as performs the application
  * startup procedures.
  */
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

protected slots:
	/*!
	  * Initializes the ui file for the amptek. Will only run once either on initialization
	  * of the app controller or, if not connected at app controller initialization,
	  * the first time the amptek is connected.
	  */
	void onInitializeAmptekView();

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

	/// slot to handle the connected signal of the AMDS server
	void onAMDSServerConnected(const QString &hostIdentifier);

protected:
	// Things to do on startup.
	/// Sets up local and remote data paths.
	virtual bool setupDataFolder();
	/// Initializes the beamline object.
	virtual void initializeBeamline();
	/// Registers all of the necessary DB classes that are SGM-specific.
	virtual void registerDBClasses();
	/// Sets up all of the exporter options for the various scan types.
	virtual void registerExporterOptions();
	/// Sets up the available scan configurations.
	virtual void setupScanConfigurations();
	/// Sets up the user configuration.
	virtual void setupUserConfiguration();

	/// create the persistent view
	virtual void createPersistentView();
	/// create pane for the general controls
	virtual void createGeneralPanes();
	/// create pane for the beamline detectors, such as xrf detectors
	virtual void createDetectorPanes();
	/// create pane for the scan configuration views
	virtual void createScanConfigurationPanes();

	/*!
	  * Initializes the Acquaman Data Server client app controller.
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

protected:
	/// a map of the definitions of the Acquaman Data Server
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

//	/// Holds the user configuration used for automatically setting up some simple aspects of the user interface.
//	SGMUserConfiguration *sgmUserConfiguration_;
};

#endif // SGMAPPCONTROLLER_H
