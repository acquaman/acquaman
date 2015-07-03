#include "AMGCS2VirtualMoveCommand.h"
#include "util/AMCArrayHandler.h"
#include "../AMGCS2Support.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"
AMGCS2VirtualMoveCommand::AMGCS2VirtualMoveCommand(const QHash<AMGCS2::Axis, double> axisPositions)
{
	axisPositions_ = axisPositions;
	isMoveSafe_ = false;
}

QString AMGCS2VirtualMoveCommand::outputString() const
{
	if(runningState_ != Succeeded) {
		return "";
	}

	if(isMoveSafe_) {
		return "Position can be approached";
	} else {
		return "Not safe to approach position";
	}
}

bool AMGCS2VirtualMoveCommand::isMoveSafe() const
{
	return isMoveSafe_;
}

bool AMGCS2VirtualMoveCommand::validateArguments()
{
	if(axisPositions_.isEmpty()) {
		lastError_ = "No axis positions provided";
		return false;
	}

	foreach (AMGCS2::Axis currentAxis, axisPositions_.keys()) {

		if(currentAxis == AMGCS2::UnknownAxis) {
			lastError_ = "Unknown axis";
			return false;
		}
	}

	return true;
}

bool AMGCS2VirtualMoveCommand::runImplementation()
{
	isMoveSafe_ = false;
	AMCArrayHandler<double> positionValues(axisPositions_.count());
	QString axesString;
	QList<AMGCS2::Axis> axes = axisPositions_.keys();

	for(int iAxis = 0, axisCount = axes.count();
		iAxis < axisCount;
		++iAxis) {

		AMGCS2::Axis currentAxis = axes.at(iAxis);
		axesString.append(QString(" %1")
						  .arg(AMGCS2Support::axisToCharacter(currentAxis)));

		positionValues.cArray()[iAxis] = axisPositions_.value(currentAxis);
	}

	int moveSafeValue = 0;

	bool success = PI_qVMO(controller_->id(),
						   axesString.toStdString().c_str(),
						   positionValues.cArray(),
						   &moveSafeValue);

	if(!success) {
		lastError_ = controllerErrorMessage();
		isMoveSafe_ = false;
	} else {
		isMoveSafe_ = (moveSafeValue == 1);
	}

	return success;
}
