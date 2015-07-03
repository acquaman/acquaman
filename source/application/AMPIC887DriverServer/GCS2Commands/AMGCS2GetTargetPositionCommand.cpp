#include "AMGCS2GetTargetPositionCommand.h"
#include "../AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"
AMGCS2GetTargetPositionCommand::AMGCS2GetTargetPositionCommand(const QList<AMGCS2::Axis>& axes)
{
	axesToQuery_ = axes;
}

QString AMGCS2GetTargetPositionCommand::outputString() const
{
	if(runningState_ != Succeeded) {
		return "";
	}

	QString returnString("Axis\t\tTarget Position\n");

	foreach(AMGCS2::Axis currentAxis, axisTargetPositions_.keys()) {

		returnString.append(QString("%1\t\t%2\n")
							.arg(AMGCS2Support::axisToCharacter(currentAxis))
							.arg(axisTargetPositions_.value(currentAxis)));
	}

	return returnString.trimmed();
}

QHash<AMGCS2::Axis, double> AMGCS2GetTargetPositionCommand::axisTargetPositions() const
{
	return axisTargetPositions_;
}

bool AMGCS2GetTargetPositionCommand::validateArguments()
{

	if(axesToQuery_.count() > AXIS_COUNT) {
		lastError_ = "Too many axes specified";
		return false;
	}

	foreach(AMGCS2::Axis currentAxis, axesToQuery_) {

		if(currentAxis == AMGCS2::UnknownAxis) {
			lastError_ = "Can not obtain target position for unknown axis.";
			return false;
		}
	}

	return true;
}

bool AMGCS2GetTargetPositionCommand::runImplementation()
{
	//Clear previous results:
	axisTargetPositions_.clear();

	AMCArrayHandler<double> targetResults(AXIS_COUNT);
	bool success = false;

	if(axesToQuery_.isEmpty()) {
		success = PI_qMOV(controller_->id(), 0, targetResults.cArray());

		if(success) {

			axisTargetPositions_.insert(AMGCS2::XAxis, targetResults.cArray()[0]);
			axisTargetPositions_.insert(AMGCS2::YAxis, targetResults.cArray()[0]);
			axisTargetPositions_.insert(AMGCS2::ZAxis, targetResults.cArray()[0]);
			axisTargetPositions_.insert(AMGCS2::UAxis, targetResults.cArray()[0]);
			axisTargetPositions_.insert(AMGCS2::VAxis, targetResults.cArray()[0]);
			axisTargetPositions_.insert(AMGCS2::WAxis, targetResults.cArray()[0]);
		}

	} else {

		QString axesString;

		for(int iAxis = 0, axisCount = axesToQuery_.count();
			iAxis < axisCount;
			++iAxis) {

			AMGCS2::Axis currentAxis = axesToQuery_.at(iAxis);

			axesString.append(QString(" %1")
							  .arg(AMGCS2Support::axisToCharacter(currentAxis)));
		}

		success = PI_qMOV(controller_->id(),
						  axesString.toStdString().c_str(),
						  targetResults.cArray());

		if(success) {

			for(int iAxis = 0, axisCount = axesToQuery_.count();
				iAxis < axisCount;
				++iAxis) {

				AMGCS2::Axis currentAxis = axesToQuery_.at(iAxis);

				axisTargetPositions_.insert(currentAxis,
											targetResults.cArray()[iAxis]);
			}
		}
	}

	if(!success) {
		lastError_ = controllerErrorMessage();
	}

	return success;

}
