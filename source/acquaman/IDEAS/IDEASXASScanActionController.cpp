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


#include "IDEASXASScanActionController.h"
#include "acquaman/AMScanActionControllerScanAssembler.h"

#include "dataman/AMXASScan.h"
#include "beamline/CLS/CLSSynchronizedDwellTime.h"
#include "beamline/IDEAS/IDEASBeamline.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "beamline/AMCurrentAmplifier.h"
#include "analysis/AM1DExpressionAB.h"
#include "analysis/AM2DSummingAB.h"
#include "application/IDEAS/IDEASAppController.h"
#include "application/AMAppControllerSupport.h"
#include "dataman/export/AMExporterAthena.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"
#include "actions3/actions/AMWaitAction.h"
#include "acquaman/AMEXAFSScanActionControllerAssembler.h"
#include "analysis/AM1DCalibrationAB.h"

IDEASXASScanActionController::IDEASXASScanActionController(IDEASXASScanConfiguration *configuration, QObject *parent) :
	AMStepScanActionController(configuration, parent)
{
	configuration_ = configuration;

	scan_ = new AMXASScan();
	scan_->setFileFormat("amCDFv1");
	scan_->setScanConfiguration(configuration);
	scan_->setIndexType("fileSystem");
	scan_->rawData()->addScanAxis(AMAxisInfo("eV", 0, "Incident Energy", "eV"));

    AMExporterOptionGeneralAscii *exporterOption = IDEAS::buildStandardExporterOption("IDEASXASDefault", false, true, true);

    if(exporterOption->id() > 0)
        AMAppControllerSupport::registerClass<IDEASXASScanConfiguration, AMExporterAthena, AMExporterOptionGeneralAscii>(exporterOption->id());

    exporterOption->deleteLater();

	QString scanName;

	if(configuration_->userScanName() == ""){

		scanName = configuration_->autoScanName();
		scan_->setName(QString("%1").arg(scanName));
	}

	else{

		scanName = configuration_->userScanName();
		scan_->setName(QString("%1").arg(scanName));
	}

	AMControlInfoList list;
	list.append(AMBeamline::bl()->exposedControlByName("Energy")->toInfo());

	configuration_->setAxisControlInfos(list);

	AMDetectorInfoSet ideasDetectors;
	ideasDetectors.addDetectorInfo(AMBeamline::bl()->exposedDetectorByName("I_0")->toInfo());
	ideasDetectors.addDetectorInfo(AMBeamline::bl()->exposedDetectorByName("Sample")->toInfo());
	ideasDetectors.addDetectorInfo(AMBeamline::bl()->exposedDetectorByName("Reference")->toInfo());

	if (configuration_->fluorescenceDetector().testFlag(IDEAS::Ketek)){

		ideasDetectors.addDetectorInfo(AMBeamline::bl()->exposedDetectorByName("KETEK")->toInfo());
		ideasDetectors.addDetectorInfo(AMBeamline::bl()->exposedDetectorByName("dwellTime")->toInfo());
	}

	else if (configuration_->fluorescenceDetector().testFlag(IDEAS::Ge13Element) && IDEASBeamline::ideas()->ge13Element()->isConnected()){

		ideasDetectors.addDetectorInfo(AMBeamline::bl()->exposedDetectorByName("13-el Ge")->toInfo());
		ideasDetectors.addDetectorInfo(AMBeamline::bl()->exposedDetectorByName("13E_dwellTime")->toInfo());
	}

	configuration_->setDetectorConfigurations(ideasDetectors);

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

IDEASXASScanActionController::~IDEASXASScanActionController(){}

void IDEASXASScanActionController::createScanAssembler()
{
	scanAssembler_ = new AMEXAFSScanActionControllerAssembler(this);
}

void IDEASXASScanActionController::buildScanControllerImplementation()
{
	QList<AMDataSource*> raw1DDataSources;

	for(int i = 0, size = scan_->rawDataSourceCount(); i < size; i++)
		if(scan_->rawDataSources()->at(i)->rank() == 1)
			raw1DDataSources << scan_->rawDataSources()->at(i);

	if(configuration_->usingTransmission()) {

		AM1DCalibrationAB* normSample = new AM1DCalibrationAB("NormSample");
		normSample->setDescription("NormSample");
		normSample->setInputDataSources(raw1DDataSources);
		normSample->setIsTransmission(true);
		normSample->setToEdgeJump(true);
		normSample->setDataName("Sample");
		normSample->setNormalizationName("I_0");
		scan_->addAnalyzedDataSource(normSample);
	 }

	if(configuration_->usingReference()) {

		AM1DCalibrationAB* normRef = new AM1DCalibrationAB("NormRef");
		normRef->setDescription("NormRef");
		normRef->setInputDataSources(raw1DDataSources);
		normRef->setIsTransmission(true);
		normRef->setToEdgeJump(true);
		normRef->setDataName("Reference");
		normRef->setNormalizationName("Sample");
		scan_->addAnalyzedDataSource(normRef);
	}

	AMXRFDetector *detector = 0;

	if (configuration_->fluorescenceDetector().testFlag(IDEAS::Ketek))
		detector = qobject_cast<AMXRFDetector *>(AMBeamline::bl()->exposedDetectorByName("KETEK"));

	else if (configuration_->fluorescenceDetector().testFlag(IDEAS::Ge13Element) && IDEASBeamline::ideas()->ge13Element()->isConnected())
		detector = qobject_cast<AMXRFDetector *>(AMBeamline::bl()->exposedDetectorByName("13-el Ge"));

	if (detector){

		detector->removeAllRegionsOfInterest();

		QString edgeSymbol = configuration_->edge().split(" ").first();
		QList<AMDataSource *> all1DDataSources;

		foreach (AMDataSource *dataSource, scan_->rawDataSources()->toList())
			if (dataSource->rank() == 1)
				all1DDataSources << dataSource;

		foreach (AMDataSource *dataSource, scan_->analyzedDataSources()->toList())
			if (dataSource->rank() == 1)
				all1DDataSources << dataSource;

		foreach (AMRegionOfInterest *region, configuration_->regionsOfInterest()){

			AMRegionOfInterestAB *regionAB = (AMRegionOfInterestAB *)region->valueSource();
			AMRegionOfInterestAB *newRegion = new AMRegionOfInterestAB(regionAB->name().remove(' '));
			newRegion->setBinningRange(regionAB->binningRange());
			newRegion->setInputDataSources(QList<AMDataSource *>() << scan_->dataSourceAt(scan_->indexOfDataSource(detector->name())));
			scan_->addAnalyzedDataSource(newRegion, false, true);
			detector->addRegionOfInterest(region);

			AM1DCalibrationAB* normalizedRegion = new AM1DCalibrationAB(QString("norm_%1").arg(newRegion->name()));
			normalizedRegion->setDescription(QString("Norm%1").arg(newRegion->name()));
			normalizedRegion->setInputDataSources(QList<AMDataSource *>() << newRegion << all1DDataSources);
			normalizedRegion->setToEdgeJump(true);
			normalizedRegion->setDataName(newRegion->name());
			normalizedRegion->setNormalizationName("I_0");
			scan_->addAnalyzedDataSource(normalizedRegion, newRegion->name().contains(edgeSymbol), !newRegion->name().contains(edgeSymbol));
		}
	}
}

AMAction3* IDEASXASScanActionController::createInitializationActions()
{
	AMListAction3 *initializationActions = new AMListAction3(new AMListActionInfo3("IDEAS XAS Initialization Stage 1", "IDEAS XAS Initialization Stage 1"), AMListAction3::Parallel);

	double startEnergy = double(configuration_->scanAxisAt(0)->regionAt(0)->regionStart());
	double mono2d = IDEASBeamline::ideas()->mono2d()->value();
	double braggAngle = asin(12398.4193 / mono2d / startEnergy);
	double backlashDegrees = 4;

	double deltaEnergy = (backlashDegrees / 180 * M_PI) * (mono2d * startEnergy * startEnergy * cos(braggAngle * M_PI / 180)) / (-12398.4193);
	double backlashEnergy = startEnergy + deltaEnergy;

	if(backlashEnergy < IDEASBeamline::ideas()->monoLowEV()->value())
		backlashEnergy = IDEASBeamline::ideas()->monoLowEV()->value();

	initializationActions->addSubAction(AMActionSupport::buildControlMoveAction(IDEASBeamline::ideas()->monoDirectEnergyControl(), backlashEnergy));
	initializationActions->addSubAction(IDEASBeamline::ideas()->scaler()->createContinuousEnableAction3(false));
	initializationActions->addSubAction(IDEASBeamline::ideas()->scaler()->createDwellTimeAction3(configuration_->scanAxisAt(0)->regionAt(0)->regionTime()));

	if (configuration_->fluorescenceDetector().testFlag(IDEAS::Ketek))
	{
		initializationActions->addSubAction(IDEASBeamline::ideas()->ketek()->createSetAcquisitionTimeAction(configuration_->scanAxisAt(0)->regionAt(0)->regionTime()));
	}

	return initializationActions;
}


void IDEASXASScanActionController::onInitializationActionsListSucceeded()
{
	AMControlInfoList positions(IDEASBeamline::ideas()->exposedControls()->toInfoList());
	positions.remove(positions.indexOf("DirectEnergy"));
	positions.remove(positions.indexOf("Energy"));
	positions.remove(positions.indexOf("XRF1E Real Time"));
	positions.remove(positions.indexOf("13-element Ge dwell time"));
	AMControlInfo I0Sensitivity("I0Sensitivity", IDEASBeamline::ideas()->scaler()->channelAt(0)->currentAmplifier()->value(), 0, 0, IDEASBeamline::ideas()->scaler()->channelAt(0)->currentAmplifier()->units(), 0.1, "I0 Amplifier Sensitivity");
	positions.append(I0Sensitivity);
	AMControlInfo sampleSensitivity("sampleSensitivity", IDEASBeamline::ideas()->scaler()->channelAt(1)->currentAmplifier()->value(), 0, 0, IDEASBeamline::ideas()->scaler()->channelAt(1)->currentAmplifier()->units(), 0.1, "Sample Amplifier Sensitivity");
	positions.append(sampleSensitivity);
	AMControlInfo referenceSensitivity("referenceSensitivity", IDEASBeamline::ideas()->scaler()->channelAt(2)->currentAmplifier()->value(), 0, 0, IDEASBeamline::ideas()->scaler()->channelAt(2)->currentAmplifier()->units(), 0.1, "Reference Amplifier Sensitivity");
	positions.append(referenceSensitivity);

	scan_->setScanInitialConditions(positions);
	AMScanActionController::onInitializationActionsListSucceeded();
}

AMAction3* IDEASXASScanActionController::createCleanupActions()
{
	AMListAction3 *cleanupActions = new AMListAction3(new AMListActionInfo3("IDEAS XAS Cleanup Actions", "IDEAS XAS Cleanup Actions"));

	cleanupActions->addSubAction(new AMWaitAction(new AMWaitActionInfo(IDEASBeamline::ideas()->scaler()->dwellTime())));
	cleanupActions->addSubAction(IDEASBeamline::ideas()->scaler()->createDwellTimeAction3(0.25));
	cleanupActions->addSubAction(new AMWaitAction(new AMWaitActionInfo(0.25)));
	cleanupActions->addSubAction(IDEASBeamline::ideas()->scaler()->createContinuousEnableAction3(true));

	return cleanupActions;
}

void IDEASXASScanActionController::onScanTimerUpdate()
{
	if (elapsedTime_.isActive()){

		if (secondsElapsed_ >= secondsTotal_)
			secondsElapsed_ = secondsTotal_;
		else
			secondsElapsed_ += 1.0;

		emit progress(secondsElapsed_, secondsTotal_);
	}
}
