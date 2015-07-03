#include "AMGCS2SetLowSoftLimitsCommand.h"
#include "../AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"

AMGCS2SetLowSoftLimitsCommand::AMGCS2SetLowSoftLimitsCommand(const QHash<AMGCS2::Axis, double>& axisLowLimits)
{
	axisLowLimits_ = axisLowLimits;
}

bool AMGCS2SetLowSoftLimitsCommand::validateArguments()
{
	if(axisLowLimits_.isEmpty()) {
		lastError_ = "Cannot set soft limits - No axis limits provided";
		return false;
	}

	foreach(AMGCS2::Axis currentAxis, axisLowLimits_.keys()) {

		if(currentAxis == AMGCS2::UnknownAxis) {
			lastError_ = "Cannot set soft limits - Unknown axis provided";
			return false;
		}
	}

	if(axisLowLimits_.count() > AXIS_COUNT) {
		lastError_ = "Duplicate axes provided";
		return false;
	}

	return true;
}

bool AMGCS2SetLowSoftLimitsCommand::runImplementation()
{
	QList<AMGCS2::Axis> axes = axisLowLimits_.keys();
	int axesCount = axes.count();
	AMCArrayHandler<double> lowLimitValues(axesCount);
	QString axesString;

	for(int iAxis = 0;
		iAxis < axesCount;
		++iAxis) {

		AMGCS2::Axis currentAxis = axes.at(iAxis);
		axesString.append(QString(" %1")
						  .arg(AMGCS2Support::axisToCharacter(currentAxis)));

		lowLimitValues.cArray()[iAxis] = axisLowLimits_.value(currentAxis);
	}

	bool success = PI_NLM(controller_->id(),
						  axesString.toStdString().c_str(),
						  lowLimitValues.cArray());

	if(!success) {
		lastError_ = controllerErrorMessage();
	}

	return success;
}
