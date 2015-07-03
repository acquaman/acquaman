#include "AMGCS2MoveCommand.h"
#include "util/AMCArrayHandler.h"
#include "../AMGCS2Support.h"
#include "../AMPIC887Controller.h"
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

	if(axisPositions_.count() > AXIS_COUNT) {
		lastError_ = "Duplicate axes provided";
		return false;
	}

	return true;
}

bool AMGCS2MoveCommand::runImplementation()
{

	QString axesString;
	AMCArrayHandler<double> positionsArrayHandler(axisPositions_.count());

	QList<AMGCS2::Axis> axes = axisPositions_.keys();

	for(int iAxis = 0, axisCount = axes.count();
		iAxis < axisCount;
		++iAxis) {

		axesString.append(QString(" %1")
						  .arg(AMGCS2Support::axisToCharacter(axes.at(iAxis)).toAscii()));
		positionsArrayHandler.cArray()[iAxis] = axisPositions_.value(axes.at(iAxis));
	}

	axesString = axesString.trimmed();

	int movePossible = 0;
	bool success = PI_qVMO(controller_->id(), axesString.toStdString().c_str(), positionsArrayHandler.cArray(), &movePossible);

	if(!success) {
		lastError_ = "Could not determine whether specified move was safe";
		return false;
	}

	if(movePossible == 0) {
		lastError_ = "Not safe to run specified move";
		return false;
	}

	success = PI_MOV(controller_->id(), axesString.toStdString().c_str(),  positionsArrayHandler.cArray());

	if(!success) {
		lastError_ = controllerErrorMessage();
	}

	return success;
}
