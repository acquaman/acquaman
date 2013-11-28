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


#ifndef AMSGMXASSCANCONTROLLER_H
#define AMSGMXASSCANCONTROLLER_H

#include "beamline/SGM/SGMBeamline.h"
#include "SGMXASScanConfiguration.h"
#include "dataman/AMXASScan.h"

#define SGMXASSCANCONTROLLER_CANNOT_ADD_MEASUREMENT 290201

/*
class SGMXASScanController
{
public:
	/// Standard constructor. Takes the scan configuration and readies a new scan object with the proper measurements and data sources for the detectors in use
	SGMXASScanController(SGMXASScanConfiguration *cfg);

	/// Returns whether or not the initialization actions and cleanup actions were created successfully
	bool isBeamlineInitialized();
	/// Creates the initialization actions and the cleanup actions, returns the value of beamlineInitialized_
	virtual bool beamlineInitialize();

protected:
	/// Holds the scan configuration pointer this controller is working with
	SGMXASScanConfiguration *config_;
	/// Holds the scan created here (specifically this will wind up being an AMXASScan)
	AMXASScan *specificScan_;

	/// Actions list for initialization actions (as Actions3 become more fully tested this needs to be converted over)
	AMBeamlineParallelActionsList *initializationActions_;
	/// Actions list for cleanup actions (as Actions3 become more fully tested this needs to be converted over)
	AMBeamlineParallelActionsList *cleanUpActions_;
	/// Bool holding true if the initialization actions and cleanup actions lists were created successfully
	bool beamlineInitialized_;
};
*/

#endif // AMSGMXASSCANCONTROLLER_H
