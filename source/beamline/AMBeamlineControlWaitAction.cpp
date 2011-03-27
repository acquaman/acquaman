#include "AMBeamlineControlWaitAction.h"

AMBeamlineControlWaitAction::AMBeamlineControlWaitAction(AMControl *control, AMBeamlineControlWaitAction::waitTargetType targetType, QObject *parent) :
		AMBeamlineActionItem(parent)
{
	control_ = 0; //NULL
	targetType_ = targetType;
	actionTolerance_ = 0;
	if(control){
		actionTolerance_ = control->tolerance();
		setControl(control);
	}
}

AMBeamlineActionView* AMBeamlineControlWaitAction::createView(int index){
	return 0;
}

AMControl* AMBeamlineControlWaitAction::control(){
	return control_;
}

double AMBeamlineControlWaitAction::waitpoint() const{
	return waitpoint_;
}

double AMBeamlineControlWaitAction::actionTolerance() const{
	return actionTolerance_;
}

int AMBeamlineControlWaitAction::targetType() const{
	return targetType_;
}

double AMBeamlineControlWaitAction::holdTime() const{
	return holdTime_;
}

void AMBeamlineControlWaitAction::start(){
	if(isReady()){
		connect(control_, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged(double)));
		startpoint_ = control_->value();
		outlierpoint_ = startpoint_;
		if( (targetType_ == AMBeamlineControlWaitAction::LessThanTarget) || (targetType_ == AMBeamlineControlWaitAction::GreaterThanTarget) )
			actionTolerance_ = 0;
		connect(&progressTimer_, SIGNAL(timeout()), this, SLOT(calculateProgress()));
		progressTimer_.start(500);
		setStarted(true);
		onValueChanged(control_->value());
	}
	else
		connect(this, SIGNAL(ready(bool)), this, SLOT(delayedStart(bool)));
}

void AMBeamlineControlWaitAction::cancel(){
	disconnect(control_, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged(double)));
	disconnect(&progressTimer_, SIGNAL(timeout()), this, SLOT(calculateProgress()));
	progressTimer_.stop();
	setFailed(true); //NEED TO SEND FAILURE EXPLANATION
}

void AMBeamlineControlWaitAction::setControl(AMControl *control){
	if(control_)
		disconnect(control_, SIGNAL(connected(bool)), this, SLOT(onConnected(bool)));
	control_ = control;
	if(control_)
		connect(control_, SIGNAL(connected(bool)), this, SLOT(onConnected(bool)));
	checkReady();
}

bool AMBeamlineControlWaitAction::setWaitpoint(double waitpoint){
	if(!control_)
		return false;
	if(control_->isConnected() && control_->valueOutOfRange(waitpoint))
		return false;
	waitpoint_ = waitpoint;
	return true;
}

void AMBeamlineControlWaitAction::setActionTolerance(double actionTolerance){
	actionTolerance_ = actionTolerance;
}

void AMBeamlineControlWaitAction::setTargetType(AMBeamlineControlWaitAction::waitTargetType targetType){
	targetType_ = targetType;
}

bool AMBeamlineControlWaitAction::setHoldTime(double holdTime){
	if(holdTime < 0)
		return false;
	holdTime_ = holdTime;
	return true;
}

void AMBeamlineControlWaitAction::cleanup(){

}

void AMBeamlineControlWaitAction::delayedStart(bool ready){
	if(ready){
		disconnect(this, SIGNAL(ready(bool)), this, SLOT(delayedStart(bool)));
		start();
	}
}

void AMBeamlineControlWaitAction::onValueChanged(double newValue){
	bool startTimer = false;
	bool stopTimer = false;
	bool timerRunning = holdTimeTimer_.isActive();

	switch(targetType_){
	case AMBeamlineControlWaitAction::LessThanTarget :
		if( !timerRunning && (newValue < waitpoint_) )
			startTimer = true;
		else if( timerRunning && (newValue >= waitpoint_) )
			stopTimer = true;
		break;
	case AMBeamlineControlWaitAction::GreaterThanTarget :
		if( !timerRunning && (newValue > waitpoint_) )
			startTimer = true;
		else if( timerRunning && (newValue <= waitpoint_) )
			stopTimer = true;
		break;
	case AMBeamlineControlWaitAction::EqualToTarget :
		if( !timerRunning && (fabs(newValue-waitpoint_) < actionTolerance_) )
			startTimer = true;
		else if( timerRunning && (fabs(newValue-waitpoint_) >= actionTolerance_) )
			stopTimer = true;
		break;
	case AMBeamlineControlWaitAction::NotEqualToTarget :
		if( !timerRunning && (fabs(newValue-waitpoint_) > actionTolerance_) )
			startTimer = true;
		else if( timerRunning && (fabs(newValue-waitpoint_) <= actionTolerance_) )
			stopTimer = true;
		break;
	}
	if(startTimer){
		connect(&holdTimeTimer_, SIGNAL(timeout()), this, SLOT(onHoldTimeReached()));
		holdTimeTimer_.start(holdTime_);
	}
	else if(stopTimer){
		disconnect(&holdTimeTimer_, SIGNAL(timeout()), this, SLOT(onHoldTimeReached()));
		holdTimeTimer_.stop();
	}
}

void AMBeamlineControlWaitAction::onConnected(bool connected){
	if(!hasStarted()){
		if(control_->valueOutOfRange(waitpoint_))
			waitpoint_ = 0;
		checkReady();
	}
}

void AMBeamlineControlWaitAction::checkReady(){
	if(!control_ || !control_->isConnected())
		setReady(false);
	setReady(true);
}

void AMBeamlineControlWaitAction::calculateProgress(){
	double elapsed;
	if(targetType_ != AMBeamlineControlWaitAction::NotEqualToTarget){
		if( fabs(control_->value()-waitpoint_) > fabs(outlierpoint_-waitpoint_) )
			outlierpoint_ = control_->value();
		elapsed = 1.0 - (fabs(control_->value()-waitpoint_)-actionTolerance_)/(fabs(outlierpoint_-waitpoint_)-actionTolerance_);
	}
	else
		elapsed = fabs(control_->value()-waitpoint_)/actionTolerance_;
	qDebug() << "Progress is " << elapsed/1.0;
	emit progress(elapsed, 1.0);
}

void AMBeamlineControlWaitAction::onHoldTimeReached(){
	disconnect(&holdTimeTimer_, SIGNAL(timeout()), this, SLOT(onHoldTimeReached()));
	holdTimeTimer_.stop();
	progressTimer_.stop();
	emit progress(1.0, 1.0);
	setSucceeded(true);
}
