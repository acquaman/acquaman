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

#include "application/BioXAS/BioXASAppController.h"

class BioXASEndstationTableView;
class BioXASSidePersistentView;
class BioXASSideXASScanConfiguration;
class BioXASXASScanConfigurationView;
class AMScanConfigurationViewHolder3;
class AMRegionOfInterest;
class AMGenericStepScanConfiguration;
class AMGenericStepScanConfigurationView;
class BioXASSSRLMonochromatorEnergyControlCalibrationView;

class BioXASSideAppController : public BioXASAppController
{
	Q_OBJECT

public:
	/// This constructor should be empty.  Call BioXASSideAppController::start() to create the object.
	explicit BioXASSideAppController(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideAppController();

	/// create and setup all of the application windows, widgets, communication connections, and data objects that are needed on program startup. Returns true on success.  If reimplementing, must call the base-class startup() as the first thing it does.
	virtual bool startup();

	/// destroy all of the windows, widgets, and data objects created by applicationStartup(). Only call this if startup() has ran successfully.  If reimplementing, must call the base-class shutdown() as the last thing it does.
	virtual void shutdown();

protected slots:
	/// Handles adding regions of interest to all the configurations that would care.
	virtual void onRegionOfInterestAdded(AMRegionOfInterest *region);
	/// Handles removing regions of interest from all the configurations that would care.
	virtual void onRegionOfInterestRemoved(AMRegionOfInterest *region);

	/// Creates the scaler view and adds it to the 'Detectors' pane, if the scaler is connected and the view hasn't been created previously.
	void onScalerConnected();

	// Things to do on startup.
	/// Registers all of the necessary classes that are specific to this beamline.
	virtual void registerClasses();
	/// Sets up all of the exporter options for the various scan types.
	virtual void setupExporterOptions();
	/// Initializes the beamline object.
	virtual void initializeBeamline();
	/// Sets up the user interface by specifying the extra pieces that will be added to the main window.
	virtual void setupUserInterface();
	/// Sets up all of the connections.
	virtual void makeConnections();
	/// Applies the current settings.
	virtual void applyCurrentSettings();

protected:
	/// The scaler view.
	BioXASSIS3820ScalerView *scalerView_;
	/// The mono configuration view.
	BioXASSSRLMonochromatorConfigurationView *monoConfigView_;
	/// The endstation table view
	BioXASEndstationTableView *endstationTableView_;
	/// The JJ slit view
	CLSJJSlitsView *jjSlitsView_;
	/// The XIA filters view.
	BioXASXIAFiltersView *xiaFiltersView_;
	/// The carbon filter farm view.
	BioXASCarbonFilterFarmView *carbonFilterFarmView_;
	/// The m1 mirror view.
	BioXASM1MirrorView *m1MirrorView_;
	/// The m2 mirror view.
	BioXASM2MirrorView *m2MirrorView_;
	/// The dbhr mirror view.
	BioXASDBHRMirrorView *dbhrView_;
	/// The right-side persistent view.
	BioXASSidePersistentView *persistentPanel_;

	/// The XAS scan configuration.
	BioXASSideXASScanConfiguration *configuration_;
	/// The XAS scan configuration view.
	BioXASXASScanConfigurationView *configurationView_;
	/// The XAS scan configuration view holder.
	AMScanConfigurationViewHolder3 *configurationViewHolder_;

	/// The commissioning tool configuration.
	AMGenericStepScanConfiguration *commissioningConfiguration_;
	/// The commissioning tool configuration view.
	AMGenericStepScanConfigurationView *commissioningConfigurationView_;
	/// The commissioning tool configuration view holder.
	AMScanConfigurationViewHolder3 *commissioningConfigurationViewHolder_;

	/// The mono energy calibration configuration.
	BioXASSSRLMonochromatorEnergyCalibrationScanConfiguration *monoCalibrationConfiguration_;
	/// The mono energy calibration configuration view.
	BioXASXASScanConfigurationView *monoCalibrationConfigurationView_;
	/// The mono energy calibration configuration view holder.
	AMScanConfigurationViewHolder3 *monoCalibrationConfigurationViewHolder_;

	/// The mono energy calibration view.
	BioXASSSRLMonochromatorEnergyControlCalibrationView *monoEnergyCalibrationView_;

	/// Holds the user configuration used for automatically setting up some simple aspects of the user interface.
	BioXASUserConfiguration *userConfiguration_;
};

#endif // BIOXASSIDEAPPCONTROLLER_H
