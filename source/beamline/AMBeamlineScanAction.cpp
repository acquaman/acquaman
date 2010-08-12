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
		//connect(lCtrl, SIGNAL(finished()), this, SIGNAL(succeeded()));
		connect(lCtrl, SIGNAL(finished()), this, SLOT(scanSucceeded()));
		connect(lCtrl, SIGNAL(cancelled()), this, SLOT(scanCancelled()));
		connect(lCtrl, SIGNAL(progress(double,double)), this, SIGNAL(progress(double,double)));
		lCtrl->initialize();
		lCtrl->start();
		AMBeamlineActionItem::start();
	}
	else
		qDebug() << "Failed with no valid scan type";
		emit failed(101);
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
	emit failed(102);
}

void AMBeamlineScanAction::scanSucceeded(){
	running_ = false;
	emit succeeded();
}

AMBeamlineScanActionView::AMBeamlineScanActionView(AMBeamlineScanAction *scanAction, int index, QWidget *parent) :
		QFrame(parent)
//		QWidget(parent)
{
	index_ = index;
	inFocus_ = false;
	scanAction_ = scanAction;
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

	scanNameLabel_ = new QLabel();
	scanNameLabel_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
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
	functionButton_ = new QPushButton("X");
	functionButton_->setMaximumHeight(progressBar_->size().height());
	functionButton_->setFixedWidth(25);
	functionButton_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	connect(functionButton_, SIGNAL(clicked()), this, SLOT(onRemoveButtonClicked()));
	hl_ = new QHBoxLayout();
	hl_->addWidget(scanNameLabel_);
//	hl_->addWidget(progressBar_);
	hl_->addLayout(progressVL);
	hl_->addWidget(functionButton_, 0, Qt::AlignTop | Qt::AlignRight);
	setLayout(hl_);
	setLineWidth(1);
	setFrameStyle(QFrame::StyledPanel);
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

void AMBeamlineScanActionView::defocusItem(){
	inFocus_ = false;
	updateLook();
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
	progressBar_->setMaximum((int)total);
	progressBar_->setValue((int)elapsed);

	double secondsRemaining = total - elapsed;
	QTime tRemaining = QTime(0,0,0,0).addMSecs((int)1000*secondsRemaining);
	QString rStr = (tRemaining.hour() > 0) ? "h:mm:ss" : "m:ss" ;
	timeRemainingLabel_->setText(tRemaining.toString(rStr)+" Remaining");
}

void AMBeamlineScanActionView::onScanStarted(){
	functionButton_->setText("||");
	disconnect(functionButton_, SIGNAL(clicked()), this, SLOT(onRemoveButtonClicked()));
	connect(functionButton_, SIGNAL(clicked()), this, SLOT(onPauseButtonClicked()));
	updateLook();
}

void AMBeamlineScanActionView::onScanFinished(){
	progressBar_->setValue(progressBar_->maximum());

	progressBar_->setMaximum(100);
	progressBar_->setValue(100);
	timeRemainingLabel_->setText("Scan Complete");
	functionButton_->setText("Hide");
	updateLook();
	emit scanSuceeded(scanAction_);
}

void AMBeamlineScanActionView::onRemoveButtonClicked(){
	emit removeRequested(scanAction_);
}

void AMBeamlineScanActionView::onPauseButtonClicked(){
	if(scanAction_->isRunning()){
		functionButton_->setText(">");
		scanAction_->pause(true);
	}
	else{
		functionButton_->setText("||");
		scanAction_->pause(false);
	}
}

void AMBeamlineScanActionView::mousePressEvent(QMouseEvent *event){
	if (event->button() != Qt::LeftButton) {
		event->ignore();
		return;
	}
	qDebug() << "Detected mouse click";
	if(inFocus_)
		defocusItem();
	else{
		inFocus_ = true;
		updateLook();
		emit focusRequested(scanAction_);
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
