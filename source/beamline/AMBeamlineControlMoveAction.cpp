/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "AMBeamlineControlMoveAction.h"

AMBeamlineControlMoveAction::AMBeamlineControlMoveAction(AMControl *control, QObject *parent) :
	AMBeamlineActionItem(parent)
{
	control_ = NULL;
	type_ = "controlMoveAction";
	if(control){
		setControl(control);
		setpoint_ = control_->value();
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
//	if(control_ && control_->canMove() && ready_){
	qDebug() << "Trying to start " << (int)this;
	if(isReady()){
		qDebug() << "Ready so start " << (int)this;
		connect(this, SIGNAL(finished()), this, SLOT(onFinished()));
		connect(control_, SIGNAL(moveSucceeded()), this, SLOT(onSucceeded()));
		connect(control_, SIGNAL(moveFailed(int)), this, SLOT(onFailed(int)));
		connect(control_, SIGNAL(moveStarted()), this, SLOT(onStarted()));
		startPoint_ = control_->value();
		connect(&progressTimer_, SIGNAL(timeout()), this, SLOT(calculateProgress()));
		progressTimer_.start(500);
		control_->move(setpoint_);
//		AMBeamlineActionItem::start();
	}
	else
		connect(this, SIGNAL(ready(bool)), this, SLOT(delayedStart(bool)));
}

void AMBeamlineControlMoveAction::cancel(){
	if(control_ && isRunning()){
		control_->stop();
	}
}

void AMBeamlineControlMoveAction::setControl(AMControl *control){
	if(control_){
		disconnect(control_, SIGNAL(movingChanged(bool)), this, SLOT(onMovingChanged(bool)) );
		disconnect(control_, SIGNAL(connected(bool)), this, SLOT(onConnected(bool)));
	}
	control_ = control;
	if(control_){
		connect(control_, SIGNAL(movingChanged(bool)), this, SLOT(onMovingChanged(bool)) );
		connect(control_, SIGNAL(connected(bool)), this, SLOT(onConnected(bool)));
	}
	if(!control)
		setpoint_ = 0;
	else if(control_->valueOutOfRange(setpoint_))
		setpoint_ = control_->value();
	checkReady();
}

bool AMBeamlineControlMoveAction::setSetpoint(double setpoint){
	if(!control_ || !control_->canMove() || control_->valueOutOfRange(setpoint))
		return false;
	else{
		setpoint_ = setpoint;
		return true;
	}
}

void AMBeamlineControlMoveAction::delayedStart(bool ready){
	if(ready){
		disconnect(this, SIGNAL(ready(bool)), this, SLOT(delayedStart(bool)));
		start();
	}
}

void AMBeamlineControlMoveAction::onMovingChanged(bool moving){
	if(!hasStarted())
		checkReady();
	else{
		/// \todo check for finsihed or stopped.
	}
}

void AMBeamlineControlMoveAction::onConnected(bool connected){
	if(!hasStarted())
		checkReady();
}

void AMBeamlineControlMoveAction::checkReady(){
	if(!control_)
		setReady(false);
	else{
		qDebug() << "CMA says " << (int)this << control_->isConnected() << !control_->isMoving();
		setReady(control_->isConnected() && !control_->isMoving());
	}
}

void AMBeamlineControlMoveAction::onStarted(){
	setStarted(true);
}

void AMBeamlineControlMoveAction::onSucceeded(){
	qDebug() << (int)this << "SUCEEDED";
	disconnect(control_, 0, this, 0);
	setSucceeded(true);
}

void AMBeamlineControlMoveAction::onFailed(int explanation){
	qDebug() << (int)this << "FAILED";
	setFailed(true, explanation);
}

void AMBeamlineControlMoveAction::onFinished(){
	qDebug() << (int)this << "FINISHED";
	progressTimer_.stop();
	emit progress(1, 1);
}

void AMBeamlineControlMoveAction::calculateProgress(){
	qDebug() << "Calculate progress for " << (int)this;
	if(control_)
		emit progress(fabs(control_->value()-startPoint_), fabs(setpoint_-startPoint_));
}

AMBeamlineControlMoveActionView::AMBeamlineControlMoveActionView(AMBeamlineControlMoveAction *moveAction, int index, QWidget *parent) :
		AMBeamlineActionView(moveAction, index, parent)
{
	//moveAction_ = moveAction;
	moveAction_ = NULL;
	setAction(moveAction);
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
	if(moveAction_){
		disconnect(moveAction_, SIGNAL(progress(double,double)), this, SLOT(updateProgressBar(double,double)));
		disconnect(moveAction_, SIGNAL(started()), this, SLOT(onStarted()));
		disconnect(moveAction_, SIGNAL(succeeded()), this, SLOT(onSucceeded()));
		disconnect(moveAction_, SIGNAL(failed(int)), this, SLOT(onFailed(int)));
	}
	moveAction_ = moveAction;
	if(moveAction_){
		connect(moveAction_, SIGNAL(progress(double,double)), this, SLOT(updateProgressBar(double,double)));
		connect(moveAction_, SIGNAL(started()), this, SLOT(onStarted()));
		connect(moveAction_, SIGNAL(succeeded()), this, SLOT(onSucceeded()));
		connect(moveAction_, SIGNAL(failed(int)), this, SLOT(onFailed(int)));
	}
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

void AMBeamlineControlMoveActionView::updateProgressBar(double elapsed, double total){
	if(elapsed == total){
		progressBar_->setMaximum(100);
		progressBar_->setValue(100);
		timeRemainingLabel_->setText("Move Complete");
		return;
	}
	if(moveAction_->hasFailed())
		return;
	progressBar_->setMaximum((int)total);
	progressBar_->setValue((int)elapsed);

	double secondsRemaining = total - elapsed;
	QTime tRemaining = QTime(0,0,0,0).addMSecs((int)1000*secondsRemaining);
	QString rStr = (tRemaining.hour() > 0) ? "h:mm:ss" : "m:ss" ;
	timeRemainingLabel_->setText(tRemaining.toString(rStr)+" Remaining");
}

void AMBeamlineControlMoveActionView::onStopCancelButtonClicked(){

}

void AMBeamlineControlMoveActionView::onPlayPauseButtonClicked(){

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
	qDebug() << "In move action (view) succeeded";
	progressBar_->setValue(progressBar_->maximum());

	progressBar_->setMaximum(100);
	progressBar_->setValue(100);
	timeRemainingLabel_->setText("Move Complete");
	disconnect(stopCancelButton_, SIGNAL(clicked()), this, SLOT(onStopCancelButtonClicked()));
	disconnect(playPauseButton_, SIGNAL(clicked()), this, SLOT(onPlayPauseButtonClicked()));
	hl_->removeWidget(stopCancelButton_);
	stopCancelButton_->hide();
	//delete hl_->takeAt(hl_->indexOf(stopCancelButton_));
	hl_->removeWidget(playPauseButton_);
	playPauseButton_->hide();
	//delete hl_->takeAt(hl_->indexOf(playPauseButton_));
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
