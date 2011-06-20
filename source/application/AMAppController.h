/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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


#ifndef AMAPPCONTROLLER_H
#define AMAPPCONTROLLER_H

#include <QObject>

#include <application/AMDatamanAppController.h>

class AMWorkflowManagerView;
class AMMainWindow;

class AMAppController : public AMDatamanAppController
{
Q_OBJECT
public:
	/// This constructor is empty. Call AMAppController::startup() to create all of the application windows, widgets, and data objects that are needed on program startup.
	explicit AMAppController(QObject *parent = 0);
	/// This destructor automatically calls shutdown() if required. (ie: if startup() has run successfully, and shutdown() hasn't been called yet.)
	virtual ~AMAppController();

	/// create and setup all of the application windows, widgets, communication connections, and data objects that are needed on program startup. Returns true on success.  If reimplementing, must call the base-class startup() as the first thing it does.
	virtual bool startup();

	/// destroy all of the windows, widgets, and data objects created by applicationStartup(). Only call this if startup() has ran successfully.  If reimplementing, must call the base-class shutdown() as the last thing it does.
	virtual void shutdown();

signals:

public slots:

	// These slots are used to tell the app to activate a particular window/widget
	///////////////////////////////

	/// Bring the Workflow view to the front level
	void goToWorkflow();

	///////////////////////////////////

protected:
	/// Top-level panes in the main window
	AMWorkflowManagerView* workflowManagerView_;
};

#endif // AMAPPCONTROLLER_H
