#include "AMBeamlineControlMoveAction.h"

AMBeamlineControlMoveAction::AMBeamlineControlMoveAction(AMControl *control, QString message, QObject *parent) :
	AMBeamlineActionItem(message, parent)
{
	ready_ = false;
	control_ = control;
	type_ = "controlMoveAction";
	if(control_){
		setpoint_ = control_->value();
//		connect(control_, SIGNAL(moveSucceeded()), this, SLOT(onSucceeded()));
//		connect(control_, SIGNAL(moveFailed(int)), this, SLOT(onFailed(int)));
//		connect(control_, SIGNAL(moveStarted()), this, SLOT(onStarted()));
	}
	else
		setpoint_ = 0;
}

QString AMBeamlineControlMoveAction::type() const{
	return AMBeamlineActionItem::type()+"."+type_;
}

AMControl* AMBeamlineControlMoveAction::control(){
	return control_;
}

double AMBeamlineControlMoveAction::setpoint(){
	return setpoint_;
}

void AMBeamlineControlMoveAction::start(){
	if(control_ && control_->canMove() && ready_){
		connect(control_, SIGNAL(moveSucceeded()), this, SLOT(onSucceeded()));
		connect(control_, SIGNAL(moveFailed(int)), this, SLOT(onFailed(int)));
		connect(control_, SIGNAL(moveStarted()), this, SLOT(onStarted()));
		control_->move(setpoint_);
//		AMBeamlineActionItem::start();
	}
}

void AMBeamlineControlMoveAction::cancel(){
	if(control_ && running_){
		control_->stop();
	}
}

void AMBeamlineControlMoveAction::setControl(AMControl *control){
//	if(control_){
//		disconnect(control_, SIGNAL(moveSucceeded()), this, SLOT(onSucceeded()));
//		disconnect(control_, SIGNAL(moveFailed(int)), this, SLOT(onFailed(int)));
//		disconnect(control_, SIGNAL(moveStarted()), this, SLOT(onStarted()));	}
	control_ = control;
//	if(control_){
//		connect(control_, SIGNAL(moveSucceeded()), this, SLOT(onSucceeded()));
//		connect(control_, SIGNAL(moveFailed(int)), this, SLOT(onFailed(int)));
//		connect(control_, SIGNAL(moveStarted()), this, SLOT(onStarted()));	}
}

bool AMBeamlineControlMoveAction::setSetpoint(double setpoint){
	bool retVal;
	ready_ = false;
	if(!control_)
		retVal = false;
	else if(!control_->canMove())
		retVal = false;
	else if(control_->valueOutOfRange(setpoint))
		retVal = false;
	else{
		setpoint_ = setpoint;
		ready_ = true;
		emit ready(true);
		retVal = true;
	}
	return retVal;
}

void AMBeamlineControlMoveAction::onStarted(){
	running_ = true;
	emit started();
}

void AMBeamlineControlMoveAction::onSucceeded(){
	running_ = false;
	disconnect(control_, SIGNAL(moveSucceeded()), this, SLOT(onSucceeded()));
	disconnect(control_, SIGNAL(moveFailed(int)), this, SLOT(onFailed(int)));
	disconnect(control_, SIGNAL(moveStarted()), this, SLOT(onStarted()));
	emit succeeded();
}

void AMBeamlineControlMoveAction::onFailed(int explanation){
	running_ = false;
	disconnect(control_, SIGNAL(moveSucceeded()), this, SLOT(onSucceeded()));
	disconnect(control_, SIGNAL(moveFailed(int)), this, SLOT(onFailed(int)));
	disconnect(control_, SIGNAL(moveStarted()), this, SLOT(onStarted()));
	emit failed(explanation);
}

