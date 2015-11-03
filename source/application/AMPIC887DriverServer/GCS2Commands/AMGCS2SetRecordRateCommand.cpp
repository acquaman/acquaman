#include "AMGCS2SetRecordRateCommand.h"
#include "../AMPIC887Controller.h"
#include "PI_GCS2_DLL.h"
AMGCS2SetRecordRateCommand::AMGCS2SetRecordRateCommand(double recordRate)
{
	recordRate_ = recordRate;
}

bool AMGCS2SetRecordRateCommand::runImplementation()
{

	bool success = PI_RTR(controller_->id(), recordRate_);

	if(success) {
		lastError_ = controllerErrorMessage();
	}

	return success;
}
