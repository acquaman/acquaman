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

AMBeamlineScanActionView::AMBeamlineScanActionView(AMBeamlineScanAction *scanAction, QWidget *parent) :
		QWidget(parent)
{
	QString scanName, tmpStr;
	scanName = "SGM XAS Scan from";
	tmpStr.setNum( ((SGMXASScanConfiguration*)scanAction->cfg())->start(0) );
	scanName.append(tmpStr+" to ");
	tmpStr.setNum( ((SGMXASScanConfiguration*)scanAction->cfg())->end(((SGMXASScanConfiguration*)scanAction->cfg())->count()-1) );
	scanName.append(tmpStr);
	scanNameLabel_ = new QLabel(scanName);
	progressBar_ = new QProgressBar();
	hl_ = new QHBoxLayout();
	hl_->addWidget(scanNameLabel_);
	hl_->addWidget(progressBar_);
	setLayout(hl_);
}
