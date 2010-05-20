#include "AMScanController.h"

AMScanController::AMScanController(AMScanConfiguration *cfg, QObject *parent) :
	QObject(parent)
{
	generalCfg_ = cfg;
	_pCfg_ = & generalCfg_;
	generalScan_ = NULL;
	_pScan_ = &generalScan_;
}

