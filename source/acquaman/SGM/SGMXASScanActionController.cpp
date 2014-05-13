#include "SGMXASScanActionController.h"

#include "dataman/AMXASScan.h"
#include "beamline/SGM/SGMBeamline.h"
#include "dataman/AMSample.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "actions3/AMListAction3.h"
#include "beamline/CLS/CLSAmptekSDD123DetectorNew.h"
#include "analysis/AM1DDarkCurrentCorrectionAB.h"

SGMXASScanActionController::SGMXASScanActionController(SGMXASScanConfiguration2013 *cfg, QObject *parent) :
	AMRegionScanActionController(cfg, parent)
{
	configuration_ = cfg;

	scan_ = new AMXASScan();
	scan_->setFileFormat("amRegionAscii2013");
	scan_->setScanConfiguration(cfg);
	//scan_->setSampleId(SGMBeamline::sgm()->currentSampleId());
	scan_->setSample(SGMBeamline::sgm()->currentSample());
	if(SGMBeamline::sgm()->currentSample())
		SGMBeamline::sgm()->currentSample()->addScan(scan_);
	connect(scan_, SIGNAL(storedToDb()), SGMBeamline::sgm()->currentSample(), SLOT(forceStoreToDb()));
	scan_->setIndexType("fileSystem");
	scan_->rawData()->addScanAxis(AMAxisInfo("eV", 0, "Incident Energy", "eV"));

	QString scanName;
	QString sampleName;
	if(scan_->sampleId() == -1)
		sampleName = "Unknown Sample";
	else if(scan_->sample())
		sampleName = scan_->sample()->name();
	else
		sampleName = AMSample(scan_->sampleId(), AMUser::user()->database()).name();
	if(configuration_->userScanName() == ""){
		scanName = configuration_->autoScanName();
		scan_->setName(QString("%1 - %2").arg(sampleName).arg(scanName));
	}
	else{
		scanName = configuration_->userScanName();
		scan_->setName(QString("%1 - %2").arg(scanName).arg(sampleName));
	}
}

SGMXASScanActionController::~SGMXASScanActionController(){}

void SGMXASScanActionController::buildScanControllerImplementation()
{

    int dwellTimeIndex = scan_->indexOfDataSource(SGMBeamline::sgm()->dwellTimeDetector()->name());

//    if (dwellTimeIndex != -1 && teyIndex != -1) {
//        AMDataSource* teySource = scan_->dataSourceAt(teyIndex);
//        AMDataSource* dwellTimeSource = scan_->dataSourceAt(dwellTimeIndex);

//        AM1DDarkCurrentCorrectionAB *teyCorrection = new AM1DDarkCurrentCorrectionAB("TEY_DarkCorrect");
//        teyCorrection->setDescription("TEY Dark Current Correction");
//        teyCorrection->setDataName(teySource->name());
//        teyCorrection->setDwellTimeName(dwellTimeSource->name());
//        teyCorrection->setInputDataSources(QList<AMDataSource*>() << teySource << dwellTimeSource);
//        teyCorrection->setDarkCurrent(SGMBeamline::sgm()->newTEYDetector()->darkCurrentCorrection());
//        teyCorrection->setTimeUnitMultiplier(.001);
//        scan_->addAnalyzedDataSource(teyCorrection, true, false);
//    }

    if (dwellTimeIndex != -1) {
        AMDataSource* dwellTimeSource = scan_->dataSourceAt(dwellTimeIndex);

        for (int i = 0, size = regionsConfiguration_->detectorConfigurations().count(); i < size; i++){
            AMDetector *detector = AMBeamline::bl()->exposedDetectorByInfo(regionsConfiguration_->detectorConfigurations().at(i));

            if (detector) {
                int detectorIndex = scan_->indexOfDataSource(detector->name());

                if (detectorIndex != -1 && detector->rank() == 0 && detector->canDoDarkCurrentCorrection()) {

                    AMDataSource* detectorSource = scan_->dataSourceAt(detectorIndex);

                    AM1DDarkCurrentCorrectionAB *detectorCorrection = new AM1DDarkCurrentCorrectionAB(QString("%1_DarkCorrect").arg(detector->name()));
                    detectorCorrection->setDescription(QString("%1 Dark Current Correction").arg(detector->name()));
                    detectorCorrection->setDataName(detectorSource->name());
                    detectorCorrection->setDwellTimeName(dwellTimeSource->name());
                    detectorCorrection->setInputDataSources(QList<AMDataSource*>() << detectorSource << dwellTimeSource);
//                    detectorCorrection->setDarkCurrent(detector->darkCurrentCorrection());
                    connect( detector, SIGNAL(newDarkCurrentMeasurementValueReady(double)), detectorCorrection, SLOT(setDarkCurrent(double)) );
                    detectorCorrection->setTimeUnitMultiplier(.001);
                    scan_->addAnalyzedDataSource(detectorCorrection, true, false);
                }
            }
        }
    }

}

