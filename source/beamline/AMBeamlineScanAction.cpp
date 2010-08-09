#include "AMBeamlineScanAction.h"

AMBeamlineScanAction::AMBeamlineScanAction(AMScanConfiguration *cfg, QString type, QString message, QObject *parent) :
	AMBeamlineActionItem(message, parent)
{
	cfg_ = cfg;
	type_ = type;
}

void AMBeamlineScanAction::start(){
	if(type_ == "SGMXASScan"){
		SGMXASScanConfiguration* lCfg = (SGMXASScanConfiguration*)cfg_;
		ctrl_ = new SGMXASDacqScanController( lCfg, this);
		SGMXASDacqScanController *lCtrl = (SGMXASDacqScanController*)ctrl_;
		connect(lCtrl, SIGNAL(finished()), this, SIGNAL(succeeded()));
		connect(lCtrl, SIGNAL(cancelled()), this, SLOT(scanCancelled()));
		lCtrl->initialize();
		lCtrl->start();
	}
	else
		qDebug() << "Failed with no valid scan type";
		emit failed(101);
}

void AMBeamlineScanAction::scanCancelled(){
	qDebug() << "Failed b/c of cancel";
	emit failed(102);
}
