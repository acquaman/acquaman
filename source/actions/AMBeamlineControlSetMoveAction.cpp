/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "AMBeamlineControlSetMoveAction.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QToolButton>
#include <QPalette>
#include <QApplication>
#include <QStyle>


AMBeamlineControlSetMoveAction::AMBeamlineControlSetMoveAction(AMControlSet *controlSet, QObject *parent) :
		AMBeamlineActionItem(parent)
{
	controlSet_ = 0; //NULL
	numSucceeded_ = 0;
	if(controlSet)
		setControlSet(controlSet);
}

AMBeamlineActionItemView* AMBeamlineControlSetMoveAction::createView(int index){
	return new AMBeamlineControlSetMoveActionView(this, index);
}

AMBeamlineActionItem* AMBeamlineControlSetMoveAction::createCopy() const{
	if(controlSet_){
		AMBeamlineControlSetMoveAction *retVal = new AMBeamlineControlSetMoveAction(controlSet_);
		retVal->setSetpoint(setpoint_);
		return retVal;
	}
	return 0; //NULL
}

AMControlSet* AMBeamlineControlSetMoveAction::controlSet(){
	return controlSet_;
}

AMControlInfoList AMBeamlineControlSetMoveAction::setpoint(){
	return setpoint_;
}

void AMBeamlineControlSetMoveAction::start(){
	if(isReady()){
		connect(this, SIGNAL(finished()), this, SLOT(onFinished()));
		for(int x = 0; x < controlSet_->count(); x++){
			connect(controlSet_->at(x), SIGNAL(moveSucceeded()), this, SLOT(onSucceeded()));
			connect(controlSet_->at(x), SIGNAL(moveFailed(int)), this, SLOT(onFailed(int)));
			connect(controlSet_->at(x), SIGNAL(moveStarted()), this, SLOT(onStarted()));
		}
		startpoint_ = controlSet_->toInfoList();
		connect(&progressTimer_, SIGNAL(timeout()), this, SLOT(calculateProgress()));
		progressTimer_.start(500);
//		qDebug() << "Setpoint is " << setpoint_;
		controlSet_->setFromInfoList(setpoint_);
	}
	else
		connect(this, SIGNAL(ready(bool)), this, SLOT(delayedStart(bool)));
}

void AMBeamlineControlSetMoveAction::cancel(){
	if(controlSet_ && isRunning())
		for(int x = 0; x < controlSet_->count(); x++)
			controlSet_->at(x)->stop();
}

void AMBeamlineControlSetMoveAction::setControlSet(AMControlSet *controlSet){
	if(controlSet_)
		for(int x = 0; x < controlSet_->count(); x++)
			disconnect(controlSet_->at(x), 0, this, 0);
	controlSet_ = controlSet;
	if(controlSet_){
		for(int x = 0; x < controlSet_->count(); x++){
			connect(controlSet_->at(x), SIGNAL(movingChanged(bool)), this, SLOT(onMovingChanged(bool)));
			connect(controlSet_->at(x), SIGNAL(connected(bool)), this, SLOT(onConnected(bool)));
		}
	}
	checkReady();
}

bool AMBeamlineControlSetMoveAction::setSetpoint(const AMControlInfoList &setpoint){
	if(!controlSet_)
		return false;
	setpoint_ = setpoint;
	fullSetpoint_ = controlSet_->toInfoList();
	for(int x = 0; x < fullSetpoint_.count(); x++){
		for(int y = 0; y < setpoint_.count(); y++)
			if(fullSetpoint_.at(x).name() == setpoint_.at(y).name()){
				fullSetpoint_[x].setValue(setpoint_.at(y).value());
//				qDebug() << "Setting " << fullSetpoint_.at(x).name() << " to " << fullSetpoint_.at(x).value() << " with tolerance " << fullSetpoint_.at(x).tolerance();
			}
	}
	return true;
}

void AMBeamlineControlSetMoveAction::cleanup(){

}

void AMBeamlineControlSetMoveAction::delayedStart(bool ready){
	if(ready){
		disconnect(this, SIGNAL(ready(bool)), this, SLOT(delayedStart(bool)));
		start();
	}
}

void AMBeamlineControlSetMoveAction::onMovingChanged(bool moving){
	if(!hasStarted() && moving)
		setReady(false);
	else if(!hasStarted())
		checkReady();
	else{
		/// \todo check for finished or stopped
	}
}

void AMBeamlineControlSetMoveAction::onConnected(bool connected){
	if(!hasStarted() && !connected)
		setReady(false);
	else if(!hasStarted())
		checkReady();
}

void AMBeamlineControlSetMoveAction::checkReady(){
	if(!controlSet_ || !controlSet_->isConnected())
		setReady(false);
	else{
		for(int x = 0; x < controlSet_->count(); x++)
			if(controlSet_->at(x)->isMoving())
				setReady(false);
	}
	setReady(true);
}

void AMBeamlineControlSetMoveAction::onStarted(){
	if(!hasStarted())
		setStarted(true);
}

