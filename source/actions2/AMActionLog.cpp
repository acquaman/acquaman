#include "AMActionLog.h"

AMActionLog::AMActionLog(QObject *parent) :
    AMDbObject(parent)
{
	info_ = 0;
	finalState_ = 0;
}

AMActionLog::AMActionLog(const AMAction *completedAction, QObject *parent) :
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

AMActionLog::AMActionLog(const AMActionLog &other) :
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

AMActionLog::~AMActionLog() {
	delete info_;
	info_ = 0;
}

bool AMActionLog::setFromAction(const AMAction *completedAction)
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

void AMActionLog::dbLoadStartDateTime(const QDateTime &startDateTime)
{
	startDateTime_ = startDateTime;
	setModified(true);
}

void AMActionLog::dbLoadEndDateTime(const QDateTime &endDateTime)
{
	endDateTime_ = endDateTime;
	setModified(true);
}

void AMActionLog::dbLoadFinalState(int finalState)
{
	finalState_ = finalState;
	setModified(true);
}

void AMActionLog::dbLoadInfo(AMDbObject *newInfo)
{
	AMActionInfo* info = qobject_cast<AMActionInfo*>(newInfo);
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

bool AMActionLog::logCompletedAction(const AMAction *completedAction, AMDatabase *database)
{
	if(completedAction && completedAction->inFinalState()) {
		AMActionLog actionLog(completedAction);
		return actionLog.storeToDb(database);
	}
	else {
		return false;
	}
}
