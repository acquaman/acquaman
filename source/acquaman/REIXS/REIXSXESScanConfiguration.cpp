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


#include "REIXSXESScanConfiguration.h"

REIXSXESScanConfiguration::REIXSXESScanConfiguration(QObject *parent) :
	AMScanConfiguration(parent), mcpDetectorInfo_()
{
	setAutoExportEnabled(false);

	gratingNumber_ = 0;
	centerEV_ = 200;

	slitWidth_ = 25;
	energy_ = 400;

	defocusDistanceMm_ = 0;
	spectrometerCalibrationId_ = -1;
	detectorTiltOffset_ = 0;
	// removed: detectorOrientation_ = 0;
	shouldStartFromCurrentPosition_ = true;
	doNotClearExistingCounts_ = false;


	maximumTotalCounts_ = 1000000;
	maximumDurationSeconds_ = 300;

	scanNumber_ = 0;
	sampleId_ = -1;
	namedAutomatically_ = true;
}

// Returns a pointer to a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
AMScanConfiguration* REIXSXESScanConfiguration::createCopy() const {
	return new REIXSXESScanConfiguration(*this);	// can use the default auto-generated copy-constructor.
}

//#include "acquaman/REIXS/REIXSXESScanController.h"
#include "acquaman/REIXS/REIXSXESScanActionController.h"
// Returns a pointer to a newly-created AMScanController that is appropriate for executing this scan configuration.
AMScanController* REIXSXESScanConfiguration::createController() {
	//return new REIXSXESScanController(this);
	return new REIXSXESScanActionController(this);
}

QString REIXSXESScanConfiguration::description() const
{
	QString rv = QString("XES Scan");// at %1 eV  (%2 seconds or %3 counts)").arg(centerEV()).arg(maximumDurationSeconds()).arg(maximumTotalCounts(), 0, 'g', 0);
	if(applyEnergy()) rv.append(QString(" at %1 eV").arg(energy()));
//	if(defocusDistanceMm() != 0)
//		rv.append(QString(", Defocussed %1 mm").arg(defocusDistanceMm()));
//	if(detectorTiltOffset() != 0)
//		rv.append(QString(", Tilt offset %1 deg").arg(detectorTiltOffset()));

	return rv;
}

#include "ui/REIXS/REIXSXESScanConfigurationView.h"
AMScanConfigurationView * REIXSXESScanConfiguration::createView()
{
	return new REIXSXESScanConfigurationView(this);
}
