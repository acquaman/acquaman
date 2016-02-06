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


#ifndef IDEASAPPCONTROLLER_H
#define IDEASAPPCONTROLLER_H

#include "application/AMAppController.h"

class AMScanConfigurationViewHolder3;
class IDEASXASScanConfiguration;
class IDEASXASScanConfigurationView;
class IDEASSampleCameraPanel;
class IDEASKETEKDetailedDetectorView;
class IDEAS13ElementGeDetailedDetectorView;
class IDEAS2DScanConfiguration;
class IDEAS2DScanConfigurationView;
class IDEASUserConfiguration;
class AMRegionOfInterest;
class AMGenericStepScanConfiguration;
class AMGenericStepScanConfigurationView;

class IDEASAppController : public AMAppController
{
	Q_OBJECT

public:
	/// This constructor should be empty. Call IDEASAppController::startup() to create all of the application windows, widgets, and data objects that are needed on program startup.
	explicit IDEASAppController(QObject* parent = 0);

	/// Destructor
	virtual ~IDEASAppController() { }

	/// create and setup all of the application windows, widgets, communication connections, and data objects that are needed on program startup. Returns true on success.  If reimplementing, must call the base-class startup() as the first thing it does.
	virtual bool startup();

	/// destroy all of the windows, widgets, and data objects created by applicationStartup(). Only call this if startup() has ran successfully.  If reimplementing, must call the base-class shutdown() as the last thing it does.
	virtual void shutdown();

protected slots:
	/// Helper slot that handles the workflow pausing/resuming when the beam dumps or is restored.
	void onBeamAvailabilityChanged(bool beamAvailable);

	/// Wait until the energy is connected before making the scan views
	void onEnergyConnected(bool connected);
	/// Wait until the Ge13Element Detector is connected before creating it's UI elements
	void onGe13Connected(bool connected);
	/// Helper slot that connects generic scan editors that use the 2D scan view to the app controller so that it can enable quick configuration of scans.
	void onScanEditorCreated(AMGenericScanEditor *editor);
	/// Helper slot that handles checking out scans when they are added to a scan editor.  For now, all this does is choose which data source is visualized in AMSingleSpectrumView in AM2DScanView.
	void onScanAddedToEditor(AMGenericScanEditor *editor, AMScan *scan);

	/// Handles setting up all the necessary settings based on the loaded user configuration.
	void onUserConfigurationLoadedFromDb();
	/// Handles adding regions of interest to all the configurations that would care.
	void onRegionOfInterestAdded(AMRegionOfInterest *region);
	/// Handles removing regions of interest from all the configurations that would care.
	void onRegionOfInterestRemoved(AMRegionOfInterest *region);
	/// Handles updating the regions of interest to all the configurations that would care.
	void onRegionOfInterestBoundingRangeChanged(AMRegionOfInterest *region);

protected:
	/// Implementation method that individual applications can flesh out if extra setup is required when a scan action is started.  This is not pure virtual because there is no requirement to do anything to scan actions.
	virtual void onCurrentScanActionStartedImplementation(AMScanAction *action);
	/// Implementation method that individual applications can flesh out if extra cleanup is required when a scan action finishes.  This is not pure virtual because there is no requirement to do anything to scan actions.
	virtual void onCurrentScanActionFinishedImplementation(AMScanAction *action);

	// Things to do on startup.
	/// Registers all of the necessary classes that are VESPERS specific.
	void registerClasses();
	/// Sets up all of the exporter options for the various scan types.
	void setupExporterOptions();
	/// Sets up the user interface by specifying the extra pieces that will be added to the main window.
	void setupUserInterface();
	/// Sets up all of the connections.
	void makeConnections();
	/// Method that finds the spectra data sources and then sets the generic scan editor single spectra viewer properly.
	void configureSingleSpectrumView(AMGenericScanEditor *editor, AMScan *scan);

	/// The configuration for XAS scans.
	IDEASXASScanConfiguration *xasScanConfiguration_;
	/// View for the IDEAS's XAS scan configurations
	IDEASXASScanConfigurationView *xasScanConfigurationView_;
	/// View holder for XAS
	AMScanConfigurationViewHolder3 *xasScanConfigurationHolder3_;

	/// The configuration for 2D scans.
	IDEAS2DScanConfiguration *mapScanConfiguration_;
	/// View for the IDEAS's 2D scan configurations.
	IDEAS2DScanConfigurationView *mapScanConfigurationView_;
	/// View holder for 2D scan configuration views.
	AMScanConfigurationViewHolder3 *mapScanConfigurationHolder3_;

	/// A generic scan configuration.
	AMGenericStepScanConfiguration *genericConfiguration_;
	/// A generic scan configuration view.
	AMGenericStepScanConfigurationView *genericConfigurationView_;
	/// The holder for the generic scan configuration view.
	AMScanConfigurationViewHolder3 *genericConfigurationViewHolder_;


	/// View for the IDEAS's Ketek XRF scan configurations
	IDEASKETEKDetailedDetectorView *ideasKETEKDetailedDetectorView_;
	/// View for the IDEAS's g13 Element scan configurations
	IDEAS13ElementGeDetailedDetectorView *ideas13ElementGeDetailedDetectorView_;
	/// View for viewing the sample cameras and aligning samples
	IDEASSampleCameraPanel *sampleCameraPanel_;

	/// The user configuration that holds some user specific parameters.
	IDEASUserConfiguration *userConfiguration_;
};

#endif // IDEASAPPCONTROLLER_H
