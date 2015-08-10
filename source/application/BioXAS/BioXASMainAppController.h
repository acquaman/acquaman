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


#ifndef BIOXASMAINAPPCONTROLLER_H
#define BIOXASMAINAPPCONTROLLER_H

#include "application/BioXAS/BioXASAppController.h"

class BioXASMainXASScanConfiguration;
class BioXASMainXASScanConfigurationView;
class BioXASMainPersistentView;

class BioXASMainAppController  : public BioXASAppController
{
	Q_OBJECT

public:
	/// This constructor should be empty.  Call BioXASMainAppController::start() to create the object.
	explicit BioXASMainAppController(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASMainAppController() {}

	/// create and setup all of the application windows, widgets, communication connections, and data objects that are needed on program startup. Returns true on success.  If reimplementing, must call the base-class startup() as the first thing it does.
	virtual bool startup();

protected:
	/// Implementation method that individual applications can flesh out if extra setup is required when a scan action is started.  This is not pure virtual because there is no requirement to do anything to scan actions.
	virtual void onCurrentScanActionStartedImplementation(AMScanAction *action);
	/// Implementation method that individual applications can flesh out if extra cleanup is required when a scan action finishes.  This is not pure virtual because there is no requirement to do anything to scan actions.
	virtual void onCurrentScanActionFinishedImplementation(AMScanAction *action);

	// Things to do on startup.
	/// Registers all of the necessary classes that are BioXAS Main specific.
	virtual void registerClasses();
	/// Sets up all of the exporter options for the various scan types.
	void setupExporterOptions();
	/// Sets up the user interface by specifying the extra pieces that will be added to the main window.
	void setupUserInterface();

	/// Creates an XAS scan configuration view for the given scan configuration and adds it to the 'Scans' main window pane.
	void addXASScanConfigurationView(BioXASMainXASScanConfiguration *configuration);

protected:
	/// XAS scan configuration.
	BioXASMainXASScanConfiguration *xasScanConfiguration_;
	/// Commissioning tool scan configuration.
	AMGenericStepScanConfiguration *commissioningScanConfiguration_;
};

#endif // BIOXASMAINAPPCONTROLLER_H
