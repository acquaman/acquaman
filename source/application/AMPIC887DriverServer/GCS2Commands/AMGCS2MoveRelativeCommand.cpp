#include "AMGCS2MoveRelativeCommand.h"
#include "../AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
AMGCS2MoveRelativeCommand::AMGCS2MoveRelativeCommand(const QHash<AMGCS2::Axis, double>& relativeAxisPositions)
{
	relativeAxisPositions_ = relativeAxisPositions;
}

bool AMGCS2MoveRelativeCommand::validateArguments()
{
	if(relativeAxisPositions_.isEmpty()) {
		lastError_ = "Cannot perform relative move. No axes provided to move";
		return false;
	}

	foreach(AMGCS2::Axis currentAxis, relativeAxisPositions_.keys()) {
		if(currentAxis == AMGCS2::UnknownAxis) {
			lastError_ = "Cannot perform relative move. Unknown axis provided";
			return false;
		}
	}

	if(relativeAxisPositions_.count() > AXIS_COUNT) {
		lastError_ = "Duplicate axes provided";
		return false;
	}

	return true;
}

bool AMGCS2MoveRelativeCommand::runImplementation()
{
	QString axesString;
	QList<AMGCS2::Axis> axes = relativeAxisPositions_.keys();
	int axisCount = axes.count();
	AMCArrayHandler<double> positionValues(axisCount);

	for (int iAxis = 0;
		 iAxis < axisCount;
		 ++iAxis) {

		AMGCS2::Axis currentAxis = axes.at(iAxis);
		axesString.append(QString(" %1")
						  .arg(AMGCS2Support::axisToCharacter(currentAxis)));

		positionValues.cArray()[iAxis] = relativeAxisPositions_.value(currentAxis);
	}

	bool success = PI_MVR(controllerId_,
						  axesString.toStdString().c_str(),
						  positionValues.cArray());

	if(!success) {
		lastError_ = controllerErrorMessage();
	}

	return success;
}