void AMBeamlineControlSetMoveAction::onSucceeded(){
	numSucceeded_++;
	if(numSucceeded_ != controlSet_->count())
		return;
	for(int x = 0; x < controlSet_->count(); x++){
//		qDebug() << "Succeeded at " << x << " " << controlSet_->at(x)->name();
		disconnect(controlSet_->at(x), 0, this, 0);
	}
	setSucceeded(true);
}

void AMBeamlineControlSetMoveAction::onFailed(int explanation){
	if(VERBOSE_ACTION_ITEMS){
		AMControl *tmpCtrl = (AMControl*)QObject::sender();
		qDebug() << "ControlSetMoveAction thinks someone failed " << tmpCtrl->name() << tmpCtrl->value() << tmpCtrl->setpoint() << tmpCtrl->tolerance() << " as " << explanation;
	}
	setFailed(true, explanation);
}

void AMBeamlineControlSetMoveAction::onFinished(){
	progressTimer_.stop();
	emit progress(1.0, 1.0);
}

void AMBeamlineControlSetMoveAction::calculateProgress(){
	if(!controlSet_)
		return;
	double avgPercent, iPercent;
	double csCount = (double)controlSet_->count();
	for(int x = 0; x < controlSet_->count(); x++){
		if( fabs(fullSetpoint_.at(x).value() - startpoint_.at(x).value()) < 0.0001 )
			iPercent = 100;
		else
			iPercent = (fabs(controlSet_->at(x)->value()-startpoint_.at(x).value())/fabs(fullSetpoint_.at(x).value() - startpoint_.at(x).value())*100);
		avgPercent += iPercent/csCount;
	}
	emit progress(avgPercent, 100);
}

AMBeamlineControlSetMoveActionView::AMBeamlineControlSetMoveActionView(AMBeamlineControlSetMoveAction *controlSetAction, int index, QWidget *parent) :
		AMBeamlineActionItemView(controlSetAction, index, parent)
{
	controlSetAction_ = 0; //NULL
//	setAction(controlSetAction);

	controlSetAction_ = controlSetAction;

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
	connect(controlSetAction_, SIGNAL(progress(double,double)), this, SLOT(updateProgressBar(double,double)));
	connect(controlSetAction_, SIGNAL(started()), this, SLOT(onStarted()));
	connect(controlSetAction_, SIGNAL(succeeded()), this, SLOT(onSucceeded()));
	connect(controlSetAction_, SIGNAL(failed(int)), this, SLOT(onFailed(int)));
	connect(controlSetAction_, SIGNAL(previousChanged()), this, SLOT(onPreviousNextChanged()));
	connect(controlSetAction_, SIGNAL(nextChanged()), this, SLOT(onPreviousNextChanged()));
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
	moveActionUpButton_ = new QToolButton();
	moveActionUpButton_->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowUp));
	moveActionUpButton_->setEnabled(false);
	moveActionDownButton_ = new QToolButton();
	moveActionDownButton_->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowDown));
	moveActionDownButton_->setEnabled(false);
	connect(stopCancelButton_, SIGNAL(clicked()), this, SLOT(onStopCancelButtonClicked()));
	connect(playPauseButton_, SIGNAL(clicked()), this, SLOT(onPlayPauseButtonClicked()));
	connect(moveActionUpButton_, SIGNAL(clicked()), this, SLOT(onMoveUpButtonClicked()));
	connect(moveActionDownButton_, SIGNAL(clicked()), this, SLOT(onMoveDownButtonClicked()));
	hl_ = new QHBoxLayout();
	hl_->addWidget(infoLabel_);
	hl_->addLayout(progressVL);
	hl_->addWidget(playPauseButton_, 0, Qt::AlignTop | Qt::AlignRight);
	hl_->addWidget(stopCancelButton_, 0, Qt::AlignTop | Qt::AlignRight);
	QVBoxLayout *swapVL = new QVBoxLayout();
	swapVL->addWidget(moveActionUpButton_);
	swapVL->addWidget(moveActionDownButton_);
	swapVL->setContentsMargins(0,0,0,0);
	swapVL->setSpacing(0);
	hl_->addLayout(swapVL);
	setLayout(hl_);
	onPreviousNextChanged();
}

void AMBeamlineControlSetMoveActionView::setIndex(int index){
	index_ = index;
	onInfoChanged();
}

