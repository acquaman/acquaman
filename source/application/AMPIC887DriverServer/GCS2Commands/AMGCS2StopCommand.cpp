#include "AMGCS2StopCommand.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
#include <QTime>
AMGCS2StopCommand::AMGCS2StopCommand()
{
	statusString_ = "Controller stop not issued";
}

QString AMGCS2StopCommand::outputString() const
{
	return statusString_;
}

bool AMGCS2StopCommand::runImplementation()
{
	// Reset status string
	statusString_ = "Controller stop not issued";

	bool success = PI_GcsCommandset(controllerId_, "STP");

	/* We have to obtain the controller error message, as calling STP always sets
	  the controller's error code to 10, and you can't issue a command if the controllers
	  error message contains a value. Retrieving the error message clears it.
	  Unfortunately there seems to be a bug in the library/controller where the
	  system hangs if you attept to immediately obtain the error message while the
	  axes are still being stopped. As such we need to pause for a little bit before
	  clearing the error */

	QTime stopTime = QTime::currentTime().addMSecs(30);
	while(QTime::currentTime() < stopTime) {
		// Wait...
	}

	statusString_ = controllerErrorMessage();

	return success;
}
