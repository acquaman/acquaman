#include "AMBeamlineScanAction.h"

#define AMBEAMLINEACTIONITEM_INVALID_SCAN_TYPE 101
#define AMBEAMLINEACTIONITEM_SCAN_CANCELLED 102
#define AMBEAMLINEACTIONITEM_CANT_SET_CURRENT_CONTROLLER 103

AMBeamlineScanAction::AMBeamlineScanAction(AMScanConfiguration *cfg, QString scanType, QString message, QObject *parent) :
	AMBeamlineActionItem(message, parent)
{
	cfg_ = cfg;
	scanType_ = scanType;
	type_ = "scanAction";
	keepOnCancel_ = true;
}

QString AMBeamlineScanAction::type() const{
	return AMBeamlineActionItem::type()+"."+type_;
}

bool AMBeamlineScanAction::isPaused() const{
	if(scanType_ == "SGMXASScan"){
		SGMXASDacqScanController *lCtrl = (SGMXASDacqScanController*)ctrl_;
		return lCtrl->isPaused();
	}
	return false;
}

void AMBeamlineScanAction::start(){
	if(scanType_ == "SGMXASScan"){
		SGMXASScanConfiguration* lCfg = (SGMXASScanConfiguration*)cfg_;
		SGMXASDacqScanController *lCtrl;
		if(keepOnCancel_){
			ctrl_ = new SGMXASDacqScanController( lCfg, this);
			if( !AMScanControllerSupervisor::scanControllerSupervisor()->setCurrentScanController(ctrl_) ){
				delete ctrl_;
				qDebug() << "Failed to set current scan controller";
				emit failed(AMBEAMLINEACTIONITEM_CANT_SET_CURRENT_CONTROLLER);
				return;
			}
		}
		lCtrl = (SGMXASDacqScanController*)AMScanControllerSupervisor::scanControllerSupervisor()->currentScanController();
		if(keepOnCancel_){
			connect(lCtrl, SIGNAL(finished()), this, SLOT(scanSucceeded()));
			connect(lCtrl, SIGNAL(cancelled()), this, SLOT(scanCancelled()));
			connect(lCtrl, SIGNAL(progress(double,double)), this, SIGNAL(progress(double,double)));
			keepOnCancel_ = false;
		}
		lCtrl->initialize();
		lCtrl->start();
		AMBeamlineActionItem::start();
	}
	else
		emit failed(AMBEAMLINEACTIONITEM_INVALID_SCAN_TYPE);
}

void AMBeamlineScanAction::cancel(){
	if(scanType_ == "SGMXASScan"){
		SGMXASDacqScanController *lCtrl = (SGMXASDacqScanController*)ctrl_;
		return lCtrl->cancel();
	}
}

void AMBeamlineScanAction::cancelButKeep(){
	keepOnCancel_ = true;
	cancel();
}

void AMBeamlineScanAction::pause(bool pause){
	if(scanType_ == "SGMXASScan"){
		SGMXASDacqScanController *lCtrl = (SGMXASDacqScanController*)ctrl_;
		if(pause)
			lCtrl->pause();
		else
			lCtrl->resume();
	}
}

void AMBeamlineScanAction::scanCancelled(){
	qDebug() << "Failed b/c of cancel";
	running_ = false;
	failed_ = true;
	if(keepOnCancel_)
		AMScanControllerSupervisor::scanControllerSupervisor()->deleteCurrentScanController();
	emit failed(AMBEAMLINEACTIONITEM_SCAN_CANCELLED);
}

void AMBeamlineScanAction::scanSucceeded(){
	running_ = false;
	succeeded_ = true;
	emit succeeded();
}

