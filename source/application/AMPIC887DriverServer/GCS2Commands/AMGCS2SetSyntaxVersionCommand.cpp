#include "AMGCS2SetSyntaxVersionCommand.h"

#include "PI_GCS2_DLL.h"
AMGCS2SetSyntaxVersionCommand::AMGCS2SetSyntaxVersionCommand()
{
}

QString AMGCS2SetSyntaxVersionCommand::outputString() const
{
	if(wasSuccessful_) {
		return "Syntax version changed to GCS v2.0";
	} else {
		return "Sntax version not altered.";
	}
}

bool AMGCS2SetSyntaxVersionCommand::runImplementation()
{
	// There is no corresponding library funciton specifically for setting the
	// syntax version. As such we must make use of the PI_GcsCommandSet() function
	// to manually send the command as an ASCII string.
	QString asciiCommandString = "CSV 2";

	bool success = PI_GcsCommandset(controllerId_, asciiCommandString.toStdString().c_str());

	if(!success) {
		lastError_ = controllerErrorMessage();
	}

	return success;
}
