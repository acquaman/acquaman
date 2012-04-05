#include "AMActionLog3.h"

AMActionLog3::AMActionLog3(QObject *parent) :
	AMDbObject(parent)
{
	info_ = 0;
	finalState_ = 0;
}

AMActionLog3::AMActionLog3(const AMAction3 *completedAction, QObject *parent) :
	AMDbObject(parent)
{
	if(completedAction && completedAction->inFinalState()) {
		info_ = completedAction->info()->createCopy();
		finalState_ = completedAction->state();
		startDateTime_ = completedAction->startDateTime();
		endDateTime_ = completedAction->endDateTime();
		setName(info_->shortDescription());
	}
	else {
		info_ = 0;
		finalState_ = 0;
	}
}

AMActionLog3::AMActionLog3(const AMActionLog3 &other) :
	AMDbObject(other)
{
	if(other.isValid()) {
		info_ = other.info()->createCopy();
		finalState_ = other.finalState();
		startDateTime_ = other.startDateTime();
		endDateTime_ = other.endDateTime();
		setName(info_->shortDescription());
	}
	else {
		info_ = 0;
		finalState_ = 0;
	}
}

AMActionLog3::~AMActionLog3() {
	delete info_;
	info_ = 0;
}

bool AMActionLog3::setFromAction(const AMAction3 *completedAction)
{
	if(completedAction && completedAction->inFinalState()) {
		delete info_;
		info_ = completedAction->info()->createCopy();
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

void AMActionLog3::dbLoadStartDateTime(const QDateTime &startDateTime)
{
	startDateTime_ = startDateTime;
	setModified(true);
}

void AMActionLog3::dbLoadEndDateTime(const QDateTime &endDateTime)
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
		delete info_;
		info_ = info;
		setName(info_->shortDescription());
		setModified(true);
	}
	else {
		// not doing anything with this object because it's the wrong type. However, it's our responsibility now, so delete it.
		delete newInfo;
	}
}

bool AMActionLog3::logCompletedAction(const AMAction3 *completedAction, AMDatabase *database)
{
	if(completedAction && completedAction->inFinalState()) {
		AMActionLog3 actionLog(completedAction);
		return actionLog.storeToDb(database);
	}
	else {
		return false;
	}
}
