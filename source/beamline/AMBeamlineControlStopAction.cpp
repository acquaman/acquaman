#include "AMBeamlineControlStopAction.h"

AMBeamlineControlStopAction::AMBeamlineControlStopAction(AMControl *control, QObject *parent) :
		AMBeamlineActionItem(parent)
{
	control_ = 0; //NULL
	setControl(control);
}

AMControl* AMBeamlineControlStopAction::control(){
	return control_;
}

void AMBeamlineControlStopAction::start(){
	if(!control_)
		onFailed(0);
	if(!control_->isConnected())
		onFailed(1);

	if(!control_->isMoving())
		onStopped();
	if(control_->canStop()){
		connect(control_, SIGNAL(movingChanged(bool)), this, SLOT(onMovingChanged(bool)));
		control_->stop();
	}
}

void AMBeamlineControlStopAction::setControl(AMControl *control){
	if(control && control->shouldStop())
		control_ = control;
	else
		control_ = 0; //NULL
}

void AMBeamlineControlStopAction::onMovingChanged(bool moving){
	if(!moving)
		onStopped();
}

void AMBeamlineControlStopAction::onStopped(){
	disconnect(control_, 0, this, 0);
	setSucceeded(true);
}

void AMBeamlineControlStopAction::onFailed(int explanation){
	setFailed(true, explanation);
}
