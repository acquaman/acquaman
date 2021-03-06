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


#include "VESPERSEnergyScanActionController.h"

#include "beamline/VESPERS/VESPERSBeamline.h"
#include "dataman/AMXASScan.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

VESPERSEnergyScanActionController::~VESPERSEnergyScanActionController(){}

VESPERSEnergyScanActionController::VESPERSEnergyScanActionController(VESPERSEnergyScanConfiguration *configuration, QObject *parent)
	: AMStepScanActionController(configuration, parent), VESPERSScanController(configuration)
{
	configuration_ = configuration;

	scan_ = new AMXASScan();
	scan_->setName(configuration_->name());
	scan_->setScanConfiguration(configuration_);
	scan_->setFileFormat("amCDFv1");
	scan_->setIndexType("fileSystem");
	scan_->setNotes(buildNotes());
	scan_->rawData()->addScanAxis(AMAxisInfo("eV", 0, "Incident Energy", "eV"));

	useFeedback_ = true;
	originalEnergy_ = VESPERSBeamline::vespers()->mono()->energy();

	AMControlInfoList list;
	list.append(VESPERSBeamline::vespers()->energy()->toInfo());
	configuration_->setAxisControlInfos(list);

	AMDetectorInfoSet detectors;
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SplitIonChamber")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("PreKBIonChamber")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("MiniIonChamber")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("PostIonChamber")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("EnergySetpoint")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("MasterDwellTime")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("RingCurrent")->toInfo());

	VESPERS::CCDDetectors ccdDetector = configuration_->ccdDetector();

	if (ccdDetector.testFlag(VESPERS::Roper))
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("RoperFileNumber")->toInfo());

	if (ccdDetector.testFlag(VESPERS::Mar))
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("MarFileNumber")->toInfo());

	if (ccdDetector.testFlag(VESPERS::Pilatus))
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("PilatusFileNumber")->toInfo());

	configuration_->setDetectorConfigurations(detectors);

	secondsElapsed_ = 0;
	secondsTotal_ = configuration_->totalTime();
	elapsedTime_.setInterval(1000);
	connect(this, SIGNAL(started()), &elapsedTime_, SLOT(start()));
	connect(this, SIGNAL(cancelled()), &elapsedTime_, SLOT(stop()));
	connect(this, SIGNAL(paused()), &elapsedTime_, SLOT(stop()));
	connect(this, SIGNAL(resumed()), &elapsedTime_, SLOT(start()));
	connect(this, SIGNAL(failed()), &elapsedTime_, SLOT(stop()));
	connect(this, SIGNAL(finished()), &elapsedTime_, SLOT(stop()));
	connect(&elapsedTime_, SIGNAL(timeout()), this, SLOT(onScanTimerUpdate()));
}

void VESPERSEnergyScanActionController::buildScanControllerImplementation()
{
}

AMAction3* VESPERSEnergyScanActionController::createInitializationActions()
{
	AMSequentialListAction3 *initializationActions = new AMSequentialListAction3(new AMSequentialListActionInfo3("Initialization List"));
	initializationActions->addSubAction(buildBaseInitializationAction(double(configuration_->scanAxisAt(0)->regionAt(0)->regionTime())));
	initializationActions->addSubAction(buildCCDInitializationAction(configuration_->ccdDetector(),
									 configuration_->ccdFileName(),
									 scan_->largestNumberInScansWhere(AMDatabase::database("user"), QString(" name = '%1'").arg(scan_->name()))+1));

	initializationActions->addSubAction(AMActionSupport::buildChangeToleranceAction(VESPERSBeamline::vespers()->energy(), double(configuration_->scanAxisAt(0)->regionAt(0)->regionEnd())*0.05));

	return initializationActions;
}

AMAction3* VESPERSEnergyScanActionController::createCleanupActions()
{
	AMListAction3 *cleanupAction = qobject_cast<AMListAction3 *>(buildCleanupAction());

	cleanupAction->addSubAction(VESPERSBeamline::vespers()->mono()->createEaAction(originalEnergy_));

	return cleanupAction;}

void VESPERSEnergyScanActionController::onScanTimerUpdate()
{
	if (elapsedTime_.isActive()){

		if (secondsElapsed_ >= secondsTotal_)
			secondsElapsed_ = secondsTotal_;
		else
			secondsElapsed_ += 1.0;

		emit progress(secondsElapsed_, secondsTotal_);
	}
}
