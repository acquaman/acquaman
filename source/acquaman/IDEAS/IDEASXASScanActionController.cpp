#include "IDEASXASScanActionController.h"
#include "acquaman/AMScanActionControllerScanAssembler.h"
#include "acquaman/AMScanActionControllerScanConfigurationConverter.h"



#include "dataman/AMXASScan.h"
#include "beamline/CLS/CLSSynchronizedDwellTime.h"
#include "beamline/IDEAS/IDEASBeamline.h"
#include "analysis/AM1DExpressionAB.h"
#include "analysis/AM2DSummingAB.h"
#include "application/IDEAS/IDEASAppController.h"

#include "actions3/AMListAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"
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

	//useFeedback_ = true;

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
//	ideasDetectors.addDetectorInfo(IDEASBeamline::ideas()->exposedDetectorByName("I_vac_6485")->toInfo());
	ideasDetectors.addDetectorInfo(IDEASBeamline::ideas()->I_0()->toInfo());
	ideasDetectors.addDetectorInfo(IDEASBeamline::ideas()->Sample()->toInfo());
	ideasDetectors.addDetectorInfo(IDEASBeamline::ideas()->Reference()->toInfo());
	ideasDetectors.addDetectorInfo(IDEASBeamline::ideas()->exposedDetectorByName("XRF1E")->toInfo());
//<<<<<<< HEAD
//	//	ideasDetectors.addDetectorInfo(IDEASBeamline::ideas()->exposedDetectorByName("DwellTime")->toInfo());
//	configuration_->setDetectorConfigurations(ideasDetectors);

//	double longestTime = 0;
//	for(int i=0, cc=configuration_->regionCount(); i<cc; ++i)
//		if(configuration_->regionTime(i) > longestTime) longestTime = configuration_->regionTime(i);

//	pokeSyncDwell_ = new QTimer(this);

//	pokeSyncDwell_->setInterval((longestTime + 2) *1000);
//	pokeSyncDwell_->setSingleShot(true);
//=======
	ideasDetectors.addDetectorInfo(IDEASBeamline::ideas()->dwellTime()->toInfo());
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

	//        double longestTime = 0;
//	for(int i=0, cc=configuration_->scanAxisAt(0)->regionCount(); i<cc; ++i)
//	    if(double(configuration_->scanAxisAt(0)->regionAt(i)->regionTime()) > longestTime) longestTime = double(configuration_->scanAxisAt(0)->regionAt(i)->regionTime());

//        pokeSyncDwell_ = new QTimer(this);

//        pokeSyncDwell_->setInterval((longestTime + 2) *1000);
//	pokeSyncDwell_->setSingleShot(true);
//>>>>>>> newXRFDetector

	//connect(pokeSyncDwell_, SIGNAL(timeout()), this, SLOT(onPokeSyncDwell());
	//        connect(pokeSyncDwell_, SIGNAL(timeout()), IDEASBeamline::ideas()->synchronizedDwellTime(), SLOT(start()));
	//        connect(IDEASBeamline::ideas()->synchronizedDwellTime(),SIGNAL(statusChanged(bool)), this, SLOT(onSyncDwellStatusChanged(bool)));
}

IDEASXASScanActionController::~IDEASXASScanActionController(){}

void IDEASXASScanActionController::createScanAssembler()
{
    scanAssembler_ = new AMEXAFSScanActionControllerAssembler(this);
}

