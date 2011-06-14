#include "AMDeferredFunctionCall.h"


QEvent::Type AMDeferredFunctionCall::eventType_ = (QEvent::Type)0;

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
