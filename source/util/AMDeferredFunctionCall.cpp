#include "AMDeferredFunctionCall.h"

AMDeferredFunctionCall::AMDeferredFunctionCall(QObject *parent) :
	QObject(parent)
{
	scheduled_ = false;
}
