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


#include "AMNumberChangeAction.h"

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

 AMNumberChangeAction::~AMNumberChangeAction(){}
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
