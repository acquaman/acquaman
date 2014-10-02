/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#include "AMSamplePlatePre2013MoveAction.h"

#include "beamline/AMControlSet.h"
#include "beamline/AMBeamline.h"
#include "util/AMErrorMonitor.h"

#include "actions3/AMListAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"

AMSamplePlatePre2013MoveAction::AMSamplePlatePre2013MoveAction(AMSamplePlatePre2013MoveActionInfo *info, QObject *parent) :
	AMAction3(info, parent)
{
	if(AMBeamline::bl()->isConnected())
		sampleManipulator_ = AMBeamline::bl()->currentSamplePositioner();
	else
		sampleManipulator_ = 0; //NULL

	if( (samplePlateMoveInfo()->samplePlateId() == -1) && (AMBeamline::bl()->currentSamplePlateId() != -1) )
		samplePlateMoveInfo()->setSamplePlateId(AMBeamline::bl()->currentSamplePlateId());

	moveListAction_ = 0; //NULL
}

AMSamplePlatePre2013MoveAction::AMSamplePlatePre2013MoveAction(const AMSamplePlatePre2013MoveAction &other) :
	AMAction3(other)
{
	if(AMBeamline::bl()->isConnected())
		sampleManipulator_ = AMBeamline::bl()->currentSamplePositioner();
	else
		sampleManipulator_ = 0; //NULL

	moveListAction_ = 0; //NULL
}

AMSamplePlatePre2013MoveAction::~AMSamplePlatePre2013MoveAction(){}

AMAction3* AMSamplePlatePre2013MoveAction::createCopy() const{
	return new AMSamplePlatePre2013MoveAction(*this);
}

void AMSamplePlatePre2013MoveAction::onMoveListStarted(){
	setStarted();
}

void AMSamplePlatePre2013MoveAction::onMoveListFailed(){
	disconnect(moveListAction_, SIGNAL(started()), this, SLOT(onMoveListStarted()));
	disconnect(moveListAction_, SIGNAL(failed()), this, SLOT(onMoveListFailed()));
	disconnect(moveListAction_, SIGNAL(succeeded()), this, SLOT(onMoveListSucceeded()));
	disconnect(moveListAction_, SIGNAL(cancelled()), this, SLOT(onMoveListCancelled()));
	disconnect(moveListAction_, SIGNAL(progressChanged(double,double)), this, SIGNAL(progressChanged(double,double)));

	AMErrorMon::alert(this, AMSAMPLEPLATEPRE2013MOVEACTION_MOVELIST_FAILED, QString("There was an error moving one or more motors after start was called."));
	setFailed();
}

void AMSamplePlatePre2013MoveAction::onMoveListSucceeded(){
	disconnect(moveListAction_, SIGNAL(started()), this, SLOT(onMoveListStarted()));
	disconnect(moveListAction_, SIGNAL(failed()), this, SLOT(onMoveListFailed()));
	disconnect(moveListAction_, SIGNAL(succeeded()), this, SLOT(onMoveListSucceeded()));
	disconnect(moveListAction_, SIGNAL(cancelled()), this, SLOT(onMoveListCancelled()));
	disconnect(moveListAction_, SIGNAL(progressChanged(double,double)), this, SIGNAL(progressChanged(double,double)));

	setProgress(100, 100);
	setSucceeded();
}

void AMSamplePlatePre2013MoveAction::onMoveListCancelled(){
	disconnect(moveListAction_, SIGNAL(started()), this, SLOT(onMoveListStarted()));
	disconnect(moveListAction_, SIGNAL(failed()), this, SLOT(onMoveListFailed()));
	disconnect(moveListAction_, SIGNAL(succeeded()), this, SLOT(onMoveListSucceeded()));
	disconnect(moveListAction_, SIGNAL(cancelled()), this, SLOT(onMoveListCancelled()));
	disconnect(moveListAction_, SIGNAL(progressChanged(double,double)), this, SIGNAL(progressChanged(double,double)));

	setCancelled();
}