void IDEASXASScanActionController::buildScanControllerImplementation()
{

	AMXRFDetector *detector = 0;

	detector = qobject_cast<AMXRFDetector *>(IDEASBeamline::bl()->exposedDetectorByName("XRF1E"));


	QList<AMDataSource*> raw1DDataSources;
	for(int i=0; i<scan_->rawDataSources()->count(); i++)
		if(scan_->rawDataSources()->at(i)->rank() == 1)
			raw1DDataSources << scan_->rawDataSources()->at(i);

//<<<<<<< HEAD
//	int rawI0Index = scan_->rawDataSources()->indexOfKey("I_0");
//	int rawIsampleIndex = scan_->rawDataSources()->indexOfKey("I_sample");
//	int rawIrefIndex = scan_->rawDataSources()->indexOfKey("I_ref");
//=======
//    int rawIvacIndex = scan_->rawDataSources()->indexOfKey("I_vac_6485");
    int rawI0Index = scan_->rawDataSources()->indexOfKey("I_0");
    int rawIsampleIndex = scan_->rawDataSources()->indexOfKey("Sample");
    int rawIrefIndex = scan_->rawDataSources()->indexOfKey("Reference");
//>>>>>>> newXRFDetector

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

//<<<<<<< HEAD
//		AM1DExpressionAB* NormSample = new AM1DExpressionAB("NormSample");
//		NormSample->setDescription("NormSample");
//		NormSample->setInputDataSources(raw1DDataSources);
//		NormSample->setExpression(QString("ln(%1/%2)").arg(configuration_->I0Channel()).arg(configuration_->ItChannel()));
//		scan_->addAnalyzedDataSource(NormSample);
//	}

//	if(rawIrefIndex != -1 && rawIsampleIndex != -1 && configuration_->isTransScan() && configuration_->IrChannel() != "None") {

//		AM1DExpressionAB* NormRef = new AM1DExpressionAB("NormRef");
//		NormRef->setDescription("NormRef");
//		NormRef->setInputDataSources(raw1DDataSources);
//		NormRef->setExpression(QString("ln(%1/%2)").arg(configuration_->ItChannel()).arg(configuration_->IrChannel()));
//		scan_->addAnalyzedDataSource(NormRef);
//	}
//=======
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
//>>>>>>> newXRFDetector

	QList<AMDataSource*> raw2DDataSources;
	for(int i=0; i<scan_->rawDataSources()->count(); i++)
		if(scan_->rawDataSources()->at(i)->rank() == 2)
			raw2DDataSources << scan_->rawDataSources()->at(i);



	//    int rawXRFIndex = scan_->rawDataSources()->indexOfKey("XRF1E");

	//    if(rawXRFIndex != -1) {
	//        AM2DSummingAB* sumAb = new AM2DSummingAB("XRFSum");
	//        sumAb->setDescription("XRFSum");
	//        sumAb->setInputDataSources(raw2DDataSources);
	//        sumAb->setSumAxis(1);
	//	sumAb->setSumRangeMin(350);
	//	sumAb->setSumRangeMax(1500);
	//        scan_->addAnalyzedDataSource(sumAb);






	//        AM1DExpressionAB* NormXRF = new AM1DExpressionAB("NormXRF");
	//        NormXRF->setDescription("NormXRF");
	//        NormXRF->setInputDataSources(all1DDataSources);
	//        NormXRF->setExpression("XRFSum/sqrt(I_0^2)");
	//        scan_->addAnalyzedDataSource(NormXRF);


	//    }

	QList<AMDataSource*> all1DDataSources;
	for(int i=0; i<scan_->analyzedDataSources()->count(); i++)
		if(scan_->analyzedDataSources()->at(i)->rank() == 1)
			all1DDataSources << scan_->analyzedDataSources()->at(i);
	for(int i=0; i<scan_->rawDataSources()->count(); i++)
		if(scan_->rawDataSources()->at(i)->rank() == 1)
			all1DDataSources << scan_->rawDataSources()->at(i);

	if (detector && configuration_->isXRFScan()){

//<<<<<<< HEAD
//		foreach (AMRegionOfInterest *region, detector->regionsOfInterest()){
//			AMRegionOfInterestAB *regionAB = (AMRegionOfInterestAB *)region->valueSource();
//			QString regionName = regionAB->name().replace(" ","_");
//			AM1DExpressionAB* NormXRF = new AM1DExpressionAB(QString("Norm%1").arg(regionName));
//			NormXRF->setDescription(QString("Norm%1").arg(regionName));
//			NormXRF->setInputDataSources(all1DDataSources);
//			NormXRF->setExpression(QString("%1/sqrt(%2^2)/DwellTime").arg(regionName).arg(configuration_->I0Channel()));
//			scan_->addAnalyzedDataSource(NormXRF);
//		}
//=======
	foreach (AMRegionOfInterest *region, detector->regionsOfInterest()){
	    AMRegionOfInterestAB *regionAB = (AMRegionOfInterestAB *)region->valueSource();
	    QString regionName = regionAB->name().replace(" ","_");
	    AM1DExpressionAB* NormXRF = new AM1DExpressionAB(QString("Norm%1").arg(regionName));
	    NormXRF->setDescription(QString("Norm%1").arg(regionName));
	    NormXRF->setInputDataSources(all1DDataSources);
//	    NormXRF->setExpression(QString("%1/I_0/dwellTime").arg(regionName));
	    NormXRF->setExpression(QString("%1/I_0").arg(regionName));
	    scan_->addAnalyzedDataSource(NormXRF);
//>>>>>>> newXRFDetector
	}


}
}

