#include "AMBeamlineScanAction.h"

#define AMBEAMLINEACTIONITEM_INVALID_SCAN_TYPE 101
#define AMBEAMLINEACTIONITEM_SCAN_CANCELLED 102
#define AMBEAMLINEACTIONITEM_CANT_SET_CURRENT_CONTROLLER 103

AMBeamlineScanAction::AMBeamlineScanAction(AMScanConfiguration *cfg, QString scanType, QObject *parent) :
	AMBeamlineActionItem(true, parent)
{
	cfg_ = cfg;
	ctrl_ = NULL;
	scanType_ = scanType;
	type_ = "scanAction";
	keepOnCancel_ = false;
	connect(SGMBeamline::sgm(), SIGNAL(beamlineScanningChanged(bool)), this, SLOT(onBeamlineScanningChanged(bool)));
	initialize();
}

QString AMBeamlineScanAction::type() const{
	return AMBeamlineActionItem::type()+"."+type_;
}

bool AMBeamlineScanAction::isRunning() const{
	return AMBeamlineActionItem::isRunning();// && !isPaused();
}

bool AMBeamlineScanAction::isPaused() const{
	if(!ctrl_)
		return false;
	//if(scanType_ == "SGMXASScan"){
	if(scanType_ == "DacqScan"){
			SGMXASDacqScanController *lCtrl = (SGMXASDacqScanController*)ctrl_;
			return lCtrl->isPaused();
	}
	return false;
}

void AMBeamlineScanAction::start(){
	if(!isReady()){
		qDebug() << "Not ready, connecting and waiting";
		connect(this, SIGNAL(ready(bool)), this, SLOT(delayedStart(bool)));
		return;
	}
	//if(scanType_ == "SGMXASScan"){
	if(scanType_ == "DacqScan"){
		qDebug() << "Ready, so get rolling";
		SGMXASScanConfiguration *sxsc = NULL;
		SGMFastScanConfiguration *sfsc = NULL;
		SGMXASDacqScanController *sxdc = NULL;
		SGMFastDacqScanController *sfdc = NULL;
		if( sxsc = qobject_cast<SGMXASScanConfiguration*>(cfg_) )
			sxdc = qobject_cast<SGMXASDacqScanController*>(ctrl_);
		else if( sfsc = qobject_cast<SGMFastScanConfiguration*>(cfg_) )
			sfdc = qobject_cast<SGMFastDacqScanController*>(ctrl_);
		else{
			setFailed(true, AMBEAMLINEACTIONITEM_INVALID_SCAN_TYPE);
			return;
		}
		if(!AMBeamlineActionItem::isReinitialized()){
			qDebug() << "Not reinitalized, creating new controller";
			if(sxsc){
				sxdc = new SGMXASDacqScanController( sxsc, this);
				ctrl_ = sxdc;
			}
			else if(sfsc){
				sfdc = new SGMFastDacqScanController( sfsc, this);
				ctrl_ = sfdc;
				qDebug() << "Looks like we set ctrl_ right" << (int)sfdc << " " << (int)ctrl_;
			}
			else{
				qDebug() << "(1)Honestly, how did we get here? We should have already failed. Okay, Acquaman, you've got me, we fail again.";
				setFailed(true, AMBEAMLINEACTIONITEM_INVALID_SCAN_TYPE);
				return;
			}
			if( !AMScanControllerSupervisor::scanControllerSupervisor()->setCurrentScanController(ctrl_) ){
				delete ctrl_;
				qDebug() << "Failed to set current scan controller";
				setFailed(true, AMBEAMLINEACTIONITEM_CANT_SET_CURRENT_CONTROLLER);
				return;
			}
			connect(ctrl_, SIGNAL(finished()), this, SLOT(onScanSucceeded()));
			connect(ctrl_, SIGNAL(cancelled()), this, SLOT(onScanCancelled()));
			connect(ctrl_, SIGNAL(started()), this, SLOT(onScanStarted()));
			connect(ctrl_, SIGNAL(progress(double,double)), this, SIGNAL(progress(double,double)));
		}
		else
			qDebug() << "Reinitialized, no controller creation";
		//((SGMXASDacqScanController*)ctrl_)->initialize(); //Can I make a pure virtual initialize in AMScanController?
		if(sxdc)
			sxdc->initialize();
		else if(sfdc)
			sfdc->initialize();
		else{
			qDebug() << "(2)Honestly, how did we get here? We should have already failed. Okay, Acquaman, you've got me, we fail again.";
			setFailed(true, AMBEAMLINEACTIONITEM_INVALID_SCAN_TYPE);
			return;
		}
		ctrl_->start();
	}
	else
		setFailed(true, AMBEAMLINEACTIONITEM_INVALID_SCAN_TYPE);
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

void AMBeamlineScanAction::reset(bool delayInitialized){
	qDebug() << "Reseting with keepOnCancel " << keepOnCancel_;
	((SGMXASDacqScanController*)ctrl_)->reinitialize(!keepOnCancel_);
	AMBeamlineActionItem::reset(true);
	initialize();
}

void AMBeamlineScanAction::cleanup(){
	if( ctrl_ == AMScanControllerSupervisor::scanControllerSupervisor()->currentScanController() )
		AMScanControllerSupervisor::scanControllerSupervisor()->deleteCurrentScanController();
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

void AMBeamlineScanAction::initialize(){
	AMBeamlineActionItem::initialize();
	onBeamlineScanningChanged(SGMBeamline::sgm()->isScanning());
}

void AMBeamlineScanAction::delayedStart(bool ready){
	start();
}

void AMBeamlineScanAction::onScanStarted(){
	setStarted(true);
}

void AMBeamlineScanAction::onScanCancelled(){
	setFailed(true, AMBEAMLINEACTIONITEM_SCAN_CANCELLED);
}

void AMBeamlineScanAction::onScanSucceeded(){
	setSucceeded(true);
}

void AMBeamlineScanAction::onBeamlineScanningChanged(bool isScanning){
	setReady(!isScanning);
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
	if(qobject_cast<SGMXASScanConfiguration*>(scanAction_->cfg())){
		scanName += "SGM XAS Scan from ";
		tmpStr.setNum( ((SGMXASScanConfiguration*)scanAction_->cfg())->start(0) );
		scanName.append(tmpStr+" to ");
		tmpStr.setNum( ((SGMXASScanConfiguration*)scanAction_->cfg())->end(((SGMXASScanConfiguration*)scanAction_->cfg())->count()-1) );
		scanName.append(tmpStr);
		scanNameLabel_->setText(scanName);
	}
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
	stopCancelButton_->hide();
	playPauseButton_->hide();
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
	qDebug() << "Running " << scanAction_->isRunning() << " started " << scanAction_->hasStarted() << " finished " << scanAction_->hasFinished() << " paused " << scanAction_->isPaused();
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
