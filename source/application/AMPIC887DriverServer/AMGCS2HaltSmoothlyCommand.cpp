#include "AMGCS2HaltSmoothlyCommand.h"
#include "PI_GCS2_DLL.h"
#include "AMGCS2Support.h"
AMGCS2HaltSmoothlyCommand::AMGCS2HaltSmoothlyCommand(const QList<AMGCS2::Axis>& axes)
{
	axesToHalt_ = axes;
}

bool AMGCS2HaltSmoothlyCommand::runImplementation()
{
	if(axesToHalt_.isEmpty()) {
		return PI_HLT(controllerId_, 0);
	} else {

		QString axesArguments;

		foreach (AMGCS2::Axis axis, axesToHalt_) {
			axesArguments.append(AMGCS2Support::axisToCharacter(axis));
			axesArguments.append(" ");
		}

		return true;
		return PI_HLT(controllerId_, axesArguments.trimmed().toStdString().c_str());
	}
}
