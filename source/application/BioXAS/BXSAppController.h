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


#ifndef BXSAPPCONTROLLER_H
#define BXSAPPCONTROLLER_H

#include "application/AMAppController.h"

class BXSAppController : public AMAppController {
	Q_OBJECT

public:
	/// This constructor should be empty. Call ____AppController::startup() to create all of the application windows, widgets, and data objects that are needed on program startup.
	explicit BXSAppController(QObject* parent = 0);

	/// Destructor
	virtual ~BXSAppController() {}

	// Startup functions can be re-implemented from AMDatamanAppController / AMAppController. Some examples:
	///////////////////

//	/// Re-implement to create the Beamline object
//	virtual bool startupBeforeAnything();
//	/// Re-implement to register app-specific database classes
//	virtual bool startupRegisterDatabases();
//	/// Re-implement to add app-specific user interfaces
//	virtual bool startupCreateUserInterface();
//	/// Re-implement to do any last setup right before running
//	virtual bool startupAfterEverything();



//	/// Destroy all of the windows, widgets, and data objects created by applicationStartup(). Only call this if startup() has ran successfully.  If reimplementing, must call the base-class shutdown() as the last thing it does.
//	virtual void shutdown();

public slots:

protected slots:


protected:

	// Pointers to custom app objects that are created by the AppController:

};

#endif // BXSAPPCONTROLLER_H
