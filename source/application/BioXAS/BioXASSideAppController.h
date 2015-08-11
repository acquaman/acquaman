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

protected:
	/// Initializes the beamline object.
	virtual void initializeBeamline();
	/// Sets up the user interface by specifying the extra pieces that will be added to the main window.
	virtual void setupUserInterface();
	/// Sets up local and remote data paths.
	virtual bool setupDataFolder();

	/// Sets up an XAS scan configuration.
	virtual void setupXASScanConfiguration(BioXASXASScanConfiguration *configuration);
};

#endif // BIOXASSIDEAPPCONTROLLER_H
