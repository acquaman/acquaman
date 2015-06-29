#include "AMGCS2ReferenceMoveCommand.h"

#include "../AMGCS2Support.h"
#include "PI_GCS2_DLL.h"
AMGCS2ReferenceMoveCommand::AMGCS2ReferenceMoveCommand(const QList<AMGCS2::Axis>& axes)
{
	axesToReference_ = axes;
}

QString AMGCS2ReferenceMoveCommand::outputString() const
{
	if(wasSuccessful_) {
		return "Reference move begun";
	} else {
		return "";
	}
}

bool AMGCS2ReferenceMoveCommand::validateArguments()
{
	foreach(AMGCS2::Axis currentAxis, axesToReference_) {
		if(currentAxis == AMGCS2::UnknownAxis) {
			lastError_ = "Can not reference move unknown axis";
			return false;
		}
	}

	if(axesToReference_.count() > AXIS_COUNT) {
		lastError_ = "Duplicate axes provided.";
		return false;
	}

	return true;
}

bool AMGCS2ReferenceMoveCommand::runImplementation()
{
	QString axesString;

	foreach(AMGCS2::Axis currentAxis, axesToReference_) {
		axesString.append(QString(" %1").arg(AMGCS2Support::axisToCharacter(currentAxis)));
	}

	bool success = PI_FRF(controllerId_, axesString.trimmed().toStdString().c_str());

	if(!success) {
		lastError_ = controllerErrorMessage();
	}

	return success;
}



