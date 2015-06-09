#include "AMGCS2MoveCommand.h"
#include "util/AMCArrayHandler.h"
#include "AMGCS2Support.h"

#include "PI_GCS2_DLL.h"
AMGCS2MoveCommand::AMGCS2MoveCommand(const QHash<AMGCS2::Axis, double> &axisPositions)
{
	axisPositions_ = axisPositions;
}

bool AMGCS2MoveCommand::validateArguments()
{
	if(axisPositions_.isEmpty()) {
		lastError_ = "No axis positions provided";
		return false;
	}

	if(axisPositions_.contains(AMGCS2::UnknownAxis)) {
		lastError_ = "Can not move an unknown axis";
		return false;
	}

	return true;
}

bool AMGCS2MoveCommand::runImplementation()
{
	AMCArrayHandler<char> axisStringHandler(axisPositions_.count());
	AMCArrayHandler<double> positionsArrayHandler(axisPositions_.count());

	QList<AMGCS2::Axis> axes = axisPositions_.keys();

	for(int iAxis = 0, axisCount = axes.count();
		iAxis < axisCount;
		++iAxis) {

		axisStringHandler.cArray()[iAxis] = AMGCS2Support::axisToCharacter(axes.at(iAxis)).toAscii();
		positionsArrayHandler.cArray()[iAxis] = axisPositions_.value(axes.at(iAxis));
	}

	bool success = PI_MOV(controllerId_,
						  axisStringHandler.cArray(),
						  positionsArrayHandler.cArray());

	return success;
}
