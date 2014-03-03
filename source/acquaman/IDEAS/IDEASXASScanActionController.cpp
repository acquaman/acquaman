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


#include <QDebug>

IDEASXASScanActionController::IDEASXASScanActionController(IDEASXASScanConfiguration *cfg, QObject *parent) :
	AMRegionScanActionController(cfg, parent)
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


	AMDetectorInfoSet ideasDetectors;
        ideasDetectors.addDetectorInfo(IDEASBeamline::ideas()->exposedDetectorByName("I_vac")->toInfo());
        ideasDetectors.addDetectorInfo(IDEASBeamline::ideas()->exposedDetectorByName("I_0")->toInfo());
        ideasDetectors.addDetectorInfo(IDEASBeamline::ideas()->exposedDetectorByName("I_sample")->toInfo());
        ideasDetectors.addDetectorInfo(IDEASBeamline::ideas()->exposedDetectorByName("I_ref")->toInfo());
        ideasDetectors.addDetectorInfo(IDEASBeamline::ideas()->exposedDetectorByName("XRF1E")->toInfo());
	configuration_->setDetectorConfigurations(ideasDetectors);


        double longestTime = 0;
        for(int i=0, cc=configuration_->regionCount(); i<cc; ++i)
            if(configuration_->regionTime(i) > longestTime) longestTime = configuration_->regionTime(i);

        pokeSyncDwell_ = new QTimer(this);

        pokeSyncDwell_->setInterval((longestTime + 2) *1000);
        pokeSyncDwell_->setSingleShot(true);

        //connect(pokeSyncDwell_, SIGNAL(timeout()), this, SLOT(onPokeSyncDwell());
        connect(pokeSyncDwell_, SIGNAL(timeout()), IDEASBeamline::ideas()->synchronizedDwellTime(), SLOT(start()));
        connect(IDEASBeamline::ideas()->synchronizedDwellTime(),SIGNAL(statusChanged(bool)), this, SLOT(onSyncDwellStatusChanged(bool)));
}

IDEASXASScanActionController::~IDEASXASScanActionController(){}

void IDEASXASScanActionController::buildScanControllerImplementation()
{

    QList<AMDataSource*> raw1DDataSources;
    for(int i=0; i<scan_->rawDataSources()->count(); i++)
            if(scan_->rawDataSources()->at(i)->rank() == 1)
                    raw1DDataSources << scan_->rawDataSources()->at(i);

    int rawIvacIndex = scan_->rawDataSources()->indexOfKey("I_vac");
    int rawI0Index = scan_->rawDataSources()->indexOfKey("I_0");
    int rawIsampleIndex = scan_->rawDataSources()->indexOfKey("I_sample");
    int rawIrefIndex = scan_->rawDataSources()->indexOfKey("I_ref");


    if(rawI0Index != -1 && rawIsampleIndex != -1) {

        AM1DExpressionAB* NormSample = new AM1DExpressionAB("NormSample");
        NormSample->setDescription("NormSample");
        NormSample->setInputDataSources(raw1DDataSources);
        NormSample->setExpression("ln(I_0/I_sample)");
        scan_->addAnalyzedDataSource(NormSample);
     }

    if(rawIrefIndex != -1 && rawIsampleIndex != -1) {

        AM1DExpressionAB* NormRef = new AM1DExpressionAB("NormRef");
        NormRef->setDescription("NormRef");
        NormRef->setInputDataSources(raw1DDataSources);
        NormRef->setExpression("ln(I_sample/I_ref)");
        scan_->addAnalyzedDataSource(NormRef);
    }

    QList<AMDataSource*> raw2DDataSources;
    for(int i=0; i<scan_->rawDataSources()->count(); i++)
            if(scan_->rawDataSources()->at(i)->rank() == 2)
                    raw2DDataSources << scan_->rawDataSources()->at(i);

    int rawXRFIndex = scan_->rawDataSources()->indexOfKey("XRF1E");


    if(rawXRFIndex != -1) {
        AM2DSummingAB* sumAb = new AM2DSummingAB("XRFSum");
        sumAb->setDescription("XRFSum");
        sumAb->setInputDataSources(raw2DDataSources);
        sumAb->setSumAxis(1);
//        sumAb->setSumRangeMin(350);
//        sumAb->setSumRangeMin(1500);
        scan_->addAnalyzedDataSource(sumAb);

          QList<AMDataSource*> all1DDataSources;


        for(int i=0; i<scan_->analyzedDataSources()->count(); i++)
                if(scan_->analyzedDataSources()->at(i)->rank() == 1)
                        all1DDataSources << scan_->analyzedDataSources()->at(i);
        for(int i=0; i<scan_->rawDataSources()->count(); i++)
                if(scan_->rawDataSources()->at(i)->rank() == 1)
                        all1DDataSources << scan_->rawDataSources()->at(i);



        AM1DExpressionAB* NormXRF = new AM1DExpressionAB("NormXRF");
        NormXRF->setDescription("NormXRF");
        NormXRF->setInputDataSources(all1DDataSources);
        NormXRF->setExpression("XRFSum/sqrt(I_0^2)");
        scan_->addAnalyzedDataSource(NormXRF);


    }

}

