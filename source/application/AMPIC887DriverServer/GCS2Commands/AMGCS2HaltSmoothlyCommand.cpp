#include "AMGCS2HaltSmoothlyCommand.h"
#include "PI_GCS2_DLL.h"
#include "../AMGCS2Support.h"
#include "../AMPIC887ErrorMessageClearer.h"

AMGCS2HaltSmoothlyCommand::AMGCS2HaltSmoothlyCommand(const QList<AMGCS2::Axis>& axes)
{
	axesToHalt_ = axes;
}

bool AMGCS2HaltSmoothlyCommand::runImplementation()
{
	bool success = false;
	if(axesToHalt_.isEmpty()) {
		success = PI_HLT(controllerId_, 0);
	} else {

		QString axesArguments;

		foreach (AMGCS2::Axis axis, axesToHalt_) {
			axesArguments.append(AMGCS2Support::axisToCharacter(axis));
			axesArguments.append(" ");
		}

		success = PI_HLT(controllerId_, axesArguments.trimmed().toStdString().c_str());
	}

	if(success) {

		/* We have to obtain the controller error message, as calling HLT always sets
		  the controller's error code to 10, and you can't issue a command if the controller's
		  error message contains a value. Retrieving the error message clears it.
		  Unfortunately there seems to be a bug in the library/controller where the
		  system hangs if you attept to immediately obtain the error message while the
		  axes are still being stopped. As such we need to schedule the clearing of
		  the error message for some period of time after we're done.*/

		AMPIC887ErrorMessageClearer* errorMessageClearer =
				new AMPIC887ErrorMessageClearer(controllerId_);

		errorMessageClearer->start(30);
	} else {

		lastError_ = controllerErrorMessage();
	}


	return success;
}
