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

class BioXASSidePersistentView;
class BioXASSideXASScanConfiguration;
class BioXASXASScanConfigurationView;
class AMScanConfigurationViewHolder3;
class AMRegionOfInterest;
class AMGenericStepScanConfiguration;
class AMGenericStepScanConfigurationView;

class BioXASSideAppController : public BioXASAppController
{
	Q_OBJECT

public:
	/// This constructor should be empty.  Call BioXASSideAppController::start() to create the object.
	explicit BioXASSideAppController(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideAppController();

	/// Create and setup all of the application windows, widgets, communication connections, and data objects that are needed on program startup. Returns true on success, false otherwise.
	virtual bool startup();

protected slots:
	/// Handles adding regions of interest to all the configurations that would care.
	virtual void onRegionOfInterestAdded(AMRegionOfInterest *region);
	/// Handles removing regions of interest from all the configurations that would care.
	virtual void onRegionOfInterestRemoved(AMRegionOfInterest *region);

	/// Implementation method that individual applications can flesh out if extra setup is required when a scan action is started.  This is not pure virtual because there is no requirement to do anything to scan actions.
	virtual void onCurrentScanActionStartedImplementation(AMScanAction *action);

protected:
	/// Registers all of the necessary classes that are specific to this beamline.
	virtual void registerClasses();
	/// Sets up all of the exporter options for the various scan types.
	virtual void setupExporterOptions();
	/// Initializes the beamline object.
	virtual void initializeBeamline();
	/// Sets up the user interface by specifying the extra pieces that will be added to the main window.
	virtual void setupUserInterface();
	/// Sets up local and remote data paths.
	virtual bool setupDataFolder();

protected:
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
};

#endif // BIOXASSIDEAPPCONTROLLER_H