AMAction3* IDEASXASScanActionController::createInitializationActions(){

        AMControlMoveActionInfo3 *moveActionInfo;
        AMControlMoveAction3 *moveAction;
        AMControl *tmpControl;

        AMListAction3 *initializationActions = new AMListAction3(new AMListActionInfo3("IDEAS XAS Initialization Actions", "IDEAS XAS Initialization Actions"));


	AMListAction3 *initializationStage1 = new AMListAction3(new AMListActionInfo3("IDEAS XAS Initialization Stage 1", "IDEAS XAS Initialization Stage 1"), AMListAction3::Parallel);

	CLSSynchronizedDwellTime *syncDwell = qobject_cast<CLSSynchronizedDwellTime*>(IDEASBeamline::ideas()->synchronizedDwellTime());
	QList<QString> allKeys;
	for(int x = 0; x < configuration_->detectorConfigurations().count(); x++)
		if(!allKeys.contains(IDEASBeamline::ideas()->exposedDetectorByInfo(configuration_->detectorConfigurations().at(x))->synchronizedDwellKey()))
			allKeys.append(IDEASBeamline::ideas()->exposedDetectorByInfo(configuration_->detectorConfigurations().at(x))->synchronizedDwellKey());

	for(int x = 0; x < syncDwell->elementCount(); x++){
		if(syncDwell->enabledAt(x) && !allKeys.contains(syncDwell->keyAt(x)) )
			initializationStage1->addSubAction(syncDwell->elementAt(x)->createEnableAction3(false));
		else if(!syncDwell->enabledAt(x) && allKeys.contains(syncDwell->keyAt(x)) )
			initializationStage1->addSubAction(syncDwell->elementAt(x)->createEnableAction3(true));
	}

        tmpControl = IDEASBeamline::ideas()->monoEnergyControl();
        AMControlInfo monoEnergy = tmpControl->toInfo();

        double startE = configuration_->startEnergy();
        double mono2d = IDEASBeamline::ideas()->mono2d()->value();
        double braggAngle = asin(12398.4193 / mono2d / startE);
        double backlashDegrees = 4;

        double dE = (backlashDegrees / 180 * M_PI) * (mono2d * startE * startE * cos(braggAngle * M_PI / 180)) / (-12398.4193);
        double backlashE = configuration_->startEnergy() + dE;
        if(backlashE < IDEASBeamline::ideas()->monoLowEV()->value()) backlashE = IDEASBeamline::ideas()->monoLowEV()->value();

        monoEnergy.setValue(backlashE);
        moveActionInfo = new AMControlMoveActionInfo3(monoEnergy);
        moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
        initializationStage1->addSubAction(moveAction);


        AMListAction3 *initializationStage2 = new AMListAction3(new AMListActionInfo3("IDEAS XAS Initialization Stage 2", "IDEAS XAS Initialization Stage 2"), AMListAction3::Sequential);
//        /*initializationStage2->addSubAction(IDEASBeamline::ideas()->createBeamOnAction());*/ NO NO NO NO


        tmpControl = IDEASBeamline::ideas()->ammeterGroupModeControl();
        AMControlInfo ammeterGroupMode = tmpControl->toInfo();
        ammeterGroupMode.setValue(1);
        moveActionInfo = new AMControlMoveActionInfo3(ammeterGroupMode);
        moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
        initializationStage2->addSubAction(moveAction);

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



	initializationActions->addSubAction(initializationStage1);
        initializationActions->addSubAction(initializationStage2);

	return initializationActions;
}

AMAction3* IDEASXASScanActionController::createCleanupActions(){

    AMListAction3 *cleanupActions = new AMListAction3(new AMListActionInfo3("IDEAS XAS Cleanup Actions", "IDEAS XAS Cleanup Actions"), AMListAction3::Parallel);

    CLSSynchronizedDwellTime *syncDwell = qobject_cast<CLSSynchronizedDwellTime*>(IDEASBeamline::ideas()->synchronizedDwellTime());
    for(int x = 0; x < syncDwell->elementCount(); x++)
            cleanupActions->addSubAction(syncDwell->elementAt(x)->createEnableAction3(syncDwell->enabledAt(x)));
    cleanupActions->addSubAction(syncDwell->createMasterTimeAction3(syncDwell->time()));

    AMControlMoveActionInfo3 *moveActionInfo;
    AMControlMoveAction3 *moveAction;
    AMControl *tmpControl;

    tmpControl = IDEASBeamline::ideas()->ammeterGroupModeControl();
    AMControlInfo ammeterGroupMode = tmpControl->toInfo();
    ammeterGroupMode.setValue(2);
    moveActionInfo = new AMControlMoveActionInfo3(ammeterGroupMode);
    moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
    cleanupActions->addSubAction(moveAction);

    return cleanupActions;
}

void IDEASXASScanActionController::onSyncDwellStatusChanged(bool unused){
    Q_UNUSED(unused);
    if(isRunning())
        pokeSyncDwell_->start();
}
