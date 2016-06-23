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


#ifndef VESPERSSCANCONTROLLER_H
#define VESPERSSCANCONTROLLER_H

#include "actions3/AMAction3.h"

#include "dataman/AMScan.h"
#include "dataman/info/AMDetectorInfoSet.h"

#include "acquaman/CLS/CLSScanController.h"

#include "acquaman/VESPERS/VESPERSScanConfiguration.h"

/*!
	This class contains many small helper methods that are used commonly amongst all of VESPERS Dacq scan controllers.
	These are primarily adding in PV names to the dacq controller, adding non-configurable datasources and measurements,
	and some common initialization and clean-up actions.
  */
class VESPERSScanController: public CLSScanController
{

public:
	/// Constructor.
	VESPERSScanController(VESPERSScanConfiguration *configuration);
	/// Destructor.
	virtual ~VESPERSScanController();

protected:
	/// Helper method that builds the base initialization actions.
	AMAction3 *buildBaseInitializationAction(double firstRegionTime);
	/// Helper method that builds the CCD file path, name, and number for the beginning of a scan.  Requires the detector enum, ccd file name from the configuration and the new scan number, AND must be called after buildInitializationActions() because it assumes the list has already been created.
	AMAction3 *buildCCDInitializationAction(VESPERS::CCDDetectors ccdChoice, const QString &ccdName, int newScanNumber);
	/// Helper method that builds all of the cleanup actions.
	AMAction3 *buildCleanupAction();

	/// Helper method that checks the \param name provided for the CCD in \param path for uniqueness.  If it is unique it returns the provided string, otherwise it creates a unique "-xxx" to the end until a valid name is found.
	QString getUniqueCCDName(const QString &path, const QString &name) const;

	/// Helper method that builds the standard set of notes added to every dacq scan.
	QString buildNotes();

	/// Pointer to the configuration.
	VESPERSScanConfiguration *config_;
};

#endif // VESPERSSCANCONTROLLER_H