AMAction3* IDEASXASScanActionController::createInitializationActions(){

	AMControlMoveActionInfo3 *moveActionInfo;
	AMControlMoveAction3 *moveAction;
	AMControl *tmpControl;

	AMListAction3 *initializationActions = new AMListAction3(new AMListActionInfo3("IDEAS XAS Initialization Actions", "IDEAS XAS Initialization Actions"));


	AMListAction3 *initializationStage1 = new AMListAction3(new AMListActionInfo3("IDEAS XAS Initialization Stage 1", "IDEAS XAS Initialization Stage 1"), AMListAction3::Parallel);

//	CLSSynchronizedDwellTime *syncDwell = qobject_cast<CLSSynchronizedDwellTime*>(IDEASBeamline::ideas()->synchronizedDwellTime());
//	QList<QString> allKeys;
//	for(int x = 0; x < configuration_->detectorConfigurations().count(); x++)
//		if(!allKeys.contains(IDEASBeamline::ideas()->exposedDetectorByInfo(configuration_->detectorConfigurations().at(x))->synchronizedDwellKey()))
//			allKeys.append(IDEASBeamline::ideas()->exposedDetectorByInfo(configuration_->detectorConfigurations().at(x))->synchronizedDwellKey());

//	for(int x = 0; x < syncDwell->elementCount(); x++){
//		if(syncDwell->enabledAt(x) && !allKeys.contains(syncDwell->keyAt(x)) )
//			initializationStage1->addSubAction(syncDwell->elementAt(x)->createEnableAction3(false));
//		else if(!syncDwell->enabledAt(x) && allKeys.contains(syncDwell->keyAt(x)) )
//			initializationStage1->addSubAction(syncDwell->elementAt(x)->createEnableAction3(true));
//	}

	tmpControl = IDEASBeamline::ideas()->monoDirectEnergyControl();
	AMControlInfo monoEnergy = tmpControl->toInfo();

//<<<<<<< HEAD
//	double startE = configuration_->startEnergy();
//	double mono2d = IDEASBeamline::ideas()->mono2d()->value();
//	double braggAngle = asin(12398.4193 / mono2d / startE);
//	double backlashDegrees = 4;

//	double dE = (backlashDegrees / 180 * M_PI) * (mono2d * startE * startE * cos(braggAngle * M_PI / 180)) / (-12398.4193);
//	double backlashE = configuration_->startEnergy() + dE;
//	if(backlashE < IDEASBeamline::ideas()->monoLowEV()->value()) backlashE = IDEASBeamline::ideas()->monoLowEV()->value();
//=======
	double startE = double(configuration_->scanAxisAt(0)->regionAt(0)->regionStart());
        double mono2d = IDEASBeamline::ideas()->mono2d()->value();
        double braggAngle = asin(12398.4193 / mono2d / startE);
        double backlashDegrees = 4;

        double dE = (backlashDegrees / 180 * M_PI) * (mono2d * startE * startE * cos(braggAngle * M_PI / 180)) / (-12398.4193);
	double backlashE = startE + dE;
        if(backlashE < IDEASBeamline::ideas()->monoLowEV()->value()) backlashE = IDEASBeamline::ideas()->monoLowEV()->value();
//>>>>>>> newXRFDetector

	monoEnergy.setValue(backlashE);
	moveActionInfo = new AMControlMoveActionInfo3(monoEnergy);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	initializationStage1->addSubAction(moveAction);


	AMListAction3 *initializationStage2 = new AMListAction3(new AMListActionInfo3("IDEAS XAS Initialization Stage 2", "IDEAS XAS Initialization Stage 2"), AMListAction3::Sequential);
	//        /*initializationStage2->addSubAction(IDEASBeamline::ideas()->createBeamOnAction());*/ NO NO NO NO


//        tmpControl = IDEASBeamline::ideas()->ammeterGroupModeControl();
//        AMControlInfo ammeterGroupMode = tmpControl->toInfo();
//        ammeterGroupMode.setValue(1);
//        moveActionInfo = new AMControlMoveActionInfo3(ammeterGroupMode);
//        moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
//        initializationStage2->addSubAction(moveAction);

	//        tmpControl = syncDwell->startScanControl();
	//        AMControlInfo syncDwellStart = tmpControl->toInfo();
	//        syncDwellStart.setValue(1);
	//        moveActionInfo = new AMControlMoveActionInfo3(syncDwellStart);
	//        moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	//        initializationStage2->addSubAction(moveAction);

//        syncDwellScanMode.setValue(1);
//        moveActionInfo = new AMControlMoveActionInfo3(syncDwellScanMode);
//        moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
//        initializationStage2->addSubAction(moveAction);
	initializationStage2->addSubAction(IDEASBeamline::ideas()->scaler()->createTotalScansAction3(1));
	initializationStage2->addSubAction(IDEASBeamline::ideas()->scaler()->createStartAction3(false));


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
	//qDebug() << positions;

	//scan_->scanInitialConditions()->setValuesFrom(positions);
	scan_->setScanInitialConditions(positions);
	AMScanActionController::onInitializationActionsListSucceeded();

}

