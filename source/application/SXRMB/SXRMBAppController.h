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


#ifndef SXRMBAPPCONTROLLER_H
#define SXRMBAPPCONTROLLER_H

#include "application/AMAppController.h"

class AMScanConfigurationViewHolder3;
class SXRMBPersistentView;
class SXRMBEXAFSScanConfiguration;
class SXRMBEXAFSScanConfigurationView;
class SXRMB2DMapScanConfiguration;
class SXRMB2DMapScanConfigurationView;
class SXRMB2DOxidationMapScanConfigurationView;
class CLSSIS3820ScalerView;
class SXRMBUserConfiguration;
class AMRegionOfInterest;
class AMListAction3;
class AMGenericScanEditor;
class AMScan;
class SXRMBOxidationMapScanConfigurationViewHolder;

class SXRMBAppController  : public AMAppController
{
	Q_OBJECT

public:
	/// This constructor should be empty.  Call SXRMBAppController::start() to create the object.
	explicit SXRMBAppController(QObject *parent = 0);
	/// Destructor.
	virtual ~SXRMBAppController() {}

	/// create and setup all of the application windows, widgets, communication connections, and data objects that are needed on program startup. Returns true on success.  If reimplementing, must call the base-class startup() as the first thing it does.
	virtual bool startup();

	/// destroy all of the windows, widgets, and data objects created by applicationStartup(). Only call this if startup() has ran successfully.  If reimplementing, must call the base-class shutdown() as the last thing it does.
	virtual void shutdown();

	/// Re-implemented from AMDatamanAppController to provide a menu action for Ambiant with gas chamber motor view.
	virtual bool startupInstallActions();

protected slots:
	/// slot to handle Beamline connected signal
	void onBeamlineConnected(bool);
	/// slot to handle Beamline control shutters timeout
	void onBeamControlShuttersTimeout();
	/// Helper slot that handles the workflow pausing/resuming when the beam dumps or is restored.
	void onBeamAvailabilityChanged(bool beamAvailable);

	/// Handles showing (or hiding) the scaler view when the scaler connects (disconnects)
	void onScalerConnected(bool isConnected);
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

	/// Handles setting up all the necessary settings based on the loaded user configuration.
	void onUserConfigurationLoadedFromDb();
	/// Handles adding regions of interest to all the configurations that would care.
	void onRegionOfInterestAdded(AMRegionOfInterest *region);
	/// Handles removing regions of interest from all the configurations that would care.
	void onRegionOfInterestRemoved(AMRegionOfInterest *region);

	/// Hanldes the action to show the sample stage motors for Ambiant with gas chamber endstation
	void onShowAmbiantSampleStageMotorsTriggered();

protected:
	/// Implementation method that individual applications can flesh out if extra setup is required when a scan action is started.  This is not pure virtual because there is no requirement to do anything to scan actions.
	virtual void onCurrentScanActionStartedImplementation(AMScanAction *action);
	/// Implementation method that individual applications can flesh out if extra cleanup is required when a scan action finishes.  This is not pure virtual because there is no requirement to do anything to scan actions.
	virtual void onCurrentScanActionFinishedImplementation(AMScanAction *action);

	/// Sets up a default XAS scan.  It will setup XANES or EXAFS based on the bool \param setupEXAFS using the information from AMGenericScanEditor \param editor.
	void setupXASScan(const AMGenericScanEditor *editor, bool setupEXAFS);
	/// Sets up a default 2D XRF scan.  It setup the 2D scan as best as it can based on the information provided by AMGenericScanEditor.
	void setup2DXRFScan(const AMGenericScanEditor *editor);
	/// Sets up and moves the motors based on the "Go to immediately" action from a 2D map.
	void moveImmediately(const AMGenericScanEditor *editor);
	/// Cleans up the moveImmediatelyAction after every move to ensure that the list action is always cleaned and is initialized for another move.
	void cleanMoveImmediatelyAction();
	/// Method that finds the spectra data sources and then sets the generic scan editor single spectra viewer properly.
	void configureSingleSpectrumView(AMGenericScanEditor *editor, AMScan *scan);

	// Things to do on startup.
	/// Registers all of the necessary classes that are VESPERS specific.
	void registerClasses();
	/// Check and set up the database for the first time run.
	void setupOnFirstRun();
	/// Sets up all of the exporter options for the various scan types.
	void setupExporterOptions();
	/// Sets up the user interface by specifying the extra pieces that will be added to the main window.
	void setupUserInterface();
	/// Sets up all of the connections.
	void makeConnections();

protected:
	/// Persistent sidebar for SXRMB
	SXRMBPersistentView *sxrmbPersistentView_;

	/// EXAFS scan configuration
	SXRMBEXAFSScanConfiguration* exafsScanConfiguration_;
	/// EXAFS scan configuration view
	SXRMBEXAFSScanConfigurationView* exafsScanConfigurationView_;
	/// EXAFS scan configuration view holder
	AMScanConfigurationViewHolder3* exafsScanConfigurationViewHolder_;

	/// 2D scan configuration
	SXRMB2DMapScanConfiguration* microProbe2DScanConfiguration_;
	/// 2D scan configuration view
	SXRMB2DMapScanConfigurationView* microProbe2DScanConfigurationView_;
	/// 2D scan configuration view holder
	AMScanConfigurationViewHolder3* microProbe2DScanConfigurationViewHolder_;

	/// 2D scan configuration
	SXRMB2DMapScanConfiguration* microProbe2DOxidationScanConfiguration_;
	/// 2D scan configuration view
	SXRMB2DOxidationMapScanConfigurationView* microProbe2DOxidationScanConfigurationView_;
	/// 2D scan configuration view holder
	SXRMBOxidationMapScanConfigurationViewHolder* microProbe2DOxidationScanConfigurationViewHolder_;

	/// The view for SXRMB's scaler
	CLSSIS3820ScalerView *scalerView_;

	/// Pointer to the list action that is used to move the sample stage.
	AMListAction3 *moveImmediatelyAction_;

	/// Holds the user configuration used for automatically setting up some simple aspects of the user interface.
	SXRMBUserConfiguration *userConfiguration_;
};

#endif // SXRMBAPPCONTROLLER_H
