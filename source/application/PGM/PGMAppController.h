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


#ifndef PGMAPPCONTROLLER_H
#define PGMAPPCONTROLLER_H

#include "dataman/PGM/PGMUserConfiguration.h"
#include "application/CLS/CLSAppController.h"

class AMControl;
class AMScanConfigurationViewHolder3;
class CLSBeamlineStatusView;
class PGMPersistentView;
class PGMXASScanConfiguration;
class PGMXASScanConfigurationView;

class PGMAppController : public CLSAppController
{
	Q_OBJECT

public:
	/// This constructor should be empty. Call IDEASAppController::startup() to create all of the application windows, widgets, and data objects that are needed on program startup.
	explicit PGMAppController(QObject* parent = 0);

	/// Destructor
	virtual ~PGMAppController() { }

protected slots:
	/// Sets the beam status view as the current view, with the given control as the selected control.
	void goToBeamlineStatusView(AMControl *control);

	/// Handles setting up all the necessary settings based on the loaded user configuration.
	void onUserConfigurationLoadedFromDb();

	/// Handles adding regions of interest to all the configurations that would care.
	virtual void onRegionOfInterestAdded(AMRegionOfInterest *region);
	/// Handles removing regions of interest from all the configurations that would care.
	virtual void onRegionOfInterestRemoved(AMRegionOfInterest *region);
	/// Handles updating the regions of interest to all the configurations that would care.
	virtual void onRegionOfInterestBoundingRangeChanged(AMRegionOfInterest *region);

protected:
	// Things to do on startup.
	/// Sets up local and remote data paths.
	virtual bool setupDataFolder();
	/// Initializes the beamline object.
	virtual void initializeBeamline();
	/// Registers all of the necessary classes that are VESPERS specific.
	void registerDBClasses();
	/// Sets up all of the exporter options for the various scan types.
	void registerExporterOptions();
	/// Sets up the available scan configurations.
	virtual void setupScanConfigurations();
	/// Initializes the user configuration.
	virtual void setupUserConfiguration();

	/// Helper slot that connects generic scan editors that use the 2D scan view to the app controller so that it can enable quick configuration of scans.
	virtual void onScanEditorCreatedImplementation(AMGenericScanEditor *editor);

	/// The customized implemention for each Beamline to set up the user interface
	void setupUserInterfaceImplementation();
	/// create the persistent view
	virtual void createPersistentView();
	/// create pane for the general controls
	virtual void createGeneralPanes();
	/// create pane for the beamline detectors, such as xrf detectors
	virtual void createDetectorPanes();
	/// create pane for the scan configuration views
	virtual void createScanConfigurationPanes();

protected:
	/// Persistent view for PGM
	PGMPersistentView *pgmPersistentView_;

	/// the beamline status view
	CLSBeamlineStatusView *beamlineStatusView_;

	/// Returns true if the list of regions of interest contains the given ROI.
	bool containsRegionOfInterest(QList<AMRegionOfInterest*> roiList, AMRegionOfInterest *regionOfInterest) const;

protected:
	/// The user configuration.
	PGMUserConfiguration *pgmUserConfiguration_;

	/// Pointer to the XAS scan configuration.
	PGMXASScanConfiguration *xasScanConfiguration_;
	/// Pointer to the XAS scan configuration view.
	PGMXASScanConfigurationView *xasScanConfigurationView_;
	/// The (new) holder for the XAS scan configuration.
	AMScanConfigurationViewHolder3 *xasScanConfigurationViewHolder3_;
};

#endif // PGMAPPCONTROLLER_H