#include "actions3/actions/AMTimedWaitAction3.h"

AMAction3* IDEASXASScanActionController::createCleanupActions(){

//<<<<<<< HEAD
//	AMListAction3 *cleanupActions = new AMListAction3(new AMListActionInfo3("IDEAS XAS Cleanup Actions", "IDEAS XAS Cleanup Actions"), AMListAction3::Parallel);

//	CLSSynchronizedDwellTime *syncDwell = qobject_cast<CLSSynchronizedDwellTime*>(IDEASBeamline::ideas()->synchronizedDwellTime());
//	for(int x = 0; x < syncDwell->elementCount(); x++)
//		cleanupActions->addSubAction(syncDwell->elementAt(x)->createEnableAction3(syncDwell->enabledAt(x)));
//	cleanupActions->addSubAction(syncDwell->createMasterTimeAction3(syncDwell->time()));

//	AMControlMoveActionInfo3 *moveActionInfo;
//	AMControlMoveAction3 *moveAction;
//	AMControl *tmpControl;

//	tmpControl = IDEASBeamline::ideas()->ammeterGroupModeControl();
//	AMControlInfo ammeterGroupMode = tmpControl->toInfo();
//	ammeterGroupMode.setValue(2);
//	moveActionInfo = new AMControlMoveActionInfo3(ammeterGroupMode);
//	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
//	cleanupActions->addSubAction(moveAction);

//	tmpControl = IDEASBeamline::ideas()->masterDwellControl();
//	AMControlInfo masterDwell = tmpControl->toInfo();
//	masterDwell.setValue(1);
//	moveActionInfo = new AMControlMoveActionInfo3(masterDwell);
//	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
//	cleanupActions->addSubAction(moveAction);

//	return cleanupActions;
//}

//void IDEASXASScanActionController::onSyncDwellStatusChanged(bool unused){
//	Q_UNUSED(unused);
//	if(isRunning())
//		pokeSyncDwell_->start();
//}

//void IDEASXASScanActionController::cancelImplementation(){

//	AMAction3 *cleanupActions = createCleanupActions();
//=======
    AMListAction3 *cleanupActions = new AMListAction3(new AMListActionInfo3("IDEAS XAS Cleanup Actions", "IDEAS XAS Cleanup Actions"));

//    CLSSynchronizedDwellTime *syncDwell = qobject_cast<CLSSynchronizedDwellTime*>(IDEASBeamline::ideas()->synchronizedDwellTime());
//    for(int x = 0; x < syncDwell->elementCount(); x++)
//            cleanupActions->addSubAction(syncDwell->elementAt(x)->createEnableAction3(syncDwell->enabledAt(x)));
//    cleanupActions->addSubAction(syncDwell->createMasterTimeAction3(syncDwell->time()));

//    AMControlMoveActionInfo3 *moveActionInfo;
//    AMControlMoveAction3 *moveAction;
//    AMControl *tmpControl;

//    tmpControl = IDEASBeamline::ideas()->ammeterGroupModeControl();
//    AMControlInfo ammeterGroupMode = tmpControl->toInfo();
//    ammeterGroupMode.setValue(2);
//    moveActionInfo = new AMControlMoveActionInfo3(ammeterGroupMode);
//    moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
//    cleanupActions->addSubAction(moveAction);

//    tmpControl = IDEASBeamline::ideas()->masterDwellControl();
//    AMControlInfo masterDwell = tmpControl->toInfo();
//    masterDwell.setValue(1);
//    moveActionInfo = new AMControlMoveActionInfo3(masterDwell);
//    moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
//    cleanupActions->addSubAction(moveAction);
//    cleanupActions->addSubAction(IDEASBeamline::ideas()->scaler()->createContinuousEnableAction3(false));

	cleanupActions->addSubAction(new AMTimedWaitAction3(new AMTimedWaitActionInfo3(IDEASBeamline::ideas()->scaler()->dwellTime())));
	cleanupActions->addSubAction(IDEASBeamline::ideas()->scaler()->createStartAction3(false));
	cleanupActions->addSubAction(IDEASBeamline::ideas()->scaler()->createContinuousEnableAction3(false));
	cleanupActions->addSubAction(IDEASBeamline::ideas()->scaler()->createDwellTimeAction3(0.1));
	cleanupActions->addSubAction(IDEASBeamline::ideas()->scaler()->createContinuousEnableAction3(true));

 //   cleanupActions->addSubAction(IDEASBeamline::ideas()->scaler()->createTotalScansAction3(0));
 //   cleanupActions->addSubAction(IDEASBeamline::ideas()->scaler()->createStartAction3(true));


    //qDebug() << "IDEASXASScanActionController::createCleanupActions() called";

    return cleanupActions;
}

//void IDEASXASScanActionController::onSyncDwellStatusChanged(bool unused){
//    Q_UNUSED(unused);
//    if(isRunning())
//        pokeSyncDwell_->start();
//}

void IDEASXASScanActionController::cancelImplementation(){

    AMStepScanActionController::cancelImplementation();

    qDebug() << "IDEASXASScanActionController::cancelImplementation() called";

    AMAction3 *cleanupActions = createCleanupActions();
//>>>>>>> newXRFDetector

    cleanupActions->start();
    setCancelled();

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
