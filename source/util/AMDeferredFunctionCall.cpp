#include "AMDeferredFunctionCall.h"


QEvent::Type AMDeferredFunctionCall::eventType_ = (QEvent::Type)0;

AMDeferredFunctionCall::AMDeferredFunctionCall(QObject *parent) :
	QObject(parent)
{
	scheduled_ = false;
	if(eventType_ == 0)
		eventType_ = (QEvent::Type)QEvent::registerEventType();
}
