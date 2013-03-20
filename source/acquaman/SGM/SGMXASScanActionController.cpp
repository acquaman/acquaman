#include "SGMXASScanActionController.h"

#include "acquaman/AMScanActionControllerScanAssembler.h"
#include <QDebug>

SGMXASScanActionController::SGMXASScanActionController(SGMXASScanConfiguration *cfg, QObject *parent) :
	AMScanActionController(cfg, parent), SGMXASScanController(cfg)
{
	scan_ = specificScan_;

	qDebug() << "Number of regions in this configuration is " << config_->regionCount();

	AMScanActionControllerScanAssembler *newScanAssembler = new AMScanActionControllerScanAssembler(this);

	AMScanAxisRegion firstRegion(config_->regionStart(0), config_->regionDelta(0), config_->regionEnd(0), config_->regionTime(0), this);
	AMScanAxis *energyAxis = new AMScanAxis(AMScanAxis::StepAxis, firstRegion, this);
	for(int x = 1; x < config_->regionCount(); x++){
		AMScanAxisRegion anotherRegion(config_->regionStart(x), config_->regionDelta(x), config_->regionEnd(x), config_->regionTime(x), this);
		energyAxis->appendRegion(anotherRegion);
	}

	newScanAssembler->appendAxis(SGMBeamline::sgm()->energy(), energyAxis);

	newScanAssembler->addDetector(SGMBeamline::sgm()->newAmptekSDD1());
	newScanAssembler->addDetector(SGMBeamline::sgm()->newAmptekSDD2());

	actionTree_ = newScanAssembler->generateActionTree();
}

AMAction3* SGMXASScanActionController::actionsTree(){
	return actionTree_;
}

bool SGMXASScanActionController::initializeImplementation(){
	return true;
}

bool SGMXASScanActionController::startImplementation(){
	if(SGMBeamline::sgm()->isBeamlineScanning()){
		AMErrorMon::report(AMErrorReport(this,
				AMErrorReport::Alert,
				SGMXASSCANACTIONCONTROLLER_CANT_INTIALIZE,
				"Error, SGM XAS Scan Action Controller failed to start (SGM is already scanning). Either another scan is currently running or the scanning flag is stuck at Scanning."));
		return false;
	}
	return AMScanActionController::startImplementation();
}

void SGMXASScanActionController::cancelImplementation(){
	AMScanActionController::cancelImplementation();
}
