#include "AMGCS2GetServoModeCommand.h"
#include "../AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"
AMGCS2GetServoModeCommand::AMGCS2GetServoModeCommand()
{

}

bool AMGCS2GetServoModeCommand::servoModeState() const
{
	return servoModeState_;
}

bool AMGCS2GetServoModeCommand::runImplementation()
{
	/*
	Syntax for the servo mode suggests it can be set per axis, however for
	our controller setting it for one axis sets it for them all. As such this
	command only queries the servo mode of one axis, and uses that value. It
	also performs a validation check which ensures all these values are the
	same, just in case this driver is attempted to be used on another controller.
	*/

	AMPIC887AxisCollection axesToQuery
			= AMPIC887AxisCollection(AMPIC887AxisCollection::AllAxes);
	int axisCount = axesToQuery.count();

	QString axisString = axesToQuery.toString();
	AMCArrayHandler<int> servoModeStateHandler(axisCount);

	bool success = PI_qSVO(controller_->id(),
						   axisString.toStdString().c_str(),
						   servoModeStateHandler.cArray());

	if(!success) {
		lastError_ = controllerErrorMessage();
	} else {

		bool firstValue = (servoModeStateHandler.cArray()[0] == 1);
		for(int iAxis = 0; iAxis < axisCount; ++iAxis) {

			bool currentValue = (servoModeStateHandler.cArray()[iAxis] == 1);
			if(currentValue != firstValue) {
				lastError_ = "Servo mode for all axes does not match";
				return false;
			}
		}
		servoModeState_ = firstValue;
	}

	return success;
}