void AMBeamlineControlSetMoveActionView::setAction(AMBeamlineActionItem *action){
	AMBeamlineControlSetMoveAction *controlSetAction = qobject_cast<AMBeamlineControlSetMoveAction*>(action);
	if(controlSetAction_){
		disconnect(controlSetAction_, SIGNAL(progress(double,double)), this, SLOT(updateProgressBar(double,double)));
		disconnect(controlSetAction_, SIGNAL(started()), this, SLOT(onStarted()));
		disconnect(controlSetAction_, SIGNAL(succeeded()), this, SLOT(onSucceeded()));
		disconnect(controlSetAction_, SIGNAL(failed(int)), this, SLOT(onFailed(int)));
		disconnect(controlSetAction_, SIGNAL(previousChanged()), this, SLOT(onPreviousNextChanged()));
		disconnect(controlSetAction_, SIGNAL(nextChanged()), this, SLOT(onPreviousNextChanged()));
	}
	controlSetAction_ = controlSetAction;
	if(controlSetAction_){
		connect(controlSetAction_, SIGNAL(progress(double,double)), this, SLOT(updateProgressBar(double,double)));
		connect(controlSetAction_, SIGNAL(started()), this, SLOT(onStarted()));
		connect(controlSetAction_, SIGNAL(succeeded()), this, SLOT(onSucceeded()));
		connect(controlSetAction_, SIGNAL(failed(int)), this, SLOT(onFailed(int)));
		connect(controlSetAction_, SIGNAL(previousChanged()), this, SLOT(onPreviousNextChanged()));
		connect(controlSetAction_, SIGNAL(nextChanged()), this, SLOT(onPreviousNextChanged()));
	}
	onPreviousNextChanged();
}

void AMBeamlineControlSetMoveActionView::onInfoChanged(){
	QString infoText, tmpStr;
	if(index_ != -1){
		infoText.setNum(index_);
		infoText.append(". ");
	}
	infoText += " Moving ";
	QString adjName = controlSetAction_->controlSet()->name();
	adjName.replace(QRegExp("([A-Z])"), " \\1");
	QChar fCap = adjName[0].toUpper();
	adjName.replace(0, 1, fCap);
	infoText += adjName;
	infoLabel_->setText(infoText);
}

void AMBeamlineControlSetMoveActionView::updateProgressBar(double elapsed, double total){
	if(elapsed == total){
		progressBar_->setMaximum(100);
		progressBar_->setValue(100);
		timeRemainingLabel_->setText("Move Complete");
		return;
	}
	if(controlSetAction_->hasFailed())
		return;
	progressBar_->setMaximum((int)total);
	progressBar_->setValue((int)elapsed);

	double secondsRemaining = total - elapsed;
	QTime tRemaining = QTime(0,0,0,0).addMSecs(int(1000*secondsRemaining));
	QString rStr = (tRemaining.hour() > 0) ? "h:mm:ss" : "m:ss" ;
	timeRemainingLabel_->setText(tRemaining.toString(rStr)+" Remaining");
}

void AMBeamlineControlSetMoveActionView::onStopCancelButtonClicked(){
	emit removeRequested(controlSetAction_);
}

void AMBeamlineControlSetMoveActionView::onPlayPauseButtonClicked(){

}

void AMBeamlineControlSetMoveActionView::onPreviousNextChanged(){
	if(controlSetAction_ && !controlSetAction_->hasFinished() && controlSetAction_->previous() && !controlSetAction_->previous()->isRunning() && !controlSetAction_->previous()->hasFinished())
		moveActionUpButton_->setEnabled(true);
	else
		moveActionUpButton_->setEnabled(false);
	if(controlSetAction_ && !controlSetAction_->hasFinished() && controlSetAction_->next())
		moveActionDownButton_->setEnabled(true);
	else
		moveActionDownButton_->setEnabled(false);
}

void AMBeamlineControlSetMoveActionView::onMoveUpButtonClicked(){
	emit moveUpRequested(controlSetAction_);
}

void AMBeamlineControlSetMoveActionView::onMoveDownButtonClicked(){
	emit moveDownRequested(controlSetAction_);
}

void AMBeamlineControlSetMoveActionView::onStarted(){
	disconnect(moveActionUpButton_, SIGNAL(clicked()), this, SLOT(onMoveUpButtonClicked()));
	disconnect(moveActionDownButton_, SIGNAL(clicked()), this, SLOT(onMoveDownButtonClicked()));
	moveActionUpButton_->setEnabled(false);
	moveActionDownButton_->setEnabled(false);
	stopCancelButton_->setIcon(stopIcon_);
	playPauseButton_->setIcon(pauseIcon_);
	playPauseButton_->setEnabled(true);
	emit actionStarted(controlSetAction_);
}

void AMBeamlineControlSetMoveActionView::onSucceeded(){
	progressBar_->setValue(progressBar_->maximum());

	progressBar_->setMaximum(100);
	progressBar_->setValue(100);
	timeRemainingLabel_->setText("Move Complete");
	disconnect(stopCancelButton_, SIGNAL(clicked()), this, SLOT(onStopCancelButtonClicked()));
	disconnect(playPauseButton_, SIGNAL(clicked()), this, SLOT(onPlayPauseButtonClicked()));
	hl_->removeWidget(stopCancelButton_);
	stopCancelButton_->hide();
	hl_->removeWidget(playPauseButton_);
	playPauseButton_->hide();
	emit actionSucceeded(controlSetAction_);
}

void AMBeamlineControlSetMoveActionView::onFailed(int explanation){
	Q_UNUSED(explanation)
	stopCancelButton_->setIcon(closeIcon_);
	playPauseButton_->setIcon(startIcon_);
	playPauseButton_->setEnabled(false);
	timeRemainingLabel_->setText("Move Cancelled");
}
