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


#ifndef REIXSAPPCONTROLLER_H
#define REIXSAPPCONTROLLER_H

#include "application/CLS/CLSAppController.h"

class REIXSXESScanConfigurationDetailedView;
class AMScanConfigurationViewHolder3;
class REIXSXESSpectrometerControlPanel;
class REIXSXASScanConfiguration;
class AMGenericStepScanConfiguration;
class AMGenericStepScanConfigurationView;
class REIXSSampleChamberButtonPanel;
class REIXSSidebar;
/**
  * A class representing the main controller for the REIXS Acquaman
  * application. This class takes care of implementing those details which REIXS
  * requires (including user interface, class registrations and signal -> slot
  * connections) over and above the standard CLSAppController.
  */
class REIXSAppController : public CLSAppController {
	Q_OBJECT

public:
	 /**
	  * Creates an instance of the REIXSAppController. In order to fully initialize
	  * the app controller it is necessary to make a call to startup() - which will
	  * register classes, populate the user interface and make the required sinal
	  * -> slot connections.
	  */
	explicit REIXSAppController(QObject* parent = 0);

	 /**
	  * Virtual destructor ensures potential subclasses of REIXSAppController are
	  * properly destroyed.
	  */
	virtual ~REIXSAppController() {}


public slots:

protected slots:
	/**
	 * Helper slot that handles the workflow pausing/resuming when the beam dumps
	 * or is restored.
	 */
	void onBeamAvailabilityChanged(bool beamAvailable);

	/**
	 * Helper slot that connects generic scan editors that use the 2D scan view
	 * to the app controller so that it can enable quick configuration of scans.
	 */
	virtual void onScanEditorCreatedImplementation(AMGenericScanEditor *editor);

	/**
	 * Helper slot that handles checking out scans when they are added to a scan
	 * editor.  For now, all this does is choose which data source is visualized
	 * in AMSingleSpectrumView in AM2DScanView.
	 */
	void onScanAddedToEditor(AMGenericScanEditor *editor, AMScan *scan);

protected:
	/// Re-implementing the build bottom bar method to use the REIXS bottom bar with XES scan integration.
	virtual void addBottomPanel();

	/**
	 * Implementation method that specifies REIXS specific actions taken when a
	 * scan action is started
	 */
	virtual void onCurrentScanActionStartedImplementation(AMScanAction *action);

	/**
	 * Implementation method that specifies REIXS specific actions taken when a
	 * scan action is finished
	 */
	virtual void onCurrentScanActionFinishedImplementation(AMScanAction *action);

	// Things to do on startup:
	/// Sets up local and remote data paths.
	virtual bool setupDataFolder();
	/// Initializes the beamline object.
	virtual void initializeBeamline();
	/// Registers all of the necessary classes that are REIXS specific.
	virtual void registerDBClasses();
	/// Sets up all of the exporter options for the various scan types.
	virtual void registerExporterOptions();
	/// Sets up the available scan configurations.
	virtual void setupScanConfigurations();

	/// The customized implemention for each Beamline to set up the user interface
	virtual void setupUserInterfaceImplementation();
	/// create the persistent view
	virtual void createPersistentView();
	/// create pane for the general controls
	virtual void createGeneralPanes();
	/// create pane for the beamline detectors, such as xrf detectors
	virtual void createDetectorPanes();
	/// create pane for the scan configuration views
	virtual void createScanConfigurationPanes();
	/// create pane for the Experiment tools views
	void createExperimentToolPanes();

protected:
	REIXSXESScanConfigurationDetailedView* xesScanConfigurationView_;
	AMScanConfigurationViewHolder3* xesScanConfigurationViewHolder_;
	AMScanConfigurationViewHolder3* rixsScanConfigurationViewHolder_;
	AMScanConfigurationViewHolder3* xasScanConfigurationViewHolder_;

	/// The XAS scan configuration.
	REIXSXASScanConfiguration *xasScanConfiguration_;
	/// The generic scan configuration.
	AMGenericStepScanConfiguration *genericScanConfiguration_;
	/// The generic scan configuration view.
	AMGenericStepScanConfigurationView *genericScanConfigurationView_;
	/// The scan configuration view holder for the generic scan.
	AMScanConfigurationViewHolder3 *genericScanConfigurationViewHolder_;

	REIXSXESSpectrometerControlPanel* spectrometerPanel_;
	REIXSSampleChamberButtonPanel* sampleChamberButtonPanel_;
	/// The persistent view for the REIXS beamline.
	REIXSSidebar* sidebar_;
};

#endif // REIXSAPPCONTROLLER_H
