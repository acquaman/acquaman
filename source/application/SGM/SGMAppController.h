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

#define SGMAPPCONTROLLER_WRONG_USER 563321

#include "application/AMAppController.h"

class AMGenericStepScanConfiguration;
class AMGenericStepScanConfigurationView;
class AMScanConfigurationViewHolder3;
class CLSAmptekDetailedDetectorView;
class SGMLaddersView;
class SGMXASScanConfiguration;
class SGMXASScanConfigurationView;
class SGMLineScanConfiguration;
class SGMLineScanConfigurationView;

/// acquaman data server
#include "source/appController/AMDSClientAppController.h"
#include "source/Connection/AMDSServer.h"
#include "source/DataElement/AMDSConfigurationDef.h"
/*!
  * A class which acts as the central application for SGM Acquaman. Holds the
  * main window which is displayed to users, as well as performs the application
  * startup procedures.
  */
class SGMAppController : public AMAppController
{
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

	/// helper function to initialize the AcquamanDataServer
	/// ideally, this should be called in super class when Acquaman Data server is a generalized feature for all BLs using Acquaman
	void connectAMDSServers();

	void onAMDSServerConnected(const QString &hostIdentifier);
protected:

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

	/// XAS continuous scan configuration.
	SGMLineScanConfiguration *lineScanConfiguration_;
	/// XAS continuous scan configuration view.
	SGMLineScanConfigurationView *lineScanConfigurationView_;
	/// XAS continuous scan configuration view holder.
	AMScanConfigurationViewHolder3 *lineScanConfigurationViewHolder_;

	CLSAmptekDetailedDetectorView* amptek1DetectorView_;
	CLSAmptekDetailedDetectorView* amptek2DetectorView_;
	CLSAmptekDetailedDetectorView* amptek3DetectorView_;
	CLSAmptekDetailedDetectorView* amptek4DetectorView_;
};

#endif // SGMAPPCONTROLLER_H
