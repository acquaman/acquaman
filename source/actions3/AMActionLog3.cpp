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


#include "AMActionLog3.h"

#include "actions3/AMLoopAction3.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "util/AMErrorMonitor.h"

AMActionLog3::AMActionLog3(QObject *parent) :
	AMDbObject(parent)
{
	info_ = 0;
	finalState_ = 0;
	loadedInfoFromDb_ = false;
}

AMActionLog3::AMActionLog3(const AMAction3 *completedAction, QObject *parent) :
	AMDbObject(parent)
{
	if(completedAction){
		const AMListAction3 *listAction = qobject_cast<const AMListAction3*>(completedAction);
		const AMLoopAction3 *loopAction = qobject_cast<const AMLoopAction3*>(completedAction);
		if(listAction)
			actionInheritedList_ = true;
		else
			actionInheritedList_ = false;
		if(loopAction)
			actionInheritedLoop_ = true;
		else
			actionInheritedLoop_ = false;

		info_ = const_cast<AMActionInfo3*>(completedAction->info());
		connect(info_, SIGNAL(destroyed()), this, SLOT(onInfoDestroyed()));

		loadedInfoFromDb_ = false;
		startDateTime_ = completedAction->startDateTime();
		finalState_ = completedAction->state();
		setName(info_->shortDescription());
		if(completedAction->inFinalState())
			endDateTime_ = completedAction->endDateTime();
		else
			endDateTime_ = QDateTime::currentDateTime();
		failureMessage_ = completedAction->failureMessage();
	}
	else{
		info_ = 0;
		finalState_ = 0;
		loadedInfoFromDb_ = false;
	}
}

AMActionLog3::AMActionLog3(const AMActionLog3 &other) :
	AMDbObject(other)
{
	if(other.isValid()) {
		actionInheritedList_ = other.actionInheritedList();
		actionInheritedLoop_ = other.actionInheritedLoop();

		info_ = const_cast<AMActionInfo3*>(other.info());
		connect(info_, SIGNAL(destroyed()), this, SLOT(onInfoDestroyed()));

		loadedInfoFromDb_ = false;
		finalState_ = other.finalState();
		startDateTime_ = other.startDateTime();
		endDateTime_ = other.endDateTime();
		setName(info_->shortDescription());
		failureMessage_ = other.failureMessage();
	}
	else {
		info_ = 0;
		finalState_ = 0;
		loadedInfoFromDb_ = false;
	}
}

AMActionLog3::~AMActionLog3() {
	disconnect(info_, SIGNAL(destroyed()), this, SLOT(onInfoDestroyed()));
	if(loadedInfoFromDb_)
		delete info_;
	info_ = 0;
}

QString AMActionLog3::failureMessage() const{
	return failureMessage_;
}

bool AMActionLog3::setFromAction(const AMAction3 *completedAction)
{
	if(completedAction && completedAction->inFinalState()) {
		if(info_)
			disconnect(info_, SIGNAL(destroyed()), this, SLOT(onInfoDestroyed()));

		info_ = const_cast<AMActionInfo3*>(completedAction->info());
		connect(info_, SIGNAL(destroyed()), this, SLOT(onInfoDestroyed()));

		loadedInfoFromDb_ = false;
		finalState_ = completedAction->state();
		startDateTime_ = completedAction->startDateTime();
		endDateTime_ = completedAction->endDateTime();
		setName(info_->shortDescription());
		setModified(true);
		return true;
	}
	else {
		return false;
	}
}

void AMActionLog3::setParentId(int parentId){
	parentId_ = parentId;
}

void AMActionLog3::dbLoadStartDateTime(const AMHighPrecisionDateTime &startDateTime)
{
	startDateTime_ = startDateTime;
	setModified(true);
}

void AMActionLog3::dbLoadEndDateTime(const AMHighPrecisionDateTime &endDateTime)
{
	endDateTime_ = endDateTime;
	setModified(true);
}

void AMActionLog3::dbLoadFinalState(int finalState)
{
	finalState_ = finalState;
	setModified(true);
}

void AMActionLog3::dbLoadInfo(AMDbObject *newInfo)
{
	AMActionInfo3* info = qobject_cast<AMActionInfo3*>(newInfo);
	if(info) {
		if(info_)
			disconnect(info_, SIGNAL(destroyed()), this, SLOT(onInfoDestroyed()));

		info_ = info;
		connect(info_, SIGNAL(destroyed()), this, SLOT(onInfoDestroyed()));

		loadedInfoFromDb_ = true;
		setName(info_->shortDescription());
		setModified(true);
	}
	else {
		// not doing anything with this object because it's the wrong type. However, it's our responsibility now, so delete it.
		delete newInfo;
	}
}

void AMActionLog3::dbLoadActionInheritedLoop(bool actionInheritedLoop){
	actionInheritedLoop_ = actionInheritedLoop;
	setModified(true);
}

void AMActionLog3::onInfoDestroyed(){
	info_ = 0; //NULL
}