AMBeamlineScanActionView::AMBeamlineScanActionView(AMBeamlineScanAction *scanAction, int index, QWidget *parent) :
		AMBeamlineActionView(scanAction, index, parent)
{
	index_ = index;
	cancelLatch_ = false;
	viewType_ = "scanView";
	scanAction_ = scanAction;
	setMinimumHeight(NATURAL_ACTION_VIEW_HEIGHT);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	scanNameLabel_ = new QLabel();
	scanNameLabel_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	scanNameLabel_->setAlignment(Qt::AlignVCenter);
	updateScanNameLabel();

	progressBar_ = new QProgressBar();
	progressBar_->setMinimum(0);
	timeRemainingLabel_ = new QLabel("Scan Not Started");
	timeRemainingLabel_->setAlignment(Qt::AlignHCenter);
	QVBoxLayout *progressVL = new QVBoxLayout();
	progressVL->addWidget(progressBar_);
	progressVL->addWidget(timeRemainingLabel_);
	connect(scanAction_, SIGNAL(progress(double,double)), this, SLOT(updateProgressBar(double,double)));
	connect(scanAction_, SIGNAL(started()), this, SLOT(onScanStarted()));
	connect(scanAction_, SIGNAL(succeeded()), this, SLOT(onScanFinished()));
	connect(scanAction_, SIGNAL(failed(int)), this, SLOT(onScanFailed(int)));
	closeIcon_ = QIcon(":/window-close.png");
	stopIcon_ = QIcon(":/media-playback-stop-dark.png");
	startIcon_ = QIcon(":/media-playback-start-dark.png");
	pauseIcon_ = QIcon(":/media-playback-pause-dark.png");
	stopCancelButton_ = new QPushButton(closeIcon_, "");
	stopCancelButton_->setMaximumHeight(progressBar_->size().height());
	stopCancelButton_->setFixedWidth(25);
	stopCancelButton_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
	playPauseButton_ = new QPushButton(startIcon_, "");
	playPauseButton_->setMaximumHeight(progressBar_->size().height());
	playPauseButton_->setFixedWidth(25);
	playPauseButton_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
	playPauseButton_->setEnabled(false);
	connect(stopCancelButton_, SIGNAL(clicked()), this, SLOT(onStopCancelButtonClicked()));
	connect(playPauseButton_, SIGNAL(clicked()), this, SLOT(onPlayPauseButtonClicked()));
	hideButton_ = NULL;
	hl_ = new QHBoxLayout();
	hl_->addWidget(scanNameLabel_, 0, Qt::AlignTop | Qt::AlignLeft);
	hl_->addLayout(progressVL);
	hl_->addWidget(playPauseButton_, 0, Qt::AlignTop | Qt::AlignRight);
	hl_->addWidget(stopCancelButton_, 0, Qt::AlignTop | Qt::AlignRight);
	setLayout(hl_);
}

QString AMBeamlineScanActionView::viewType() const{
	return AMBeamlineActionView::viewType()+"."+viewType_;
}

void AMBeamlineScanActionView::setIndex(int index){
	index_ = index;
	updateScanNameLabel();
}

void AMBeamlineScanActionView::setAction(AMBeamlineScanAction *scanAction){
	disconnect(scanAction_, SIGNAL(progress(double,double)), this, SLOT(updateProgressBar(double,double)));
	disconnect(scanAction_, SIGNAL(started()), this, SLOT(onScanStarted()));
	disconnect(scanAction_, SIGNAL(succeeded()), this, SLOT(onScanFinished()));
	scanAction_ = scanAction;
	updateScanNameLabel();
	connect(scanAction_, SIGNAL(progress(double,double)), this, SLOT(updateProgressBar(double,double)));
	connect(scanAction_, SIGNAL(started()), this, SLOT(onScanStarted()));
	connect(scanAction_, SIGNAL(succeeded()), this, SLOT(onScanFinished()));
}

void AMBeamlineScanActionView::onInfoChanged(){

}

void AMBeamlineScanActionView::updateScanNameLabel(){
	QString scanName, tmpStr;
	if(index_ != -1){
		scanName.setNum(index_);
		scanName.append(". ");
	}
	scanName += "SGM XAS Scan from ";
	tmpStr.setNum( ((SGMXASScanConfiguration*)scanAction_->cfg())->start(0) );
	scanName.append(tmpStr+" to ");
	tmpStr.setNum( ((SGMXASScanConfiguration*)scanAction_->cfg())->end(((SGMXASScanConfiguration*)scanAction_->cfg())->count()-1) );
	scanName.append(tmpStr);
	scanNameLabel_->setText(scanName);
}

