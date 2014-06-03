/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef REIXSAPPCONTROLLER_H
#define REIXSAPPCONTROLLER_H

#include "application/AMAppController.h"

class REIXSXESScanConfigurationDetailedView;
class AMWorkflowView;

class REIXSAppController : public AMAppController {
	Q_OBJECT

public:
	/// This constructor should be empty. Call REIXSAppController::startup() to create all of the application windows, widgets, and data objects that are needed on program startup.
	explicit REIXSAppController(QObject* parent = 0);

	/// Destructor
	virtual ~REIXSAppController() {}

	/// Re-implemented to add REIXS specific Info, Actions and Editors
	virtual bool startup();
	/// Re-implemented to create the REIXSBeamline object
	virtual bool startupBeforeAnything();
	/// Re-implemented to register REIXS-specific database classes
	virtual bool startupRegisterDatabases();
	/// Re-implemented to add REIXS-specific user interfaces
	virtual bool startupCreateUserInterface();
	virtual bool startupAfterEverything();


	/// destroy all of the windows, widgets, and data objects created by applicationStartup(). Only call this if startup() has ran successfully.  If reimplementing, must call the base-class shutdown() as the last thing it does.
	virtual void shutdown();


	///	Opens a single scan configuration from a given database URL.  Reimplemented to put the scan into a config view holder to possibly add it to the workflow.
	virtual void launchScanConfigurationFromDb(const QUrl &url);


public slots:

protected slots:


protected:

	REIXSXESScanConfigurationDetailedView* xesScanConfigurationView_;

};

#endif // REIXSAPPCONTROLLER_H
