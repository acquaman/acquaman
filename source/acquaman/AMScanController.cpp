#include "AMScanController.h"

AMScanController::AMScanController(AMScanConfiguration *cfg, QObject *parent) :
	QObject(parent)
{
	generalCfg_ = pCfg_ = cfg;
	generalScan_ = NULL;
//	pScan_ = &generalScan_;
}