void AMBeamlineScanActionView::updateProgressBar(double elapsed, double total){
	if(cancelLatch_)
		return;

	progressBar_->setMaximum((int)total);
	progressBar_->setValue((int)elapsed);

	double secondsRemaining = total - elapsed;
	QTime tRemaining = QTime(0,0,0,0).addMSecs((int)1000*secondsRemaining);
	QString rStr = (tRemaining.hour() > 0) ? "h:mm:ss" : "m:ss" ;
	timeRemainingLabel_->setText(tRemaining.toString(rStr)+" Remaining");
}

void AMBeamlineScanActionView::onScanStarted(){
	cancelLatch_ = false;
	stopCancelButton_->setIcon(stopIcon_);
	playPauseButton_->setIcon(pauseIcon_);
	playPauseButton_->setEnabled(true);
	updateLook();
	emit scanStarted(scanAction_);
}

void AMBeamlineScanActionView::onScanFinished(){
	progressBar_->setValue(progressBar_->maximum());

	progressBar_->setMaximum(100);
	progressBar_->setValue(100);
	if(!cancelLatch_)
		timeRemainingLabel_->setText("Scan Complete");
	cancelLatch_ = false;
	disconnect(stopCancelButton_, SIGNAL(clicked()), this, SLOT(onStopCancelButtonClicked()));
	disconnect(playPauseButton_, SIGNAL(clicked()), this, SLOT(onPlayPauseButtonClicked()));
	hl_->removeWidget(stopCancelButton_);
	hl_->removeWidget(playPauseButton_);
	hideButton_ = new QPushButton("Hide");
	connect(hideButton_, SIGNAL(clicked()), this, SLOT(onHideButtonClicked()));
	hl_->addWidget(hideButton_, 0, Qt::AlignTop | Qt::AlignRight);
	updateLook();
	emit scanSuceeded(scanAction_);
}

void AMBeamlineScanActionView::onScanFailed(int explanation){
	if(explanation == 102){//102 is scan cancelled
		cancelLatch_ = true;
		stopCancelButton_->setIcon(closeIcon_);
		playPauseButton_->setIcon(startIcon_);
		playPauseButton_->setEnabled(false);
		timeRemainingLabel_->setText("Scan Cancelled");
		emit scanCancelled(scanAction_);
	}
}

void AMBeamlineScanActionView::onStopCancelButtonClicked(){
	if(scanAction_->isRunning()){
		scanAction_->pause(true);
		QMessageBox msgBox;
		msgBox.setText("Keep this data?");
		msgBox.setInformativeText("Do you wish to keep the data from this partially collected scan?");
		msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Save);
		int ret = msgBox.exec();
		switch (ret) {
		case QMessageBox::Save:
			scanAction_->cancelButKeep();
			break;
		case QMessageBox::Discard:
			scanAction_->cancel();
			break;
		case QMessageBox::Cancel:
			scanAction_->pause(false);
			break;
		default:
			// should never be reached
			break;
		}
//		scanAction_->cancel();
	}
	else{
		emit removeRequested(scanAction_);
	}
}

void AMBeamlineScanActionView::onPlayPauseButtonClicked(){
	if(scanAction_->isRunning() && !scanAction_->isPaused()){
		playPauseButton_->setIcon(startIcon_);
		scanAction_->pause(true);
		emit pauseRequested(scanAction_);
	}
	else if(scanAction_->isRunning() && scanAction_->isPaused()){
		playPauseButton_->setIcon(pauseIcon_);
		scanAction_->pause(false);
		emit resumeRequested(scanAction_);
	}
}

void AMBeamlineScanActionView::onHideButtonClicked(){
	emit hideRequested(scanAction_);
}

void AMBeamlineScanActionView::updateLook(){
	if(scanAction_->isRunning() || inFocus_)
		setFrameStyle(QFrame::Box);
	if(scanAction_->isRunning() && inFocus_)
		setStyleSheet("AMBeamlineScanActionView { background : rgb(198, 202, 230) }");
	else if(scanAction_->isRunning())
		setStyleSheet("AMBeamlineScanActionView { background : rgb(194, 230, 208) }");
	else if(inFocus_)
		setStyleSheet("AMBeamlineScanActionView { background : rgb(194, 210, 215) }");
	else{
		setStyleSheet("AMBeamlineScanActionView { background : rgb(230, 222, 214) }");
		setFrameStyle(QFrame::StyledPanel);
	}
}
