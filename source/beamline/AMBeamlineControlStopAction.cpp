#include "AMBeamlineControlStopAction.h"

AMBeamlineControlStopAction::AMBeamlineControlStopAction(AMControl *control, QObject *parent) :
		AMBeamlineActionItem(parent)
{
	control_ = 0; //NULL
	setControl(control);
}

AMBeamlineActionItemView* AMBeamlineControlStopAction::createView(int index){
	Q_UNUSED(index)
	return 0;//NULL
}

AMControl* AMBeamlineControlStopAction::control(){
	return control_;
}

void AMBeamlineControlStopAction::start(){
	if(!control_){
		qDebug() << "Failed: No Stop Control";
		onFailed(0);
		return;
	}
	if(!control_->isConnected()){
		qDebug() << "Failed: Stop Control not connected";
		onFailed(1);
		return;
	}

	if(!control_->isMoving()){
		qDebug() << "Auto-Succeed: Stop Control not moving";
		onStopped();
		return;
	}
	if(control_->canStop()){
		qDebug() << "Trying: Stop Control stop called";
		connect(control_, SIGNAL(movingChanged(bool)), this, SLOT(onMovingChanged(bool)));
		control_->stop();
		return;
	}
	else{
		qDebug() << "Failed: Control can't stop";
		onFailed(2);
		return;
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
