#include "AMGCS2GetRecordRateCommand.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"
AMGCS2GetRecordRateCommand::AMGCS2GetRecordRateCommand()
{
	recordRate_ = 0;
}

double AMGCS2GetRecordRateCommand::recordRate() const
{
	return recordRate_;
}

bool AMGCS2GetRecordRateCommand::runImplementation()
{
	// Clear record rate
	recordRate_ = 0;

	int recordRateInCycles = 0;

	bool success = PI_qRTR(controller_->id(), &recordRateInCycles);

	if(!success) {

		lastError_ = controllerErrorMessage();
	} else {

		recordRate_ = cyclesToRecordRate(recordRateInCycles);
	}

	return success;
}

double AMGCS2GetRecordRateCommand::cyclesToRecordRate(int cycles)
{
	return 1/(SECS_PER_CYCLE * double(cycles));
}
