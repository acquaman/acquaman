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


#include "AMDeferredFunctionCall.h"


QEvent::Type AMDeferredFunctionCall::eventType_ = (QEvent::Type)0;

 AMDeferredFunctionCall::~AMDeferredFunctionCall(){}
AMDeferredFunctionCall::AMDeferredFunctionCall(QObject *parent) :
	QObject(parent)
{
	scheduled_ = false;
	if(eventType_ == 0)
		eventType_ = (QEvent::Type)QEvent::registerEventType();

	connect(&delayTimer_, SIGNAL(timeout()), this, SLOT(onDelayTimerTimeout()));
}

void AMDeferredFunctionCall::onDelayTimerTimeout()
{
	delayTimer_.stop();
	emit executed();
	execute();
}

bool AMDeferredFunctionCall::event(QEvent *e) {
	if(e->type() == eventType_) {
		e->accept();
		if(scheduled_) {
			scheduled_ = false;
			emit executed();
			execute();
		}
		return true;
	}
	else
		return false;
}

void AMDeferredFunctionCall::schedule() {
	if(!scheduled_) {
		QCoreApplication::postEvent(this, new QEvent(eventType_), Qt::HighEventPriority);
		scheduled_ = true;
	}
}

void AMDeferredFunctionCall::runLater(int delayMs)  {
	if(!delayTimer_.isActive()) {
		delayTimer_.start(delayMs);
	}
}
