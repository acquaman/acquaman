#include "AMBeamlineHighVoltageChannelToggleAction.h"

#include <QDebug>

AMBeamlineHighVoltageChannelToggleAction::AMBeamlineHighVoltageChannelToggleAction(AMHighVoltageChannel *highVoltageChannel, QObject *parent) :
		AMBeamlineActionItem(parent)
{
	highVoltageChannel_ = 0;//NULL
	if(highVoltageChannel){
		setHighVoltageChannel(highVoltageChannel);
	}
	setpoint_ = AMHighVoltageChannel::isPowerOff;
}

AMBeamlineActionItemView* AMBeamlineHighVoltageChannelToggleAction::createView(int index){
	return 0;//NULL
}

AMBeamlineActionItem* AMBeamlineHighVoltageChannelToggleAction::createCopy() const{
	if(highVoltageChannel_){
		AMBeamlineHighVoltageChannelToggleAction *retVal = new AMBeamlineHighVoltageChannelToggleAction(highVoltageChannel_);
		retVal->setSetpoint(setpoint_);
		return retVal;
	}
	return 0;
}

AMHighVoltageChannel* AMBeamlineHighVoltageChannelToggleAction::highVoltageChannel(){
	return highVoltageChannel_;
}

AMHighVoltageChannel::highVoltageChannelPowerState AMBeamlineHighVoltageChannelToggleAction::setpoint(){
	return setpoint_;
}

void AMBeamlineHighVoltageChannelToggleAction::start(){
	qDebug() << "Trying to start";
	if(isReady()){
		qDebug() << "Is ready, so start";
		connect(this, SIGNAL(finished()), this, SLOT(onFinsihed()));
		if(setpoint_ == AMHighVoltageChannel::isPowerOn){
			connect(highVoltageChannel_, SIGNAL(fullyPowered()), this, SLOT(onSucceeded()));
		}
		else if(setpoint_ == AMHighVoltageChannel::isPowerOff){
			connect(highVoltageChannel_, SIGNAL(turnedOff()), this, SLOT(onSucceeded()));
		}
		connect(highVoltageChannel_, SIGNAL(statusChanged(QString)), this, SLOT(onStarted()));
		connect(&progressTimer_, SIGNAL(timeout()), this, SLOT(calculateProgress()));
		progressTimer_.start(250);
		if(setpoint_ == AMHighVoltageChannel::isPowerOn){
			if(highVoltageChannel_->isOn()){
				qDebug() << "Already on, no worries";
				onStarted();
				onSucceeded();
			}
			else{
				qDebug() << "Trying to turn on";
				highVoltageChannel_->setOn(true);
			}
		}
		else if(setpoint_ == AMHighVoltageChannel::isPowerOff){
			if(highVoltageChannel_->isOff()){
				qDebug() << "Already off, no worries";
				onStarted();
				onSucceeded();
			}
			else{
				qDebug() << "Trying to turn off";
				highVoltageChannel_->setOff();
			}
		}
	}
	else
		connect(this, SIGNAL(ready(bool)), this, SLOT(delayedStart(bool)));
}

void AMBeamlineHighVoltageChannelToggleAction::cancel(){

}

void AMBeamlineHighVoltageChannelToggleAction::setHighVoltageChannel(AMHighVoltageChannel *highVoltageChannel){
	if(highVoltageChannel_){
		disconnect(highVoltageChannel_, SIGNAL(voltageChanged(double)), this, SLOT(onVoltageChanged(double)));
		disconnect(highVoltageChannel_, SIGNAL(connected(bool)), this, SLOT(onConnected(bool)));
	}
	highVoltageChannel_ = highVoltageChannel;
	if(highVoltageChannel_){
		connect(highVoltageChannel_, SIGNAL(voltageChanged(double)), this, SLOT(onVoltageChanged(double)));
		connect(highVoltageChannel_, SIGNAL(connected(bool)), this, SLOT(onConnected(bool)));
	}
	checkReady();
}

void AMBeamlineHighVoltageChannelToggleAction::setSetpoint(AMHighVoltageChannel::highVoltageChannelPowerState setpoint){
	if( (setpoint == AMHighVoltageChannel::isPowerOff) || (setpoint == AMHighVoltageChannel::isPowerOn))
		setpoint_ = setpoint;
}

void AMBeamlineHighVoltageChannelToggleAction::delayedStart(bool ready){
	if(ready){
		disconnect(this, SIGNAL(ready(bool)), this, SLOT(delayedStart(bool)));
		start();
	}
}

void AMBeamlineHighVoltageChannelToggleAction::onConnected(bool connected){
	if(!hasStarted())
		checkReady();
}

void AMBeamlineHighVoltageChannelToggleAction::onVoltageChanged(double voltage){
	if(!hasStarted())
		checkReady();
}

bool AMBeamlineHighVoltageChannelToggleAction::checkReady(){
	if(!highVoltageChannel_)
		return false;
	else
		setReady(highVoltageChannel_->isConnected());
}

void AMBeamlineHighVoltageChannelToggleAction::onStarted(){
	setStarted(true);
}

void AMBeamlineHighVoltageChannelToggleAction::onSucceeded(){
	disconnect(highVoltageChannel_, 0, this, 0);
	setSucceeded(true);
}

void AMBeamlineHighVoltageChannelToggleAction::onFailed(int explanation){
	setFailed(true, explanation);
}

void AMBeamlineHighVoltageChannelToggleAction::onFinsihed(){
	progressTimer_.stop();
	emit progress(1,1);
}

void AMBeamlineHighVoltageChannelToggleAction::calculateProgress(){
	if(highVoltageChannel_ && setpoint_ == AMHighVoltageChannel::isPowerOn)
		emit progress(highVoltageChannel_->voltage(), highVoltageChannel_->demand());
	else if(highVoltageChannel_ && setpoint_ == AMHighVoltageChannel::isPowerOff)
		emit progress(highVoltageChannel_->demand()-highVoltageChannel_->voltage(), highVoltageChannel_->demand());
}
