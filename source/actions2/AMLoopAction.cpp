#include "AMLoopAction.h"

AMLoopAction::AMLoopAction(AMActionInfo *info, QObject *parent) : AMNestedAction(info, parent) {
	loopCount_ = 3;
	currentLoop_ = 0;
	currentSubAction_ = -1;
	logSubActionsSeparately_ = true;
}


AMLoopAction::AMLoopAction(const AMLoopAction &other) : AMNestedAction(other) {
	loopCount_ = other.loopCount_;
	logSubActionsSeparately_ = other.logSubActionsSeparately_;
	// These are runtime, not configure, properties. This is supposed to be a new action:
	currentLoop_ = 0;
	currentSubAction_ = -1;

	foreach(AMAction* action, other.subActions_)
		subActions_ << action->createCopy();
}

AMLoopAction::~AMLoopAction() {
	qDeleteAll(subActions_);
}


void AMLoopAction::insertSubActionImplementation(AMAction *action, int index)
{
	subActions_.insert(index, action);
}

void AMLoopAction::deleteSubActionImplementation(int index)
{
	delete subActions_.takeAt(index);
}
