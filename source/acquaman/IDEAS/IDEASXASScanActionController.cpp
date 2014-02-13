#include "IDEASXASScanActionController.h"

#include "dataman/AMXASScan.h"
#include "beamline/CLS/CLSSynchronizedDwellTime.h"
#include "beamline/IDEAS/IDEASBeamline.h"

#include "actions3/AMListAction3.h"

IDEASXASScanActionController::IDEASXASScanActionController(IDEASXASScanConfiguration *cfg, QObject *parent) :
	AMRegionScanActionController(cfg, parent)
{
	configuration_ = cfg;

	scan_ = new AMXASScan();
	scan_->setFileFormat("ideas2014XAS");
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
	ideasDetectors.addDetectorInfo(IDEASBeamline::ideas()->exposedDetectorByName("OldIonDetector")->toInfo());
	ideasDetectors.addDetectorInfo(IDEASBeamline::ideas()->exposedDetectorByName("I0Detector")->toInfo());
	ideasDetectors.addDetectorInfo(IDEASBeamline::ideas()->exposedDetectorByName("SampleDetector")->toInfo());
	ideasDetectors.addDetectorInfo(IDEASBeamline::ideas()->exposedDetectorByName("ReferenceDetector")->toInfo());
        ideasDetectors.addDetectorInfo(IDEASBeamline::ideas()->exposedDetectorByName("KETEK")->toInfo());
	configuration_->setDetectorConfigurations(ideasDetectors);
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


	AMListAction3 *initializationStage2 = new AMListAction3(new AMListActionInfo3("IDEAS XAS Initialization Stage 2", "IDEAS XAS Initialization Stage 2"), AMListAction3::Parallel);
	initializationStage2->addSubAction(IDEASBeamline::ideas()->createBeamOnAction());

	initializationActions->addSubAction(initializationStage1);
	initializationActions->addSubAction(initializationStage2);

	return initializationActions;
}

AMAction3* IDEASXASScanActionController::createCleanupActions(){
	return 0;
}