AMAction3* SGMXASScanActionController::createInitializationActions(){
	AMControlMoveActionInfo3 *moveActionInfo;
	AMControlMoveAction3 *moveAction;
	AMControl *tmpControl;

	AMListAction3 *initializationActions = new AMListAction3(new AMListActionInfo3("SGM XAS Initialization Actions", "SGM XAS Initialization Actions"));


	AMListAction3 *initializationStage1 = new AMListAction3(new AMListActionInfo3("SGM XAS Initialization Stage 1", "SGM XAS Initialization Stage 1"), AMListAction3::Parallel);

	tmpControl = SGMBeamline::sgm()->grating();
	AMControlInfo gratingSetpoint = tmpControl->toInfo();
	gratingSetpoint.setValue(configuration_->fluxResolutionGroup().controlNamed(tmpControl->name()).value());
	moveActionInfo = new AMControlMoveActionInfo3(gratingSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	initializationStage1->addSubAction(moveAction);

	tmpControl = SGMBeamline::sgm()->exitSlitGap();
	AMControlInfo exitSlitGapSetpoint = tmpControl->toInfo();
	exitSlitGapSetpoint.setValue(configuration_->fluxResolutionGroup().controlNamed(tmpControl->name()).value());
	moveActionInfo = new AMControlMoveActionInfo3(exitSlitGapSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	initializationStage1->addSubAction(moveAction);

	CLSSynchronizedDwellTime *syncDwell = qobject_cast<CLSSynchronizedDwellTime*>(SGMBeamline::sgm()->synchronizedDwellTime());
	QList<QString> allKeys;
	for(int x = 0; x < configuration_->detectorConfigurations().count(); x++)
		if(!allKeys.contains(SGMBeamline::sgm()->exposedDetectorByInfo(configuration_->detectorConfigurations().at(x))->synchronizedDwellKey()))
			allKeys.append(SGMBeamline::sgm()->exposedDetectorByInfo(configuration_->detectorConfigurations().at(x))->synchronizedDwellKey());

	for(int x = 0; x < syncDwell->elementCount(); x++){
		if(syncDwell->enabledAt(x) && !allKeys.contains(syncDwell->keyAt(x)) )
			initializationStage1->addSubAction(syncDwell->elementAt(x)->createEnableAction3(false));
		else if(!syncDwell->enabledAt(x) && allKeys.contains(syncDwell->keyAt(x)) )
			initializationStage1->addSubAction(syncDwell->elementAt(x)->createEnableAction3(true));
	}

	CLSAmptekSDD123DetectorNew *amptek1 = qobject_cast<CLSAmptekSDD123DetectorNew*>(SGMBeamline::sgm()->newAmptekSDD1());
	if(amptek1)
		initializationStage1->addSubAction(amptek1->createEnableAction3(configuration_->detectorConfigurations().contains(SGMBeamline::sgm()->newAmptekSDD1()->name())));
	CLSAmptekSDD123DetectorNew *amptek2 = qobject_cast<CLSAmptekSDD123DetectorNew*>(SGMBeamline::sgm()->newAmptekSDD2());
	if(amptek2)
		initializationStage1->addSubAction(amptek2->createEnableAction3(configuration_->detectorConfigurations().contains(SGMBeamline::sgm()->newAmptekSDD2()->name())));
	CLSAmptekSDD123DetectorNew *amptek3 = qobject_cast<CLSAmptekSDD123DetectorNew*>(SGMBeamline::sgm()->newAmptekSDD3());
	if(amptek3)
		initializationStage1->addSubAction(amptek3->createEnableAction3(configuration_->detectorConfigurations().contains(SGMBeamline::sgm()->newAmptekSDD3()->name())));
	CLSAmptekSDD123DetectorNew *amptek4 = qobject_cast<CLSAmptekSDD123DetectorNew*>(SGMBeamline::sgm()->newAmptekSDD4());
	if(amptek4)
		initializationStage1->addSubAction(amptek4->createEnableAction3(configuration_->detectorConfigurations().contains(SGMBeamline::sgm()->newAmptekSDD4()->name())));

	initializationStage1->addSubAction(SGMBeamline::sgm()->scaler()->createStartAction3(false));
	initializationStage1->addSubAction(SGMBeamline::sgm()->scaler()->createContinuousEnableAction3(false));


	AMListAction3 *initializationStage2 = new AMListAction3(new AMListActionInfo3("SGM XAS Initialization Stage 2", "SGM XAS Initialization Stage 2"), AMListAction3::Parallel);
	initializationStage2->addSubAction(SGMBeamline::sgm()->scaler()->createScansPerBufferAction3(1));
	initializationStage2->addSubAction(SGMBeamline::sgm()->scaler()->createTotalScansAction3(1));


	AMListAction3 *initializationStage3 = new AMListAction3(new AMListActionInfo3("SGM XAS Initialization Stage 3", "SGM XAS Initialization Stage 3"), AMListAction3::Parallel);

	tmpControl = SGMBeamline::sgm()->undulatorTracking();
	AMControlInfo undulatorTrackingSetpoint = tmpControl->toInfo();
	undulatorTrackingSetpoint.setValue(configuration_->trackingGroup().controlNamed(tmpControl->name()).value());
	moveActionInfo = new AMControlMoveActionInfo3(undulatorTrackingSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	initializationStage3->addSubAction(moveAction);

	qDebug() << "Undulator tracking " << configuration_->trackingGroup().controlNamed(tmpControl->name()).value();

	tmpControl = SGMBeamline::sgm()->exitSlitTracking();
	AMControlInfo exitSlitTrackingSetpoint = tmpControl->toInfo();
	exitSlitTrackingSetpoint.setValue(configuration_->trackingGroup().controlNamed(tmpControl->name()).value());
	moveActionInfo = new AMControlMoveActionInfo3(exitSlitTrackingSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	initializationStage3->addSubAction(moveAction);

	qDebug() << "Exitslit tracking " << configuration_->trackingGroup().controlNamed(tmpControl->name()).value();

	tmpControl = SGMBeamline::sgm()->monoTracking();
	AMControlInfo monoTrackingSetpoint = tmpControl->toInfo();
	monoTrackingSetpoint.setValue(configuration_->trackingGroup().controlNamed(tmpControl->name()).value());
	moveActionInfo = new AMControlMoveActionInfo3(monoTrackingSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	initializationStage3->addSubAction(moveAction);

	qDebug() << "Mono tracking " << configuration_->trackingGroup().controlNamed(tmpControl->name()).value();

	//really? here?
	tmpControl = SGMBeamline::sgm()->harmonic();
	AMControlInfo harmonicSetpoint = tmpControl->toInfo();
	harmonicSetpoint.setValue(configuration_->fluxResolutionGroup().controlNamed(tmpControl->name()).value());
	moveActionInfo = new AMControlMoveActionInfo3(harmonicSetpoint);
	moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
	initializationStage3->addSubAction(moveAction);

    AMListAction3* initializationStage4 = new AMListAction3(new AMListActionInfo3("SGM XAS Initialization Stage 4", "SGM XAS Initialization Stage 4"), AMListAction3::Sequential);

    for (int i = 0, size = regionsConfiguration_->detectorConfigurations().count(); i < size; i++){
        AMDetector *detector = AMBeamline::bl()->exposedDetectorByInfo(regionsConfiguration_->detectorConfigurations().at(i));

        bool sharedSourceFound = false;

        if (detector) {
            int detectorIndex = scan_->indexOfDataSource(detector->name());

            if (detectorIndex != -1 && detector->rank() == 0 && detector->canDoDarkCurrentCorrection()) {
                bool isSourceShared = detector->sharesDetectorTriggerSource();

                if (isSourceShared && !sharedSourceFound) {
                    sharedSourceFound = true;
                    initializationStage4->addSubAction(detector->createDarkCurrentCorrectionActions(10));

                } else if (!isSourceShared) {
                    initializationStage4->addSubAction(detector->createDarkCurrentCorrectionActions(10));

                }
            }
        }
    }


    AMListAction3 *initializationStage5 = new AMListAction3(new AMListActionInfo3("SGM XAS Initialization Stage 5", "SGM XAS Initialization Stage 5"), AMListAction3::Parallel);
    initializationStage5->addSubAction(SGMBeamline::sgm()->createBeamOnActions3());

	initializationActions->addSubAction(initializationStage1);
	initializationActions->addSubAction(initializationStage2);
	initializationActions->addSubAction(initializationStage3);
    initializationActions->addSubAction(initializationStage4);
    initializationActions->addSubAction(initializationStage5);

	return initializationActions;
}

AMAction3* SGMXASScanActionController::createCleanupActions(){
	AMListAction3 *cleanupActions = new AMListAction3(new AMListActionInfo3("SGM XAS Cleanup Actions", "SGM XAS Cleanup Actions"), AMListAction3::Parallel);

	CLSSIS3820Scaler *scaler = SGMBeamline::sgm()->scaler();
	cleanupActions->addSubAction(scaler->createContinuousEnableAction3(scaler->isContinuous()));
	cleanupActions->addSubAction(scaler->createScansPerBufferAction3(scaler->scansPerBuffer()));
	cleanupActions->addSubAction(scaler->createTotalScansAction3(scaler->totalScans()));

	CLSSynchronizedDwellTime *syncDwell = qobject_cast<CLSSynchronizedDwellTime*>(SGMBeamline::sgm()->synchronizedDwellTime());
	for(int x = 0; x < syncDwell->elementCount(); x++)
		cleanupActions->addSubAction(syncDwell->elementAt(x)->createEnableAction3(syncDwell->enabledAt(x)));
	cleanupActions->addSubAction(syncDwell->createMasterTimeAction3(syncDwell->time()));

	CLSAmptekSDD123DetectorNew *amptek1 = qobject_cast<CLSAmptekSDD123DetectorNew*>(SGMBeamline::sgm()->newAmptekSDD1());
	if(amptek1)
		cleanupActions->addSubAction(amptek1->createEnableAction3(amptek1->isEnabled()));
	CLSAmptekSDD123DetectorNew *amptek2 = qobject_cast<CLSAmptekSDD123DetectorNew*>(SGMBeamline::sgm()->newAmptekSDD2());
	if(amptek2)
		cleanupActions->addSubAction(amptek2->createEnableAction3(amptek2->isEnabled()));
	CLSAmptekSDD123DetectorNew *amptek3 = qobject_cast<CLSAmptekSDD123DetectorNew*>(SGMBeamline::sgm()->newAmptekSDD3());
	if(amptek3)
		cleanupActions->addSubAction(amptek3->createEnableAction3(amptek3->isEnabled()));
	CLSAmptekSDD123DetectorNew *amptek4 = qobject_cast<CLSAmptekSDD123DetectorNew*>(SGMBeamline::sgm()->newAmptekSDD4());
	if(amptek4)
		cleanupActions->addSubAction(amptek4->createEnableAction3(amptek4->isEnabled()));

	AMControlInfo fastShutterSetpoint = SGMBeamline::sgm()->fastShutterVoltage()->toInfo();
	AMControlMoveAction3 *fastShutterAction = new AMControlMoveAction3(new AMControlMoveActionInfo3(fastShutterSetpoint), SGMBeamline::sgm()->fastShutterVoltage());
	cleanupActions->addSubAction(fastShutterAction);

	return cleanupActions;
}
