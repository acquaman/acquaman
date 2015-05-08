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


#ifndef BIOXASSIDEAPPCONTROLLER_H
#define BIOXASSIDEAPPCONTROLLER_H

#include "application/AMAppController.h"

class QGroupBox;
class QWidget;
class BioXASSIS3820ScalerView;
class CLSJJSlitsView;
class BioXASSidePersistentView;
class BioXASSideXASScanConfiguration;
class BioXASSideXASScanConfigurationView;
class AMScanConfigurationViewHolder3;
class BioXASSSRLMonochromatorConfigurationView;
class BioXASUserConfiguration;
class AMRegionOfInterest;
class BioXASXIAFiltersView;
class BioXASCarbonFilterFarmView;
class BioXASM2MirrorView;
class BioXASDBHRMirrorView;
class AMGenericStepScanConfiguration;
class AMGenericStepScanConfigurationView;

class BioXASSideAppController : public AMAppController
{
	Q_OBJECT

public:
	/// This constructor should be empty.  Call BioXASSideAppController::start() to create the object.
	explicit BioXASSideAppController(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideAppController() {}

	/// create and setup all of the application windows, widgets, communication connections, and data objects that are needed on program startup. Returns true on success.  If reimplementing, must call the base-class startup() as the first thing it does.
	virtual bool startup();

	/// destroy all of the windows, widgets, and data objects created by applicationStartup(). Only call this if startup() has ran successfully.  If reimplementing, must call the base-class shutdown() as the last thing it does.
	virtual void shutdown();

protected slots:
	/// Creates the scaler view and adds it to the 'Detectors' pane, if the scaler is connected and the view hasn't been created previously.
	void onScalerConnected();

	/// Handles setting up all the necessary settings based on the loaded user configuration.
	void onUserConfigurationLoadedFromDb();
	/// Handles adding regions of interest to all the configurations that would care.
	void onRegionOfInterestAdded(AMRegionOfInterest *region);
	/// Handles removing regions of interest from all the configurations that would care.
	void onRegionOfInterestRemoved(AMRegionOfInterest *region);

protected:
	/// Implementation method that individual applications can flesh out if extra setup is required when a scan action is started.  This is not pure virtual because there is no requirement to do anything to scan actions.
	virtual void onCurrentScanActionStartedImplementation(AMScanAction *action);
	/// Implementation method that individual applications can flesh out if extra cleanup is required when a scan action finishes.  This is not pure virtual because there is no requirement to do anything to scan actions.
	virtual void onCurrentScanActionFinishedImplementation(AMScanAction *action);

	// Things to do on startup.
	/// Registers all of the necessary classes that are specific to this beamline.
	void registerClasses();
	/// Sets up all of the exporter options for the various scan types.
	void setupExporterOptions();
	/// Sets up the user interface by specifying the extra pieces that will be added to the main window.
	void setupUserInterface();
	/// Sets up all of the connections.
	void makeConnections();
	/// Applies the current settings.
	void applyCurrentSettings();

	/// create squeeze groupbox layout
	QGroupBox *createSqueezeGroupBoxWithView(QString title, QWidget *view);

protected:
	/// The scaler view.
	BioXASSIS3820ScalerView *scalerView_;
	/// The mono configuration view.
	BioXASSSRLMonochromatorConfigurationView *monoConfigView_;
	/// The JJ slit view
	CLSJJSlitsView *jjSlitsView_;
	/// The XIA filters view.
	BioXASXIAFiltersView *xiaFiltersView_;
	/// The carbon filter farm view.
	BioXASCarbonFilterFarmView *carbonFilterFarmView_;
	/// The m2 mirror view.
	BioXASM2MirrorView *m2MirrorView_;
	/// The dbhr mirror view.
	BioXASDBHRMirrorView *dbhrView_;
	/// The right-side persistent view.
	BioXASSidePersistentView *persistentPanel_;

	/// The XAS scan configuration.
	BioXASSideXASScanConfiguration *configuration_;
	/// The XAS scan configuration view.
	BioXASSideXASScanConfigurationView *configurationView_;
	/// The XAS scan configuration view holder.
	AMScanConfigurationViewHolder3 *configurationViewHolder_;

	/// The commissioning tool configuration.
	AMGenericStepScanConfiguration *commissioningConfiguration_;
	/// The commissioning tool configuration view.
	AMGenericStepScanConfigurationView *commissioningConfigurationView_;
	/// The commissioning tool configuration view holder.
	AMScanConfigurationViewHolder3 *commissioningConfigurationViewHolder_;

	/// Holds the user configuration used for automatically setting up some simple aspects of the user interface.
	BioXASUserConfiguration *userConfiguration_;
};

#endif // BIOXASSIDEAPPCONTROLLER_H
