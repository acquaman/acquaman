#include "AMNumberChangeAction.h"

#include <QDebug>
#include <QTimer>

namespace AMNumberChangeActionSupport{
	QList<int> AMNumberChangeActionList_;
	QStringList AMNumberChangeActionListAsString_;
	QStringListModel *AMNumberChangeActionModel_ = 0;

	const QList<int>* AMNumberChangeActionList(){
		return &AMNumberChangeActionList_;
	}

	void appendNumber(int number){
		if(!AMNumberChangeActionModel_)
			AMNumberChangeActionModel_ = new QStringListModel();
		AMNumberChangeActionList_.append(number);
		AMNumberChangeActionListAsString_.append(QString("%1").arg(number));
		AMNumberChangeActionModel_->setStringList(AMNumberChangeActionListAsString_);
	}

	bool replaceNumber(int index, int newNumber){
		if(index < 0 || index >= AMNumberChangeActionList_.count())
			return false;
		AMNumberChangeActionList_.replace(index, newNumber);
		AMNumberChangeActionListAsString_.replace(index, QString("%1").arg(newNumber));
		AMNumberChangeActionModel_->setStringList(AMNumberChangeActionListAsString_);
		return true;
	}
}

AMNumberChangeAction::AMNumberChangeAction(AMNumberChangeActionInfo *info, QObject *parent) :
	AMAction3(info, parent)
{
}

AMNumberChangeAction::AMNumberChangeAction(const AMNumberChangeAction &other) :
	AMAction3(other)
{
}

AMAction3* AMNumberChangeAction::createCopy() const{
	return new AMNumberChangeAction(*this);
}

bool AMNumberChangeAction::canPause() const{
	return false;
}

bool AMNumberChangeAction::hasChildren() const{
	return false;
}

int AMNumberChangeAction::numberOfChildren() const{
	return 0;
}

void AMNumberChangeAction::goFail(){
	setFailed();
}

void AMNumberChangeAction::goSucceed(){
	setSucceeded();
}

void AMNumberChangeAction::startImplementation(){
	setStarted();
	if(!AMNumberChangeActionSupport::replaceNumber(numberChangeInfo()->indexToChange(), numberChangeInfo()->newNumber()))
		QTimer::singleShot(50, this, SLOT(goFail()));
	QTimer::singleShot(50, this, SLOT(goSucceed()));
}

void AMNumberChangeAction::pauseImplementation(){
	return;
}

void AMNumberChangeAction::resumeImplementation(){
	return;
}

void AMNumberChangeAction::cancelImplementation(){
	setCancelled();
}

const AMNumberChangeActionInfo* AMNumberChangeAction::numberChangeInfo() const{
	return qobject_cast<const AMNumberChangeActionInfo*>(info());
}

AMNumberChangeActionInfo* AMNumberChangeAction::numberChangeInfo(){
	return qobject_cast<AMNumberChangeActionInfo*>(info());
}
