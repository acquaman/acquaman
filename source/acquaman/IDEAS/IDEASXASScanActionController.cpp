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
        ideasDetectors.addDetectorInfo(IDEASBeamline::ideas()->exposedDetectorByName("XRF")->toInfo());
	configuration_->setDetectorConfigurations(ideasDetectors);

//        for(int i=0, cc=scan_->rawData()->measurementCount(); i<cc; ++i)
//                scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), i));


//        QList<AMDataSource*> rawDataSources;
//        for(int i=0, cc=scan_->rawDataSourceCount(); i<cc; ++i)
//                rawDataSources << scan_->rawDataSources()->at(i);


//        AM1DExpressionAB* ab = new AM1DExpressionAB("NormSample");
//        ab->setDescription("NormSample");
//        ab->setInputDataSources(rawDataSources);
//        ab->setExpression("ln(I_0/I_sample)");
//        scan_->addAnalyzedDataSource(ab);

//        ab = new AM1DExpressionAB("NormRef");
//        ab->setDescription("NormRef");
//        ab->setInputDataSources(rawDataSources);
//        ab->setExpression("ln(I_sample/I_ref)");
//        scan_->addAnalyzedDataSource(ab);

//        AM2DSummingAB* sumAb = new AM2DSummingAB("XRF Sum");
//        sumAb->setDescription("XRF Sum");
//        sumAb->setInputDataSources(rawDataSources);
//        sumAb->setSumAxis(1);
//        scan_->addAnalyzedDataSource(sumAb);

        double longestTime = 0;
        for(int i=0, cc=configuration_->regionCount(); i<cc; ++i)
            if(configuration_->regionTime(i) > longestTime) longestTime = configuration_->regionTime(i);

        pokeSyncDwell_ = new QTimer(this);

        pokeSyncDwell_->setInterval((longestTime + 5) *1000);
        pokeSyncDwell_->setSingleShot(true);

        //connect(pokeSyncDwell_, SIGNAL(timeout()), this, SLOT(onPokeSyncDwell());
        connect(pokeSyncDwell_, SIGNAL(timeout()), IDEASBeamline::ideas()->synchronizedDwellTime(), SLOT(start()));
        connect(IDEASBeamline::ideas()->synchronizedDwellTime(),SIGNAL(statusChanged(bool)), this, SLOT(onSyncDwellStatusChanged(bool)));
}

IDEASXASScanActionController::~IDEASXASScanActionController(){}

void IDEASXASScanActionController::buildScanControllerImplementation()
{
}

AMAction3* IDEASXASScanActionController::createInitializationActions(){
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


        //AMListAction3 *initializationStage2 = new AMListAction3(new AMListActionInfo3("IDEAS XAS Initialization Stage 2", "IDEAS XAS Initialization Stage 2"), AMListAction3::Parallel);
        //initializationStage2->addSubAction(IDEASBeamline::ideas()->createBeamOnAction());

	initializationActions->addSubAction(initializationStage1);
        //initializationActions->addSubAction(initializationStage2);

	return initializationActions;
}

AMAction3* IDEASXASScanActionController::createCleanupActions(){

    return 0;
}

void IDEASXASScanActionController::onSyncDwellStatusChanged(bool unused){
    Q_UNUSED(unused);
    if(isRunning())
        pokeSyncDwell_->start();
    qDebug() << "timer restarted";
}
