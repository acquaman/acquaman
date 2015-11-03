#include "AMGCS2SetRecordRateCommand.h"
#include "../AMPIC887Controller.h"
#include "PI_GCS2_DLL.h"
AMGCS2SetRecordRateCommand::AMGCS2SetRecordRateCommand(int recordRate)
{
	recordRate_ = recordRate;
}

bool AMGCS2SetRecordRateCommand::validateArguments()
{
	return recordRate_ > 1 &&
			recordRate_ < 1000000;
}

bool AMGCS2SetRecordRateCommand::runImplementation()
{

	bool success = PI_RTR(controller_->id(), recordRate_);

	if(success) {
		lastError_ = controllerErrorMessage();
	}

	return success;
}


