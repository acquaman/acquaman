#include "AMAxisStartedActionInfo.h"

AMAxisStartedActionInfo::~AMAxisStartedActionInfo(){}

AMAxisStartedActionInfo::AMAxisStartedActionInfo(const QString &axisName, AMScanAxis::AxisType axisType, QObject *parent) :
	AMActionInfo3("Start Axis", "Start Axis", ":/system-run.png", parent)
{
	axisName_ = axisName;
	axisType_ = axisType;

	setShortDescription("Start Axis");
	setLongDescription("Start Axis");
}

AMAxisStartedActionInfo::AMAxisStartedActionInfo(const AMAxisStartedActionInfo &other) :
	AMActionInfo3(other)
{
	axisName_ = other.axisName();
	axisType_ = other.axisType();
}