void AMSamplePlatePre2013MoveAction::startImplementation(){
	if(!samplePlateMoveInfo()->samplePosition()){
		AMErrorMon::alert(this, AMSAMPLEPLATEPRE2013MOVEACTION_BAD_POSITION_SETPOINT, QString("There was an error moving the sample positioner, because the sample position is not valid."));
		setFailed();
		return;
	}

	const AMControlInfoList& setpoint = *(samplePlateMoveInfo()->samplePosition());

	// If you still don't have a positioner check the beamline one last time
	if(!sampleManipulator_)
		sampleManipulator_ = AMBeamline::bl()->currentSamplePositioner();

	// Must have a sample positioner and the motors must be able to move
	if(!sampleManipulator_){
		AMErrorMon::alert(this, AMSAMPLEPLATEPRE2013MOVEACTION_NO_SAMPLE_POSITIONER, QString("There was an error moving the sample positioner, because the sample positioner was not found."));
		setFailed();
		return;
	}
	// Check if we can move all positioner motors
	for(int x = 0; x < sampleManipulator_->count(); x++){
		if(!sampleManipulator_->at(x)->canMove()){
			AMErrorMon::alert(this, AMSAMPLEPLATEPRE2013MOVEACTION_POSITIONER_MOTOR_CANNOT_MOVE, QString("There was an error moving the sample positioner, because the motor named %1 could not move.").arg(sampleManipulator_->at(x)->description()));
			setFailed();
			return;
		}
	}
	// Check that each movement requested is in range
	for(int x = 0; x < sampleManipulator_->count(); x++){
		if(sampleManipulator_->at(x)->valueOutOfRange(setpoint.at(x).value())){
			AMErrorMon::alert(this, AMSAMPLEPLATEPRE2013MOVEACTION_POSITIONER_MOVE_OUT_OF_RANGE, QString("There was an error moving the sample positioner, because moving to %1 is out of range for the motor named %2.").arg(setpoint.at(x).value()).arg(sampleManipulator_->at(x)->description()));
			setFailed();
			return;
		}
	}

	// make the parallel list and put controlMoveActions in it
	moveListAction_ = new AMParallelListAction3(new AMParallelListActionInfo3(), this);
	moveListAction_->setShouldLogSubActionsSeparately(false);
	AMControlMoveActionInfo3 *tempMoveActionInfo;
	AMControlMoveAction3 *tempMoveAction;
	for(int x = 0; x < sampleManipulator_->count(); x++){
		tempMoveActionInfo = new AMControlMoveActionInfo3(setpoint.at(x));
		tempMoveAction = new AMControlMoveAction3(tempMoveActionInfo, sampleManipulator_->at(x));
		if(tempMoveAction)
			moveListAction_->addSubAction(tempMoveAction);
	}

	// doublecheck to make sure we got them all
	if(moveListAction_->numberOfChildren() != setpoint.count()){
		AMErrorMon::alert(this, AMSAMPLEPLATEPRE2013MOVEACTION_CONTROL_MOVE_CREATION_FAILED, "There was an error moving the sample positioner, one or more motors failed to create a move action.");
		setFailed();
		return;
	}

	// connect to the list's signals
	connect(moveListAction_, SIGNAL(started()), this, SLOT(onMoveListStarted()));
	connect(moveListAction_, SIGNAL(failed()), this, SLOT(onMoveListFailed()));
	connect(moveListAction_, SIGNAL(succeeded()), this, SLOT(onMoveListSucceeded()));
	connect(moveListAction_, SIGNAL(cancelled()), this, SLOT(onMoveListCancelled()));
	connect(moveListAction_, SIGNAL(progressChanged(double,double)), this, SIGNAL(progressChanged(double,double)));

	// remember the start positions
	startPosition_ = sampleManipulator_->toInfoList();

	moveListAction_->start();
}

void AMSamplePlatePre2013MoveAction::cancelImplementation(){
	if(!sampleManipulator_){
		setCancelled();
		return;
	}

	moveListAction_->cancel();
}
