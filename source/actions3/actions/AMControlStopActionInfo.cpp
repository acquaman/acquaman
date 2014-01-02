#include "AMControlStopActionInfo.h"

 AMControlStopActionInfo::~AMControlStopActionInfo(){}
AMControlStopActionInfo::AMControlStopActionInfo(const AMControlInfo &setpoint, QObject *parent)
	: AMActionInfo3("Control Stop", "Control Stop", ":/stop.png", parent)
{
	controlInfo_.setValuesFrom(setpoint);
}

AMControlStopActionInfo::AMControlStopActionInfo(const AMControlStopActionInfo &other)
	: AMActionInfo3(other)
{
	controlInfo_.setValuesFrom(*(other.controlInfo()));
}

void AMControlStopActionInfo::setControlInfo(const AMControlInfo &controlInfo)
{
	controlInfo_.setValuesFrom(controlInfo);
	setModified(true);
}
