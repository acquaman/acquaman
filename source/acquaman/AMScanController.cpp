#include "AMScanController.h"

AMScanController* AMScanController::currentScanController_ = 0;

AMScanController::AMScanController(AMScanConfiguration *cfg, QObject *parent) :
	QObject(parent)
{
	generalCfg_ = cfg;
	_pCfg_ = & generalCfg_;
	generalScan_ = NULL;
	_pScan_ = &generalScan_;
}

AMScanController* AMScanController::currentScanController(){
	return currentScanController_;
}

bool AMScanController::setCurrentScanController(AMScanController *newScanController){
	if(currentScanController_)
		return false;
	currentScanController_ = newScanController;
	currentScanController_->initiateCurrentScanControllerCreated();
	return true;
}

void AMScanController::initiateCurrentScanControllerCreated(){
	emit currentScanControllerCreated();
}

void AMScanController::initiateCurrentScanControllerDestroyed(){
	emit currentScanControllerDestroyed();
}

void AMScanController::onCurrentScanControllerFinished(){
	delete currentScanController_;
	currentScanController_ = 0;
	currentScanController_->initiateCurrentScanControllerDestroyed();
}
