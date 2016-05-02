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


#ifndef BIOXASIMAGINGAPPCONTROLLER_H
#define BIOXASIMAGINGAPPCONTROLLER_H

#include "application/CLS/CLSAppController.h"

class BioXASImagingAppController : public CLSAppController
{
	Q_OBJECT

public:
	/// This constructor should be empty.  Call BioXASImagingAppController::start() to create the object.
	explicit BioXASImagingAppController(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASImagingAppController() {}

	/// create and setup all of the application windows, widgets, communication connections, and data objects that are needed on program startup. Returns true on success.  If reimplementing, must call the base-class startup() as the first thing it does.
	virtual bool startup();

	/// destroy all of the windows, widgets, and data objects created by applicationStartup(). Only call this if startup() has ran successfully.  If reimplementing, must call the base-class shutdown() as the last thing it does.
	virtual void shutdown();

protected:
	/// Implementation method that individual applications can flesh out if extra setup is required when a scan action is started.  This is not pure virtual because there is no requirement to do anything to scan actions.
	virtual void onCurrentScanActionStartedImplementation(AMScanAction *action);
	/// Implementation method that individual applications can flesh out if extra cleanup is required when a scan action finishes.  This is not pure virtual because there is no requirement to do anything to scan actions.
	virtual void onCurrentScanActionFinishedImplementation(AMScanAction *action);

	// Things to do on startup.
	/// Sets up local and remote data paths.
	virtual bool setupDataFolder();
	/// Initializes the beamline object.
	virtual void initializeBeamline();
	/// Registers all of the necessary classes that are VESPERS specific.
	virtual void registerClasses();
	/// Sets up all of the exporter options for the various scan types.
	virtual void setupExporterOptions();
	/// Initializes the user configuration.
	virtual void setupUserConfiguration();
	/// Sets up the user interface by specifying the extra pieces that will be added to the main window.
	virtual void setupUserInterface();
	/// Sets up all of the connections.
	virtual void makeConnections();

};

#endif // BIOXASIMAGINGAPPCONTROLLER_H
