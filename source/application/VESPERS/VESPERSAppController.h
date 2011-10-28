/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef VESPERSAPPCONTROLLER_H
#define VESPERSAPPCONTROLLER_H

#include "application/AMAppController.h"

class VESPERSEndstationView;
class VESPERSVESPERSXRFFreeRunView;
class VESPERSPersistentView;
class VESPERSXASScanConfigurationView;

class AMGenericScanEditor;

class VESPERSAppController : public AMAppController {
	Q_OBJECT

public:
	/// This constructor should be empty. Call VESPERSAppController::startup() to create all of the application windows, widgets, and data objects that are needed on program startup.
	explicit VESPERSAppController(QObject* parent = 0);

	/// Destructor
	virtual ~VESPERSAppController() {}

	/// create and setup all of the application windows, widgets, communication connections, and data objects that are needed on program startup. Returns true on success.  If reimplementing, must call the base-class startup() as the first thing it does.
	virtual bool startup();

	/// destroy all of the windows, widgets, and data objects created by applicationStartup(). Only call this if startup() has ran successfully.  If reimplementing, must call the base-class shutdown() as the last thing it does.
	virtual void shutdown();

protected slots:
	/// Helper slot that builds a generic scan editor for the XAS scan.  \todo this seems like something that should be higher up in the framework.
	void onCurrentScanControllerStarted();

protected:
	/// A pointer to the current XAS scan editor. \todo this seems like something that should be higher up in the framework.
	AMGenericScanEditor *scanControllerActiveEditor_;
};

#endif // VESPERSAPPCONTROLLER_H
