#include "AMBeamlineScanAction.h"

AMBeamlineScanAction::AMBeamlineScanAction(AMScanConfiguration *cfg, QString scanType, QString message, QObject *parent) :
	AMBeamlineActionItem(message, parent)
{
	cfg_ = cfg;
	scanType_ = scanType;
	type_ = "scanAction";
}

QString AMBeamlineScanAction::type() const{
	return AMBeamlineActionItem::type()+"."+type_;
}

void AMBeamlineScanAction::start(){
	if(scanType_ == "SGMXASScan"){
		SGMXASScanConfiguration* lCfg = (SGMXASScanConfiguration*)cfg_;
		ctrl_ = new SGMXASDacqScanController( lCfg, this);
		SGMXASDacqScanController *lCtrl = (SGMXASDacqScanController*)ctrl_;
		connect(lCtrl, SIGNAL(finished()), this, SIGNAL(succeeded()));
		connect(lCtrl, SIGNAL(cancelled()), this, SLOT(scanCancelled()));
		connect(lCtrl, SIGNAL(progress(double,double)), this, SIGNAL(progress(double,double)));
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

AMBeamlineScanActionView::AMBeamlineScanActionView(AMBeamlineScanAction *scanAction, int index, QWidget *parent) :
		QWidget(parent)
{
	index_ = index;
	scanAction_ = scanAction;
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

	scanNameLabel_ = new QLabel();
	scanNameLabel_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
	updateScanNameLabel();

	progressBar_ = new QProgressBar();
	progressBar_->setMinimum(0);
	timeRemainingLabel_ = new QLabel("");
	QVBoxLayout *progressVL = new QVBoxLayout();
	progressVL->addWidget(progressBar_);
	progressVL->addWidget(timeRemainingLabel_);
	connect(scanAction_, SIGNAL(progress(double,double)), this, SLOT(updateProgressBar(double,double)));
	connect(scanAction_, SIGNAL(succeeded()), this, SLOT(onScanFinished()));
	hl_ = new QHBoxLayout();
	hl_->addWidget(scanNameLabel_);
//	hl_->addWidget(progressBar_);
	hl_->addLayout(progressVL);
	setLayout(hl_);
}

void AMBeamlineScanActionView::setIndex(int index){
	index_ = index;
	updateScanNameLabel();
}

void AMBeamlineScanActionView::updateScanNameLabel(){
	QString scanName, tmpStr;
	scanName.setNum(index_);
	scanName += ". SGM XAS Scan from ";
	tmpStr.setNum( ((SGMXASScanConfiguration*)scanAction_->cfg())->start(0) );
	scanName.append(tmpStr+" to ");
	tmpStr.setNum( ((SGMXASScanConfiguration*)scanAction_->cfg())->end(((SGMXASScanConfiguration*)scanAction_->cfg())->count()-1) );
	scanName.append(tmpStr);
	scanNameLabel_->setText(scanName);
}

void AMBeamlineScanActionView::updateProgressBar(double elapsed, double total){
	progressBar_->setMaximum((int)total);
	progressBar_->setValue((int)elapsed);

	double secondsRemaining = total - elapsed;
	QTime tRemaining = QTime(0,0,0,0).addMSecs((int)1000*secondsRemaining);
	QString rStr = (tRemaining.hour() > 0) ? "h:mm:ss" : "m:ss" ;
	timeRemainingLabel_->setText(tRemaining.toString(rStr)+" Remaining");
}

void AMBeamlineScanActionView::onScanFinished(){
	qDebug() << "Wants to set 100%" << progressBar_->maximum();
	progressBar_->setValue(progressBar_->maximum());
	qDebug() << "Now " << progressBar_->value() << " of " << progressBar_->maximum();

	progressBar_->setMaximum(100);
	progressBar_->setValue(100);
	timeRemainingLabel_->setText("Scan Complete");
}
