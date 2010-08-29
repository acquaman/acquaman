#include "AMScanController.h"

AMScanControllerSupervisor* AMScanControllerSupervisor::instance_ = 0;

AMScanController::AMScanController(AMScanConfiguration *cfg, QObject *parent) :
	QObject(parent)
{
	generalCfg_ = cfg;
	_pCfg_ = & generalCfg_;
	generalScan_ = NULL;
	_pScan_ = &generalScan_;
}


AMScanControllerSupervisor::AMScanControllerSupervisor(QObject *parent) :
		QObject(parent)
{
	currentScanController_ = NULL;
}


AMScanControllerSupervisor::~AMScanControllerSupervisor()
{
}

AMScanControllerSupervisor* AMScanControllerSupervisor::scanControllerSupervisor(){
	if(instance_ == 0)
		instance_ = new AMScanControllerSupervisor();
	return instance_;
}

void AMScanControllerSupervisor::releaseScanControllerSupervisor(){
	if(instance_){
		delete instance_;
		instance_ = 0;
	}
}

AMScanController* AMScanControllerSupervisor::currentScanController(){
	return currentScanController_;
}

bool AMScanControllerSupervisor::setCurrentScanController(AMScanController *newScanController){
	qDebug() << "In setCurrentSC with " << (int)currentScanController_;
	if(currentScanController_)
		return false;
	currentScanController_ = newScanController;
	qDebug()  << " and " << currentScanController_->scan();
	if(!currentScanController_->scan())
		return false;
	connect(currentScanController_, SIGNAL(finished()), this, SLOT(onCurrentScanControllerFinished()));
	emit currentScanControllerCreated();
	return true;
}

bool AMScanControllerSupervisor::deleteCurrentScanController(){
	if(!currentScanController_)
		return false;
	onCurrentScanControllerFinished();
	return true;
}

void AMScanControllerSupervisor::onCurrentScanControllerFinished(){
	emit currentScanControllerDestroyed();
	currentScanController_->deleteLater();
	currentScanController_ = NULL;
}
