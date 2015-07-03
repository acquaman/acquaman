#include "AMGCS2GetServoModeCommand.h"
#include "../AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"
AMGCS2GetServoModeCommand::AMGCS2GetServoModeCommand(const QList<AMGCS2::Axis>& axes)
{
	axesToQuery_ = axes;
}

QString AMGCS2GetServoModeCommand::outputString() const
{
	if(runningState_ != Succeeded) {
		return "";
	}

	QString outputString("Axis\t\tServo Mode\n");

	foreach(AMGCS2::Axis currentAxis, servoModeStatuses_.keys()) {
		QString statusString;
		if(servoModeStatuses_.value(currentAxis)) {
			statusString = "Active";
		} else {
			statusString = "Inactive";
		}

		outputString.append(QString("%1\t\t%2\n")
							.arg(AMGCS2Support::axisToCharacter(currentAxis))
							.arg(statusString));
	}

	return outputString.trimmed();
}

QHash<AMGCS2::Axis, bool> AMGCS2GetServoModeCommand::servoModeStatuses() const
{
	return servoModeStatuses_;
}

bool AMGCS2GetServoModeCommand::validateArguments()
{
	foreach(AMGCS2::Axis currentAxis, axesToQuery_) {

		if(currentAxis == AMGCS2::UnknownAxis) {
			lastError_ = "Unknown axis";
			return false;
		}
	}

	if(axesToQuery_.count() > AXIS_COUNT) {
		lastError_ = "Too many axes";
		return false;
	}

	return true;
}

bool AMGCS2GetServoModeCommand::runImplementation()
{
	// Clear previous values
	servoModeStatuses_.clear();

	AMCArrayHandler<int> activeStatusHandler(AXIS_COUNT);
	bool success = false;

	if(axesToQuery_.count() == 0) {
		success = PI_qSVO(controller_->id(), 0, activeStatusHandler.cArray());

		if(success) {
			servoModeStatuses_.insert(AMGCS2::XAxis, activeStatusHandler.cArray()[0] != 0);
			servoModeStatuses_.insert(AMGCS2::YAxis, activeStatusHandler.cArray()[1] != 0);
			servoModeStatuses_.insert(AMGCS2::ZAxis, activeStatusHandler.cArray()[2] != 0);
			servoModeStatuses_.insert(AMGCS2::UAxis, activeStatusHandler.cArray()[3] != 0);
			servoModeStatuses_.insert(AMGCS2::VAxis, activeStatusHandler.cArray()[4] != 0);
			servoModeStatuses_.insert(AMGCS2::WAxis, activeStatusHandler.cArray()[5] != 0);
		}

	} else {

		QString axesString;

		foreach(AMGCS2::Axis currentAxis, axesToQuery_) {

			axesString.append(QString(" %1")
							  .arg(AMGCS2Support::axisToCharacter(currentAxis)));
		}

		success = PI_SVO(controller_->id(),
						 axesString.toStdString().c_str(),
						 activeStatusHandler.cArray());

		if(success) {

			for(int iAxis = 0, axisCount = axesToQuery_.count();
				iAxis < axisCount;
				++iAxis) {

				servoModeStatuses_.insert(
							axesToQuery_.at(iAxis),
							activeStatusHandler.cArray()[iAxis] != 0);
			}
		}
	}

	if(!success) {
		lastError_ = controllerErrorMessage();
	}

	return success;
}
