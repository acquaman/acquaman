#include "AMGCS2SetHighSoftLimitsCommand.h"
#include "../AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"

AMGCS2SetHighSoftLimitsCommand::AMGCS2SetHighSoftLimitsCommand(const QHash<AMGCS2::Axis, double>& axisHighLimits)
{
	axisHighLimits_ = axisHighLimits;
}

bool AMGCS2SetHighSoftLimitsCommand::validateArguments()
{
	if(axisHighLimits_.isEmpty()) {
		lastError_ = "Cannot set soft limits - No axis limits provided";
		return false;
	}

	foreach(AMGCS2::Axis currentAxis, axisHighLimits_.keys()) {

		if(currentAxis == AMGCS2::UnknownAxis) {
			lastError_ = "Cannot set soft limits - Unknown axis provided";
			return false;
		}
	}

	if(axisHighLimits_.count() > AXIS_COUNT) {
		lastError_ = "Duplicate axes provided";
		return false;
	}

	return true;
}

bool AMGCS2SetHighSoftLimitsCommand::runImplementation()
{
	QList<AMGCS2::Axis> axes = axisHighLimits_.keys();
	int axesCount = axes.count();
	AMCArrayHandler<double> highLimitValues(axesCount);
	QString axesString;

	for(int iAxis = 0;
		iAxis < axesCount;
		++iAxis) {

		AMGCS2::Axis currentAxis = axes.at(iAxis);
		axesString.append(QString(" %1")
						  .arg(AMGCS2Support::axisToCharacter(currentAxis)));

		highLimitValues.cArray()[iAxis] = axisHighLimits_.value(currentAxis);
	}

	bool success = PI_PLM(controller_->id(),
						  axesString.toStdString().c_str(),
						  highLimitValues.cArray());

	if(!success) {
		lastError_ = controllerErrorMessage();
	}

	return success;
}


