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

#include "application/CLS/CLSAppController.h"

class PGMPersistentView;

class PGMAppController : public CLSAppController
{
	Q_OBJECT

public:
	/// This constructor should be empty. Call IDEASAppController::startup() to create all of the application windows, widgets, and data objects that are needed on program startup.
	explicit PGMAppController(QObject* parent = 0);

	/// Destructor
	virtual ~PGMAppController() { }

	/// create and setup all of the application windows, widgets, communication connections, and data objects that are needed on program startup. Returns true on success.  If reimplementing, must call the base-class startup() as the first thing it does.
	virtual bool startup();

	/// destroy all of the windows, widgets, and data objects created by applicationStartup(). Only call this if startup() has ran successfully.  If reimplementing, must call the base-class shutdown() as the last thing it does.
	virtual void shutdown();

protected:
	// Things to do on startup.
	/// Initializes the beamline object.
	virtual void initializeBeamline();
	/// Registers all of the necessary classes that are VESPERS specific.
	void registerClasses();
	/// Sets up all of the exporter options for the various scan types.
	void setupExporterOptions();
	/// Initializes the user configuration.
	virtual void setupUserConfiguration();
	/// Sets up the user interface by specifying the extra pieces that will be added to the main window.
	void setupUserInterface();
	/// Sets up all of the connections.
	void makeConnections();

    /// Persistent view for PGM
    PGMPersistentView *pgmPersistentView_;

};

#endif // PGMAPPCONTROLLER_H
