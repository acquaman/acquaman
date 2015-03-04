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


#ifndef VESPERSTIMESCANACTIONCONTROLLER_H
#define VESPERSTIMESCANACTIONCONTROLLER_H

#include "acquaman/AMTimedScanActionController.h"
#include "acquaman/VESPERS/VESPERSTimeScanConfiguration.h"
#include "acquaman/VESPERS/VESPERSScanController.h"

class VESPERSTimeScanActionController : public AMTimedScanActionController, public VESPERSScanController
{
	Q_OBJECT

public:
	VESPERSTimeScanActionController(VESPERSTimeScanConfiguration *configuration, QObject *parent = 0);
	virtual ~VESPERSTimeScanActionController(){}

protected:
	/// Reimplemented to provide actions that will setup the beamline for optimized operation of the XAS scan.
	AMAction3* createInitializationActions();
	/// Reimplemented to put the beamline in a good state after a scan has been completed.
	AMAction3* createCleanupActions();

	/// Adds anything extra (eg: analysis blocks) to the scan before it's started.
	virtual void buildScanControllerImplementation(){}

	/// Specific scan configuration with all the VESPERS specific information inside.
	VESPERSTimeScanConfiguration *configuration_;
};

#endif // VESPERSTIMESCANACTIONCONTROLLER_H
