/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "AMBeamlineHighVoltageChannelToggleAction.h"

//AMBeamlineHighVoltageChannelToggleAction::AMBeamlineHighVoltageChannelToggleAction(AMHighVoltageChannel *highVoltageChannel, QObject *parent) :
//		AMBeamlineActionItem(parent)
//{
//	highVoltageChannel_ = 0;//NULL
//	if(highVoltageChannel){
//		setHighVoltageChannel(highVoltageChannel);
//	}
//	setpoint_ = AMHighVoltageChannel::isPowerOff;
//}

//AMBeamlineActionItemView* AMBeamlineHighVoltageChannelToggleAction::createView(int index){
//	Q_UNUSED(index)
//	return 0;//NULL
//}

//AMBeamlineActionItem* AMBeamlineHighVoltageChannelToggleAction::createCopy() const{
//	if(highVoltageChannel_){
//		AMBeamlineHighVoltageChannelToggleAction *retVal = new AMBeamlineHighVoltageChannelToggleAction(highVoltageChannel_);
//		retVal->setSetpoint(setpoint_);
//		return retVal;
//	}
//	return 0;
//}

//AMHighVoltageChannel* AMBeamlineHighVoltageChannelToggleAction::highVoltageChannel(){
//	return highVoltageChannel_;
//}

//AMHighVoltageChannel::highVoltageChannelPowerState AMBeamlineHighVoltageChannelToggleAction::setpoint(){
//	return setpoint_;
//}

//void AMBeamlineHighVoltageChannelToggleAction::start(){
////	qdebug() << "Trying to start";
//	if(isReady()){
////		qdebug() << "Is ready, so start";
//		connect(this, SIGNAL(finished()), this, SLOT(onFinsihed()));
//		if(setpoint_ == AMHighVoltageChannel::isPowerOn){
//			connect(highVoltageChannel_, SIGNAL(fullyPowered()), this, SLOT(onSucceeded()));
//		}
//		else if(setpoint_ == AMHighVoltageChannel::isPowerOff){
//			connect(highVoltageChannel_, SIGNAL(turnedOff()), this, SLOT(onSucceeded()));
//		}
//		connect(highVoltageChannel_, SIGNAL(statusChanged(QString)), this, SLOT(onStarted()));
//		connect(&progressTimer_, SIGNAL(timeout()), this, SLOT(calculateProgress()));
//		progressTimer_.start(250);
//		if(setpoint_ == AMHighVoltageChannel::isPowerOn){
//			if(highVoltageChannel_->isFullyPowered()){
////				qdebug() << "Already on, no worries";
//				onStarted();
//				onSucceeded();
//			}
//			else{
////				qdebug() << "Trying to turn on";
//				highVoltageChannel_->setOn(true);
//			}
//		}
//		else if(setpoint_ == AMHighVoltageChannel::isPowerOff){
//			if(highVoltageChannel_->isOff()){
////				qdebug() << "Already off, no worries";
//				onStarted();
//				onSucceeded();
//			}
//			else{
////				qdebug() << "Trying to turn off";
//				highVoltageChannel_->setOff();
//			}
//		}
//	}
//	else
//		connect(this, SIGNAL(ready(bool)), this, SLOT(delayedStart(bool)));
//}

//void AMBeamlineHighVoltageChannelToggleAction::cancel(){

//}

//void AMBeamlineHighVoltageChannelToggleAction::setHighVoltageChannel(AMHighVoltageChannel *highVoltageChannel){
//	if(highVoltageChannel_){
//		disconnect(highVoltageChannel_, SIGNAL(voltageChanged(double)), this, SLOT(onVoltageChanged(double)));
//		disconnect(highVoltageChannel_, SIGNAL(connected(bool)), this, SLOT(onConnected(bool)));
//	}
//	highVoltageChannel_ = highVoltageChannel;
//	if(highVoltageChannel_){
//		connect(highVoltageChannel_, SIGNAL(voltageChanged(double)), this, SLOT(onVoltageChanged(double)));
//		connect(highVoltageChannel_, SIGNAL(connected(bool)), this, SLOT(onConnected(bool)));
//	}
//	checkReady();
//}

//void AMBeamlineHighVoltageChannelToggleAction::setSetpoint(AMHighVoltageChannel::highVoltageChannelPowerState setpoint){
//	if( (setpoint == AMHighVoltageChannel::isPowerOff) || (setpoint == AMHighVoltageChannel::isPowerOn))
//		setpoint_ = setpoint;
//}

//void AMBeamlineHighVoltageChannelToggleAction::delayedStart(bool ready){
//	if(ready){
//		disconnect(this, SIGNAL(ready(bool)), this, SLOT(delayedStart(bool)));
//		start();
//	}
//}

//void AMBeamlineHighVoltageChannelToggleAction::onConnected(bool connected){
//	Q_UNUSED(connected)
//	if(!hasStarted())
//		checkReady();
//}

//void AMBeamlineHighVoltageChannelToggleAction::onVoltageChanged(double voltage){
//	Q_UNUSED(voltage)
//	if(!hasStarted())
//		checkReady();
//}

//bool AMBeamlineHighVoltageChannelToggleAction::checkReady(){
//	if(!highVoltageChannel_)
//		return false;
//	else{
//		setReady(highVoltageChannel_->isConnected());
//		return true;
//	}
//}

//void AMBeamlineHighVoltageChannelToggleAction::onStarted(){
//	setStarted(true);
//}

//void AMBeamlineHighVoltageChannelToggleAction::onSucceeded(){
//	disconnect(highVoltageChannel_, 0, this, 0);
//	setSucceeded(true);
//}

//void AMBeamlineHighVoltageChannelToggleAction::onFailed(int explanation){
//	setFailed(true, explanation);
//}

//void AMBeamlineHighVoltageChannelToggleAction::onFinsihed(){
//	progressTimer_.stop();
//	emit progress(1,1);
//}

//void AMBeamlineHighVoltageChannelToggleAction::calculateProgress(){
//	if(highVoltageChannel_ && setpoint_ == AMHighVoltageChannel::isPowerOn)
//		emit progress(highVoltageChannel_->voltage(), highVoltageChannel_->demand());
//	else if(highVoltageChannel_ && setpoint_ == AMHighVoltageChannel::isPowerOff)
//		emit progress(highVoltageChannel_->demand()-highVoltageChannel_->voltage(), highVoltageChannel_->demand());
//}
