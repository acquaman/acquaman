#include "AMGCS2SetPivotPointCommand.h"
#include "AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
AMGCS2SetPivotPointCommand::AMGCS2SetPivotPointCommand(const QHash<AMGCS2::Axis, double>& pivotPoint)
{
	pivotPoint_ = pivotPoint;
}

bool AMGCS2SetPivotPointCommand::validateArguments()
{
	if(pivotPoint_.isEmpty()) {
		lastError_ = "No axes:pivot values provided";
		return false;
	}

	if(pivotPoint_.count() > 3) {
		lastError_ = "More than the maximum number of axes:pivot values provided";
		return false;
	}

	foreach(AMGCS2::Axis currentAxis, pivotPoint_.keys()) {
		if(currentAxis == AMGCS2::UnknownAxis ||
				currentAxis == AMGCS2::UAxis ||
				currentAxis == AMGCS2::VAxis ||
				currentAxis == AMGCS2::WAxis) {

			lastError_ = "Invalid axis specified (only X, Y and Z are valid)";
			return false;
		}
	}

	return true;
}

bool AMGCS2SetPivotPointCommand::runImplementation()
{
	AMCArrayHandler<double> pivotPointValuesHandler(AXIS_COUNT);
	QString axesString;

	QList<AMGCS2::Axis> axesToSet = pivotPoint_.keys();

	for(int iAxis = 0, axisCount = axesToSet.count();
		iAxis < axisCount;
		++iAxis) {

		AMGCS2::Axis currentAxis = axesToSet.at(iAxis);
		axesString.append(QString(" %1")
						  .arg(AMGCS2Support::axisToCharacter(currentAxis)));

		pivotPointValuesHandler.cArray()[iAxis]
				= pivotPoint_.value(currentAxis);
	}

	bool success = PI_SPI(controllerId_,
						  axesString.toStdString().c_str(),
						  pivotPointValuesHandler.cArray());

	if(!success) {
		lastError_ = controllerErrorMessage();
	}

	return success;
}
