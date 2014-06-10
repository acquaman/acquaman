#include "AMAxisFinishedActionInfo.h"

 AMAxisFinishedActionInfo::~AMAxisFinishedActionInfo(){}
AMAxisFinishedActionInfo::AMAxisFinishedActionInfo(const QString &axisName, QObject *parent) :
	AMActionInfo3("Finish Axis", "Finish Axis", ":/system-run.png", parent)
{
	axisName_ = axisName;

	setShortDescription("Finish Axis");
	setLongDescription("Finish Axis");
}

AMAxisFinishedActionInfo::AMAxisFinishedActionInfo(const AMAxisFinishedActionInfo &other) :
	AMActionInfo3(other)
{
	axisName_ = other.axisName();
}
