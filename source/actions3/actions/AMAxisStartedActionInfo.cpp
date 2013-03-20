#include "AMAxisStartedActionInfo.h"

AMAxisStartedActionInfo::AMAxisStartedActionInfo(const QString &axisName, QObject *parent) :
	AMActionInfo3("Start Axis", "Start Axis", ":/system-run.png", parent)
{
	axisName_ = axisName;

	setShortDescription("Start Axis");
	setLongDescription("Start Axis");
}

AMAxisStartedActionInfo::AMAxisStartedActionInfo(const AMAxisStartedActionInfo &other) :
	AMActionInfo3(other)
{
	axisName_ = other.axisName();
}
