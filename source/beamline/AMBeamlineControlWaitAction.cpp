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
		qDebug() << "Holla! Wait action has started";
		connect(control_, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged(double)));
		startpoint_ = control_->value();
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
	qDebug() << "Heard control value changed " << newValue << " versus " << waitpoint_;
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

	/*
	switch(targetType_){
	case AMBeamlineControlWaitAction::LessThanTarget :
		if( (!holdTimeTimer_.isActive()) && (newValue < waitpoint_) ){
			qDebug() << "Start the timer, value is good";
			//holdTimeTimer_.singleShot(holdTime_, this, SLOT(onHoldTimeReached()));
			connect(&holdTimeTimer_, SIGNAL(timeout()), this, SLOT(onHoldTimeReached()));
			holdTimeTimer_.start(holdTime_);
		}
		else if( (holdTimeTimer_.isActive()) && (newValue >= waitpoint_) ){
			qDebug() << "Stop the timer, value is no longer good";
			disconnect(&holdTimeTimer_, SIGNAL(timeout()), this, SLOT(onHoldTimeReached()));
			holdTimeTimer_.stop();
		}
		break;
	case AMBeamlineControlWaitAction::GreaterThanTarget :
		if( (!holdTimeTimer_.isActive()) && (newValue > waitpoint_) )
			holdTimeTimer_.singleShot(holdTime_, this, SLOT(onHoldTimeReached()));
		else if( (holdTimeTimer_.isActive()) && (newValue <= waitpoint_) )
			holdTimeTimer_.stop();
		break;
	case AMBeamlineControlWaitAction::EqualToTarget :
		if( (!holdTimeTimer_.isActive()) && (fabs(newValue-waitpoint_) < actionTolerance_) )
			holdTimeTimer_.singleShot(holdTime_, this, SLOT(onHoldTimeReached()));
		else if( (holdTimeTimer_.isActive()) && (fabs(newValue-waitpoint_) >= actionTolerance_) )
			holdTimeTimer_.stop();
		break;
	case AMBeamlineControlWaitAction::NotEqualToTarget :
		if( (!holdTimeTimer_.isActive()) && (fabs(newValue-waitpoint_) > actionTolerance_) )
			holdTimeTimer_.singleShot(holdTime_, this, SLOT(onHoldTimeReached()));
		else if( (holdTimeTimer_.isActive()) && (fabs(newValue-waitpoint_) <= actionTolerance_) )
			holdTimeTimer_.stop();
		break;
	}
	*/
}

void AMBeamlineControlWaitAction::onConnected(bool connected){
	qDebug() << "Heard connected changed to " << connected << " in wait action";
	if(!hasStarted()){
		if(control_->valueOutOfRange(waitpoint_))
			waitpoint_ = 0;
		checkReady();
	}
}

void AMBeamlineControlWaitAction::checkReady(){
	if(!control_ || !control_->isConnected())
		setReady(false);
	qDebug() << "Check ready passed in wait action";
	setReady(true);
}

void AMBeamlineControlWaitAction::onStarted(){
	//setStarted(true);
}

void AMBeamlineControlWaitAction::calculateProgress(){

}

void AMBeamlineControlWaitAction::onHoldTimeReached(){
	qDebug() << "Double HOLLA! Hold Time Reached and wait action succeeded";
	disconnect(&holdTimeTimer_, SIGNAL(timeout()), this, SLOT(onHoldTimeReached()));
	holdTimeTimer_.stop();
	setSucceeded(true);
}
