#include "AMGCS2SetStepSizeCommand.h"
#include "../AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
AMGCS2SetStepSizeCommand::AMGCS2SetStepSizeCommand(const QHash<AMGCS2::Axis, double>& axisStepSizes)
{
	axisStepSizes_ = axisStepSizes;
}

bool AMGCS2SetStepSizeCommand::validateArguments()
{
	if(axisStepSizes_.isEmpty()) {
		lastError_ = "No axes/step sizes provided.";
		return false;
	}

	foreach(AMGCS2::Axis currentAxis, axisStepSizes_.keys()) {
		if(currentAxis == AMGCS2::UnknownAxis) {
			lastError_ = "Unknown axis provided";
			return false;
		}
	}

	if(axisStepSizes_.count() > AXIS_COUNT) {
		lastError_ = "Too many axes provided";
		return false;
	}

	return true;
}

bool AMGCS2SetStepSizeCommand::runImplementation()
{
	AMCArrayHandler<double> stepSizeValueHandler(axisStepSizes_.count());
	QString axesString;
	QList<AMGCS2::Axis> axes = axisStepSizes_.keys();

	for(int iAxis = 0, axisCount = axes.count();
		iAxis < axisCount;
		++iAxis) {

		AMGCS2::Axis currentAxis = axes.at(iAxis);
		axesString.append(QString(" %1")
						  .arg(AMGCS2Support::axisToCharacter(currentAxis)));

		stepSizeValueHandler.cArray()[iAxis] = axisStepSizes_.value(currentAxis);
	}

	bool success = PI_SST(controllerId_,
						  axesString.toStdString().c_str(),
						  stepSizeValueHandler.cArray());

	if(!success) {
		lastError_ = controllerErrorMessage();
	}

	return success;
}
