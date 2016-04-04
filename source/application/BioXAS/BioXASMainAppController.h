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

class BioXASMainAppController  : public BioXASAppController
{
	Q_OBJECT

public:
	/// This constructor should be empty.  Call BioXASMainAppController::start() to create the object.
	explicit BioXASMainAppController(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASMainAppController();

	/// Create and setup all of the application windows, widgets, communication connections, and data objects that are needed on program startup. Returns true on success, false otherwise.
	virtual bool startup();

protected:
	/// Initializes the beamline object.
	virtual void initializeBeamline();
	/// Sets up the user interface by specifying the extra pieces that will be added to the main window.
	virtual void setupUserInterface();
	/// Sets up local and remote data paths.
	virtual bool setupDataFolder();
};

#endif // BIOXASMAINAPPCONTROLLER_H
