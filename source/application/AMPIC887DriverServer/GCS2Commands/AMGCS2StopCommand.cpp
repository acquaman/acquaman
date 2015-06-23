#include "AMGCS2StopCommand.h"

#include "PI_GCS2_DLL.h"

AMGCS2StopCommand::AMGCS2StopCommand()
{
}

bool AMGCS2StopCommand::runImplementation()
{
	return PI_StopAll(controllerId_);
}
