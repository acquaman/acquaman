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


#include "REIXSXESScanConfiguration.h"
#include "beamline/REIXS/REIXSBeamline.h"


 REIXSXESScanConfiguration::~REIXSXESScanConfiguration(){}
REIXSXESScanConfiguration::REIXSXESScanConfiguration(QObject *parent) :
	AMScanConfiguration(parent), mcpDetectorInfo_()
{
	gratingNumber_ = 0;

	slitWidth_ = 25;
	applySlitWidth_ = false;
	energy_ = 400;
	applyEnergy_ = false;

	polarization_ = 1;
	polarizationAngle_ = 0;
	applyPolarization_ = false;


	defocusDistanceMm_ = 0;
	spectrometerCalibrationId_ = -1;
	detectorTiltOffset_ = 0;

	shouldStartFromCurrentPosition_ = true;
	doNotClearExistingCounts_ = false;


	maximumTotalCounts_ = 1000000;
	maximumDurationSeconds_ = 300;
	totalTime_ = 0;

	userScanName_ = "XES";
	scanNumber_ = 0;
	sampleId_ = -1;
	namedAutomatically_ = true;
}

REIXSXESScanConfiguration::REIXSXESScanConfiguration(const REIXSXESScanConfiguration &original)
	: AMScanConfiguration()
{
	mcpDetectorInfo_ = *(original.mcpDetectorInfo());

	gratingNumber_ = original.gratingNumber();

	slitWidth_ = original.slitWidth();
	applySlitWidth_ = original.applySlitWidth();
	energy_ = original.energy();
	applyEnergy_ = original.applyEnergy();

	polarization_ = original.polarization();
	polarizationAngle_ = original.polarizationAngle();
	applyPolarization_ = original.applyPolarization();


	defocusDistanceMm_ = original.defocusDistanceMm();
	spectrometerCalibrationId_ = original.spectrometerCalibrationId();
	detectorTiltOffset_ = original.detectorTiltOffset();

	shouldStartFromCurrentPosition_ = original.shouldStartFromCurrentPosition();
	doNotClearExistingCounts_ = original.doNotClearExistingCounts();


	maximumTotalCounts_ = original.maximumTotalCounts();
	maximumDurationSeconds_ = original.maximumDurationSeconds();
	totalTime_ = 0;


	userScanName_ = original.userScanName();
	scanNumber_ = original.scanNumber();
	sampleId_ = original.sampleId();
	namedAutomatically_ = original.namedAutomatically();
}

// Returns a pointer to a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
AMScanConfiguration* REIXSXESScanConfiguration::createCopy() const
{
	AMScanConfiguration *configuration = new REIXSXESScanConfiguration(*this);
	configuration->dissociateFromDb(true);
	return configuration;
}

//#include "acquaman/REIXS/REIXSXESScanController.h"
#include "acquaman/REIXS/REIXSXESScanActionController.h"
// Returns a pointer to a newly-created AMScanController that is appropriate for executing this scan configuration.
AMScanController* REIXSXESScanConfiguration::createController() {
	AMScanActionController *controller = new REIXSXESScanActionController(this);
	controller->buildScanController();

	return controller;
}

QString REIXSXESScanConfiguration::description() const
{
	QString rv = QString("XES Scan");
	if(applyEnergy())
		rv.append(QString(" at %1 eV").arg(energy()));

	return rv;
}

#include "ui/REIXS/REIXSXESScanConfigurationView.h"
AMScanConfigurationView * REIXSXESScanConfiguration::createView()
{
	return new REIXSXESScanConfigurationView(this);
}

void REIXSXESScanConfiguration::computeTotalTimeImplementation()
{
	double averageCountRate =  REIXSBeamline::bl()->mcpDetector()->countsPerSecond();
	double estimatedTime = maximumTotalCounts_ / averageCountRate;

	double scanLength = REIXSBeamline::bl()->mcpDetector()->acquisitionTime();

	totalTime_ = qMin(double(scanLength), estimatedTime);

	setExpectedDuration(totalTime_);
	emit totalTimeChanged(totalTime_);

}
