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


#include "AMBeamlineControlStopAction.h"

//AMBeamlineControlStopAction::AMBeamlineControlStopAction(AMControl *control, QObject *parent) :
//		AMBeamlineActionItem(parent)
//{
//	control_ = 0; //NULL
//	setControl(control);
//}

//AMBeamlineActionItemView* AMBeamlineControlStopAction::createView(int index){
//	Q_UNUSED(index)
//	return 0;//NULL
//}

//AMControl* AMBeamlineControlStopAction::control(){
//	return control_;
//}

//void AMBeamlineControlStopAction::start(){
//	if(!control_){
////		qdebug() << "Failed: No Stop Control";
//		onFailed(0);
//		return;
//	}
//	if(!control_->isConnected()){
////		qdebug() << "Failed: Stop Control not connected";
//		onFailed(1);
//		return;
//	}

//	if(!control_->isMoving()){
////		qdebug() << "Auto-Succeed: Stop Control not moving";
//		onStopped();
//		return;
//	}
//	if(control_->canStop()){
////		qdebug() << "Trying: Stop Control stop called";
//		connect(control_, SIGNAL(movingChanged(bool)), this, SLOT(onMovingChanged(bool)));
//		control_->stop();
//		return;
//	}
//	else{
////		qdebug() << "Failed: Control can't stop";
//		onFailed(2);
//		return;
//	}
//}

//void AMBeamlineControlStopAction::setControl(AMControl *control){
//	if(control && control->shouldStop())
//		control_ = control;
//	else
//		control_ = 0; //NULL
//}

//void AMBeamlineControlStopAction::onMovingChanged(bool moving){
//	if(!moving)
//		onStopped();
//}

//void AMBeamlineControlStopAction::onStopped(){
//	disconnect(control_, 0, this, 0);
//	setSucceeded(true);
//}

//void AMBeamlineControlStopAction::onFailed(int explanation){
//	setFailed(true, explanation);
//}
