#include "AMGCS2SetRecordRateCommand.h"

#include "AMGCS2GetRecordRateCommand.h" // For the rate to cycles conversion factor
#include "../AMPIC887Controller.h"
#include "PI_GCS2_DLL.h"
AMGCS2SetRecordRateCommand::AMGCS2SetRecordRateCommand(double recordRate)
{
	cycleRate_ = recordRateToCycles(recordRate);
}

bool AMGCS2SetRecordRateCommand::validateArguments()
{
	return cycleRate_ > 1 &&
			cycleRate_ < 1000000;
}

bool AMGCS2SetRecordRateCommand::runImplementation()
{

	bool success = PI_RTR(controller_->id(), cycleRate_);

	if(success) {
		lastError_ = controllerErrorMessage();
	}

	return success;
}

int AMGCS2SetRecordRateCommand::recordRateToCycles(double recordRate)
{
	return int(1/(SECS_PER_CYCLE * recordRate));
}


