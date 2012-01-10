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


#include "REIXSXESScanConfiguration.h"

REIXSXESScanConfiguration::REIXSXESScanConfiguration(QObject *parent) :
	AMScanConfiguration(parent), mcpDetectorInfo_()
{
	setAutoExportEnabled(false);

	gratingNumber_ = 0;
	centerEV_ = 200;
	defocusDistanceMm_ = 0;
	spectrometerCalibrationId_ = -1;
	detectorTiltOffset_ = 0;
	// removed: detectorOrientation_ = 0;
	shouldStartFromCurrentPosition_ = false;
	doNotClearExistingCounts_ = false;

	maximumTotalCounts_ = 1000000;
	maximumDurationSeconds_ = 300;
}

// Returns a pointer to a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
AMScanConfiguration* REIXSXESScanConfiguration::createCopy() const {
	return new REIXSXESScanConfiguration(*this);	// can use the default auto-generated copy-constructor.
}

#include "acquaman/REIXS/REIXSXESScanController.h"

// Returns a pointer to a newly-created AMScanController that is appropriate for executing this scan configuration.
AMScanController* REIXSXESScanConfiguration::createController() {
	return new REIXSXESScanController(this);
}
