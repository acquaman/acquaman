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
#include "analysis/AM1DExpressionAB.h"
#include "analysis/AM2DSummingAB.h"
#include "application/IDEAS/IDEASAppController.h"

#include "actions3/AMListAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "actions3/actions/AMWaitAction.h"
#include "acquaman/AMEXAFSScanActionControllerAssembler.h"


#include <QDebug>

IDEASXASScanActionController::IDEASXASScanActionController(IDEASXASScanConfiguration *cfg, QObject *parent) :
	AMStepScanActionController(cfg, parent)
{
	configuration_ = cfg;

	scan_ = new AMXASScan();
	scan_->setFileFormat("amRegionAscii2013");
	scan_->setScanConfiguration(cfg);
	scan_->setIndexType("fileSystem");
	scan_->rawData()->addScanAxis(AMAxisInfo("eV", 0, "Incident Energy", "eV"));

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
	list.append(IDEASBeamline::ideas()->monoEnergyControl()->toInfo());
	configuration_->setAxisControlInfos(list);

	AMDetectorInfoSet ideasDetectors;
	ideasDetectors.addDetectorInfo(IDEASBeamline::ideas()->I_0()->toInfo());
	ideasDetectors.addDetectorInfo(IDEASBeamline::ideas()->Sample()->toInfo());
	ideasDetectors.addDetectorInfo(IDEASBeamline::ideas()->Reference()->toInfo());

//	if (configuration_->fluorescenceDetector().testFlag(IDEASXASScanConfiguration::Ketek)){

//		ideasDetectors.addDetectorInfo(IDEASBeamline::ideas()->exposedDetectorByName("KETEK")->toInfo());
//		ideasDetectors.addDetectorInfo(IDEASBeamline::ideas()->ketekDwellTime()->toInfo());
//	}

//	else if (configuration_->fluorescenceDetector().testFlag(IDEASXASScanConfiguration::Ge13Element)){

//		ideasDetectors.addDetectorInfo(IDEASBeamline::ideas()->exposedDetectorByName("13-el Ge")->toInfo());
//		ideasDetectors.addDetectorInfo(IDEASBeamline::ideas()->ketekDwellTime()->toInfo());
//	}

	if (configuration_->fluorescenceDetector() == IDEASXASScanConfiguration::Ketek){

		ideasDetectors.addDetectorInfo(IDEASBeamline::ideas()->exposedDetectorByName("KETEK")->toInfo());
		ideasDetectors.addDetectorInfo(IDEASBeamline::ideas()->ketekDwellTime()->toInfo());
	}

	else if (configuration_->fluorescenceDetector() == IDEASXASScanConfiguration::Ge13Element){

		ideasDetectors.addDetectorInfo(IDEASBeamline::ideas()->exposedDetectorByName("13-el Ge")->toInfo());
		ideasDetectors.addDetectorInfo(IDEASBeamline::ideas()->ge13ElementDwellTime()->toInfo();
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
	AMXRFDetector *detector = 0;

//	if (configuration_->fluorescenceDetector().testFlag(IDEASXASScanConfiguration::Ketek))
//		detector = qobject_cast<AMXRFDetector *>(IDEASBeamline::bl()->exposedDetectorByName("KETEK"));

//	else if (configuration_->fluorescenceDetector().testFlag(IDEASXASScanConfiguration::Ge13Element))
//		detector = qobject_cast<AMXRFDetector *>(IDEASBeamline::bl()->exposedDetectorByName("13-el Ge"));

	if (configuration_->fluorescenceDetector() == IDEASXASScanConfiguration::Ketek)
		detector = qobject_cast<AMXRFDetector *>(IDEASBeamline::bl()->exposedDetectorByName("KETEK"));

	else if (configuration_->fluorescenceDetector() == IDEASXASScanConfiguration::Ge13Element)
		detector = qobject_cast<AMXRFDetector *>(IDEASBeamline::bl()->exposedDetectorByName("13-el Ge"));

	QList<AMDataSource*> raw1DDataSources;

	for(int i = 0, size = scan_->rawDataSourceCount(); i < size; i++)
		if(scan_->rawDataSources()->at(i)->rank() == 1)
			raw1DDataSources << scan_->rawDataSources()->at(i);

	int rawI0Index = scan_->rawDataSources()->indexOfKey("I_0");
	int rawIsampleIndex = scan_->rawDataSources()->indexOfKey("Sample");
	int rawIrefIndex = scan_->rawDataSources()->indexOfKey("Reference");

	if (detector && configuration_->isXRFScan()){

		foreach (AMRegionOfInterest *region, detector->regionsOfInterest()){

			AMRegionOfInterestAB *regionAB = (AMRegionOfInterestAB *)region->valueSource();
			AMRegionOfInterestAB *newRegion = new AMRegionOfInterestAB(regionAB->name().replace(" ","_"));
			newRegion->setBinningRange(regionAB->binningRange());
			newRegion->setInputDataSources(QList<AMDataSource *>() << scan_->dataSourceAt(scan_->indexOfDataSource(detector->name())));
			scan_->addAnalyzedDataSource(newRegion);
		}
	}

	if(rawI0Index != -1 && rawIsampleIndex != -1 && configuration_->isTransScan()) {

		AM1DExpressionAB* NormSample = new AM1DExpressionAB("NormSample");
		NormSample->setDescription("NormSample");
		NormSample->setInputDataSources(raw1DDataSources);
		NormSample->setExpression("ln(I_0/Sample)");
		scan_->addAnalyzedDataSource(NormSample);
	 }

	if(rawIrefIndex != -1 && rawIsampleIndex != -1 && configuration_->isTransScan() && configuration_->useRef()) {

		AM1DExpressionAB* NormRef = new AM1DExpressionAB("NormRef");
		NormRef->setDescription("NormRef");
		NormRef->setInputDataSources(raw1DDataSources);
		NormRef->setExpression("ln(Sample/Reference)");
		scan_->addAnalyzedDataSource(NormRef);
	}

	QList<AMDataSource*> raw2DDataSources;
	for(int i=0; i<scan_->rawDataSources()->count(); i++)
		if(scan_->rawDataSources()->at(i)->rank() == 2)
			raw2DDataSources << scan_->rawDataSources()->at(i);

	QList<AMDataSource*> all1DDataSources;
	for(int i=0; i<scan_->analyzedDataSources()->count(); i++)
		if(scan_->analyzedDataSources()->at(i)->rank() == 1)
			all1DDataSources << scan_->analyzedDataSources()->at(i);
	for(int i=0; i<scan_->rawDataSources()->count(); i++)
		if(scan_->rawDataSources()->at(i)->rank() == 1)
			all1DDataSources << scan_->rawDataSources()->at(i);

	if (detector && configuration_->isXRFScan()){

		foreach (AMRegionOfInterest *region, detector->regionsOfInterest()){
			AMRegionOfInterestAB *regionAB = (AMRegionOfInterestAB *)region->valueSource();
			QString regionName = regionAB->name().replace(" ","_");
			AM1DExpressionAB* NormXRF = new AM1DExpressionAB(QString("Norm%1").arg(regionName));
			NormXRF->setDescription(QString("Norm%1").arg(regionName));
			NormXRF->setInputDataSources(all1DDataSources);
			NormXRF->setExpression(QString("%1/I_0").arg(regionName));
			scan_->addAnalyzedDataSource(NormXRF);
		}
	}
}

AMAction3* IDEASXASScanActionController::createInitializationActions(){

	AMControlMoveActionInfo3 *moveActionInfo;
	AMControlMoveAction3 *moveAction;
	AMControl *tmpControl;

	AMListAction3 *initializationActions = new AMListAction3(new AMListActionInfo3("IDEAS XAS Initialization Actions", "IDEAS XAS Initialization Actions"));


	AMListAction3 *initializationStage1 = new AMListAction3(new AMListActionInfo3("IDEAS XAS Initialization Stage 1", "IDEAS XAS Initialization Stage 1"), AMListAction3::Parallel);

	tmpControl = IDEASBeamline::ideas()->monoDirectEnergyControl();
	AMControlInfo monoEnergy = tmpControl->toInfo();

		double startE = double(configuration_->scanAxisAt(0)->regionAt(0)->regionStart());
		double mono2d = IDEASBeamline::ideas()->mono2d()->value();
		double braggAngle = asin(12398.4193 / mono2d / startE);
		double backlashDegrees = 4;

		double dE = (backlashDegrees / 180 * M_PI) * (mono2d * startE * startE * cos(braggAngle * M_PI / 180)) / (-12398.4193);
		double backlashE = startE + dE;
		if(backlashE < IDEASBeamline::ideas()->monoLowEV()->value()) backlashE = IDEASBeamline::ideas()->monoLowEV()->value();


	monoEnergy.setValue(backlashE);
	moveActionInfo = new AMControlMoveActionInfo3(monoEnergy);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	initializationStage1->addSubAction(moveAction);
	//initializationStage1->addSubAction(IDEASBeamline::ideas()->scaler()->createContinuousEnableAction3(false));
	initializationStage1->addSubAction(IDEASBeamline::ideas()->scaler()->createStartAction3(false));
	initializationStage1->addSubAction(IDEASBeamline::ideas()->scaler()->createTotalScansAction3(1));



	AMListAction3 *initializationStage2 = new AMListAction3(new AMListActionInfo3("IDEAS XAS Initialization Stage 2", "IDEAS XAS Initialization Stage 2"), AMListAction3::Sequential);

	initializationActions->addSubAction(initializationStage1);
	initializationActions->addSubAction(initializationStage2);

	return initializationActions;
}


void IDEASXASScanActionController::onInitializationActionsListSucceeded(){

	AMControlInfoList positions(IDEASBeamline::ideas()->exposedControls()->toInfoList());
	positions.remove(positions.indexOf("masterDwell"));
	positions.remove(positions.indexOf("DirectEnergy"));
	positions.remove(positions.indexOf("Energy"));
	positions.remove(positions.indexOf("XRF1E Real Time"));

	if(!configuration_->isXRFScan())
	{
		positions.remove(positions.indexOf("XRF1E Peaking Time"));
		positions.remove(positions.indexOf("XRF1E Trigger Level"));
		positions.remove(positions.indexOf("XRF1E Baseline Threshold"));
		positions.remove(positions.indexOf("XRF1E Preamp Gain"));
	}
	scan_->setScanInitialConditions(positions);
	AMScanActionController::onInitializationActionsListSucceeded();

}

AMAction3* IDEASXASScanActionController::createCleanupActions(){

	AMListAction3 *cleanupActions = new AMListAction3(new AMListActionInfo3("IDEAS XAS Cleanup Actions", "IDEAS XAS Cleanup Actions"));

	cleanupActions->addSubAction(new AMWaitAction(new AMWaitActionInfo(IDEASBeamline::ideas()->scaler()->dwellTime())));
	cleanupActions->addSubAction(IDEASBeamline::ideas()->scaler()->createStartAction3(false));
	//cleanupActions->addSubAction(IDEASBeamline::ideas()->scaler()->createContinuousEnableAction3(false));
	cleanupActions->addSubAction(IDEASBeamline::ideas()->scaler()->createDwellTimeAction3(0.1));
	//cleanupActions->addSubAction(IDEASBeamline::ideas()->scaler()->createContinuousEnableAction3(true));
	cleanupActions->addSubAction(IDEASBeamline::ideas()->scaler()->createTotalScansAction3(0));
	cleanupActions->addSubAction(IDEASBeamline::ideas()->scaler()->createStartAction3(true));


	return cleanupActions;
}


void IDEASXASScanActionController::cancelImplementation(){

	AMStepScanActionController::cancelImplementation();

	AMAction3 *cleanupActions = createCleanupActions();

	cleanupActions->start();

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
