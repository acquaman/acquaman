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


#ifndef VESPERSAPPCONTROLLER_H
#define VESPERSAPPCONTROLLER_H

#include "application/CLS/CLSAppController.h"
#include "application/VESPERS/VESPERS.h"

class AMScanConfigurationViewHolder3;
class AMScanAction;
class AMGenericScanEditor;
class AMListAction3;
class AMRegionOfInterest;

class VESPERSEndstationView;
class VESPERSDeviceStatusView;
class VESPERSCCDDetectorView;
class VESPERSPilatusCCDDetectorView;
class VESPERSPersistentView;
class VESPERSEXAFSScanConfiguration;
class VESPERSEXAFSScanConfigurationView;
class VESPERS2DScanConfiguration;
class VESPERS2DScanConfigurationView;
class VESPERSSpatialLineScanConfiguration;
class VESPERSSpatialLineScanConfigurationView;
class VESPERSEnergyScanConfiguration;
class VESPERSEnergyScanConfigurationView;
class VESPERS3DScanConfiguration;
class VESPERS3DScanConfigurationView;
class VESPERSUserConfiguration;
class VESPERSTimeScanConfiguration;
class VESPERSTimeScanConfigurationView;
class VESPERSTimedLineScanConfiguration;
class VESPERSTimedLineScanConfigurationView;

#define VESPERSAPPCONTROLLER_COULD_NOT_CREATE_VESPERS_FOLDER 999000
#define VESPERSAPPCONTROLLER_AURORA_PATH_NOT_FOUND 999001
#define VESPERSAPPCONTROLLER_PILATUS_PATH_NOT_FOUND 999002

class VESPERSAppController : public CLSAppController {
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
	/// Helper slot that handles the workflow pausing/resuming when the beam dumps or is restored.
	void onBeamAvailabilityChanged(bool beamAvailable = false);

	/// Helper slot that handles moving the status page to the front of the main window stack.
	void onStatusViewRequrested();
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
	void onSampleStageChoiceChanged(const QString &name);

	/// Handles setting the path for the Roper CCD when it is connected.
	void onRoperCCDConnected(bool connected);
	/// Handles setting the path for the Mar CCD when it is connected.
	void onMarCCDConnected(bool connected);
	/// Handles setting the path for the Pilatus CCD when it is connected.
	void onPilatusCCDConnected(bool connected);

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
	/// Method that finds the spectra data sources and then sets the generic scan editor single spectra viewer properly.
	void configureSingleSpectrumView(AMGenericScanEditor *editor, AMScan *scan);

	// Methods that know what is acceptable for which scan type.
	/// Returns whether the given motor is acceptable for EXAFS.
	bool exafsMotorAcceptable(int motor) const;
	/// Returns whether the given motor is acceptable for energy scans.
	bool energyScanMotorAcceptable(int motor) const;
	/// Returns whether the given motor is acceptable for 2D maps.
	bool mapMotorAcceptable(int motor) const;
	/// Returns whether the given motor is acceptable for line scans.
	bool lineScanMotorAcceptable(int motor) const;
	/// Converts a given combination motor group into an acceptable motor enum for the line scan.  Returns only the "horizontal" component.
	int convertSampleStageMotorToIndividualMotor(int motor) const;

	// Things to do on startup.
	/// Initializes the beamline object.
	virtual void initializeBeamline();
	/// Registers all of the necessary classes that are VESPERS specific.
	virtual void registerBeamlineDBClasses();
	/// Sets up all of the exporter options for the various scan types.
	virtual void setupExporterOptions();
	/// Ensures that all the necessary directories exist before they are used and create errors.
	bool ensureProgramStructure();
	/// Sets up the user interface by specifying the extra pieces that will be added to the main window.
	void setupUserInterface();
	/// Sets up all of the connections.
	void makeConnections();

	/// Roper CCD detector view.
	VESPERSCCDDetectorView *roperCCDView_;
	/// Mar CCD detector view.
	VESPERSCCDDetectorView *marCCDView_;
	/// Pilatus CCD detector view.
	VESPERSPilatusCCDDetectorView *pilatusView_;

	/// Pointer to the XAS scan configuration.
	VESPERSEXAFSScanConfiguration *exafsScanConfiguration_;
	/// Pointer to the XAS scan configuration view.
	VESPERSEXAFSScanConfigurationView *exafsConfigurationView_;
	/// The (new) holder for the XAS scan configuration.
	AMScanConfigurationViewHolder3 *exafsConfigurationViewHolder3_;

	/// Pointer to the 2D scan configuration.
	VESPERS2DScanConfiguration *mapScanConfiguration_;
	/// The 2D scan configuration view.
	VESPERS2DScanConfigurationView *mapScanConfigurationView_;
	/// The (new) holder for the 2D scan configuration.
	AMScanConfigurationViewHolder3 *mapScanConfigurationViewHolder3_;

	/// Pointer to the 3D scan configuration.
	VESPERS3DScanConfiguration *map3DScanConfiguration_;
	/// The 3D scan configuration view.
	VESPERS3DScanConfigurationView *map3DScanConfigurationView_;
	/// The (new) holder for the 3D scan configuration.
	AMScanConfigurationViewHolder3 *map3DScanConfigurationViewHolder3_;

	/// The line scan configuration.
	VESPERSSpatialLineScanConfiguration *lineScanConfiguration_;
	/// The line scan configuration view.
	VESPERSSpatialLineScanConfigurationView *lineScanConfigurationView_;
	/// The (new) holder for the line scan configuration.
	AMScanConfigurationViewHolder3 *lineScanConfigurationViewHolder3_;

	/// The energy scan configuration.
	VESPERSEnergyScanConfiguration *energyScanConfiguration_;
	/// The energy scan configuration view.
	VESPERSEnergyScanConfigurationView *energyScanConfigurationView_;
	/// The (new) holder for the energy scan configuration.
	AMScanConfigurationViewHolder3 *energyScanConfigurationViewHolder3_;

	/// The time scan configuration.
	VESPERSTimeScanConfiguration *timeScanConfiguration_;
	/// The time scan configuration view.
	VESPERSTimeScanConfigurationView *timeScanConfigurationView_;
	/// The (new) holder for the time scan configuration.
	AMScanConfigurationViewHolder3 *timeScanConfigurationViewHolder3_;

	/// The timed line scan configuration.
	VESPERSTimedLineScanConfiguration *timedLineScanConfiguration_;
	/// The timed line scan configuration view.
	VESPERSTimedLineScanConfigurationView *timedLineScanConfigurationView_;
	/// The (new) holder for the timed line scan configuration.
	AMScanConfigurationViewHolder3 *timedLineScanConfigurationViewHolder3_;

	/// Pointer to the list action that is used to move the sample stage.
	AMListAction3 *moveImmediatelyAction_;

	/// Pointer to the persistent view.
	VESPERSPersistentView *persistentView_;
	/// Pointer to the endstation view.
	VESPERSEndstationView *endstationView_;
	/// Pointer to the status view.
	VESPERSDeviceStatusView *statusPage_;

	/// Flag for holding the startup flag for the Roper CCD.  This is false until the roper is connected.
	bool roperCCDStartup_;
	/// Flag for holding the startup flag for the Mar CCD.  This is false until the roper is connected.
	bool marCCDStartup_;
	/// Flag for holding the startup flag for the Pilatus CCD.  This is false until the roper is connected.
	bool pilatusCCDStartup_;

	/// Holds the user configuration used for automatically setting up some simple aspects of the user interface.
	VESPERSUserConfiguration *userConfiguration_;
};

#endif // VESPERSAPPCONTROLLER_H