AMBeamlineControlMoveActionView::AMBeamlineControlMoveActionView(AMBeamlineControlMoveAction *moveAction, int index, QWidget *parent) :
		AMBeamlineActionView(moveAction, index, parent)
{
	moveAction_ = moveAction;
	viewType_ = "controlMoveView";

	setMinimumHeight(NATURAL_ACTION_VIEW_HEIGHT);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

	infoLabel_ = new QLabel();
	infoLabel_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
	onInfoChanged();

	progressBar_ = new QProgressBar();
	progressBar_->setMinimum(0);
	timeRemainingLabel_ = new QLabel("Move Not Started");
	timeRemainingLabel_->setAlignment(Qt::AlignHCenter);
	QVBoxLayout *progressVL = new QVBoxLayout();
	progressVL->addWidget(progressBar_);
	progressVL->addWidget(timeRemainingLabel_);
	//connect(scanAction_, SIGNAL(progress(double,double)), this, SLOT(updateProgressBar(double,double)));
	connect(moveAction_, SIGNAL(started()), this, SLOT(onStarted()));
	connect(moveAction_, SIGNAL(succeeded()), this, SLOT(onSucceeded()));
	connect(moveAction_, SIGNAL(failed(int)), this, SLOT(onFailed(int)));
	closeIcon_ = QIcon(":/window-close.png");
	stopIcon_ = QIcon(":/media-playback-stop-dark.png");
	startIcon_ = QIcon(":/media-playback-start-dark.png");
	pauseIcon_ = QIcon(":/media-playback-pause-dark.png");
	stopCancelButton_ = new QPushButton(closeIcon_, "");
	stopCancelButton_->setMaximumHeight(progressBar_->size().height());
	stopCancelButton_->setFixedWidth(25);
	stopCancelButton_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	playPauseButton_ = new QPushButton(startIcon_, "");
	playPauseButton_->setMaximumHeight(progressBar_->size().height());
	playPauseButton_->setFixedWidth(25);
	playPauseButton_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	playPauseButton_->setEnabled(false);
	connect(stopCancelButton_, SIGNAL(clicked()), this, SLOT(onStopCancelButtonClicked()));
	connect(playPauseButton_, SIGNAL(clicked()), this, SLOT(onPlayPauseButtonClicked()));
	hideButton_ = NULL;
	hl_ = new QHBoxLayout();
	hl_->addWidget(infoLabel_);
	hl_->addLayout(progressVL);
	hl_->addWidget(playPauseButton_, 0, Qt::AlignTop | Qt::AlignRight);
	hl_->addWidget(stopCancelButton_, 0, Qt::AlignTop | Qt::AlignRight);
	setLayout(hl_);
}

QString AMBeamlineControlMoveActionView::viewType() const{
	return AMBeamlineActionView::viewType()+"."+viewType_;
}

void AMBeamlineControlMoveActionView::setIndex(int index){
	index_ = index;
	onInfoChanged();
}

void AMBeamlineControlMoveActionView::setAction(AMBeamlineControlMoveAction *moveAction){
	disconnect(moveAction_, SIGNAL(started()), this, SLOT(onStarted()));
	disconnect(moveAction_, SIGNAL(succeeded()), this, SLOT(onSucceeded()));
	disconnect(moveAction_, SIGNAL(failed(int)), this, SLOT(onFailed(int)));
	moveAction_ = moveAction;
	connect(moveAction_, SIGNAL(started()), this, SLOT(onStarted()));
	connect(moveAction_, SIGNAL(succeeded()), this, SLOT(onSucceeded()));
	connect(moveAction_, SIGNAL(failed(int)), this, SLOT(onFailed(int)));
}

void AMBeamlineControlMoveActionView::onInfoChanged(){
	QString infoText, tmpStr;
	if(index_ != -1){
		infoText.setNum(index_);
		infoText.append(". ");
	}
	infoText += " Moving ";
	QString adjName = moveAction_->control()->name();
	adjName.replace(QRegExp("([A-Z])"), " \\1");
	QChar fCap = adjName[0].toUpper();
	adjName.replace(0, 1, fCap);
	//infoText += moveAction_->control()->name();
	infoText += adjName;
	infoText += " to ";
	tmpStr.setNum( moveAction_->setpoint() );
	infoText.append(tmpStr);
	infoLabel_->setText(infoText);
}

void AMBeamlineControlMoveActionView::onStopCancelButtonClicked(){

}

void AMBeamlineControlMoveActionView::onPlayPauseButtonClicked(){

}

void AMBeamlineControlMoveActionView::onHideButtonClicked(){

}

void AMBeamlineControlMoveActionView::onStarted(){
//	cancelLatch_ = false;
	stopCancelButton_->setIcon(stopIcon_);
	playPauseButton_->setIcon(pauseIcon_);
	playPauseButton_->setEnabled(true);
//	updateLook();
	emit actionStarted(moveAction_);
}

void AMBeamlineControlMoveActionView::onSucceeded(){
	progressBar_->setValue(progressBar_->maximum());

	progressBar_->setMaximum(100);
	progressBar_->setValue(100);
//	if(!cancelLatch_)
//		timeRemainingLabel_->setText("Scan Complete");
//	cancelLatch_ = false;
	disconnect(stopCancelButton_, SIGNAL(clicked()), this, SLOT(onStopCancelButtonClicked()));
	disconnect(playPauseButton_, SIGNAL(clicked()), this, SLOT(onPlayPauseButtonClicked()));
	hl_->removeWidget(stopCancelButton_);
	hl_->removeWidget(playPauseButton_);
	hideButton_ = new QPushButton("Hide");
	connect(hideButton_, SIGNAL(clicked()), this, SLOT(onHideButtonClicked()));
	hl_->addWidget(hideButton_, 0, Qt::AlignTop | Qt::AlignRight);
//	updateLook();
	emit actionSucceeded(moveAction_);
}

void AMBeamlineControlMoveActionView::onFailed(int explanation){
//	if(explanation == 102){//102 is cancelled
//		cancelLatch_ = true;
		stopCancelButton_->setIcon(closeIcon_);
		playPauseButton_->setIcon(startIcon_);
		playPauseButton_->setEnabled(false);
		timeRemainingLabel_->setText("Move Cancelled");
//		emit scanCancelled(scanAction_);
//	}
}
