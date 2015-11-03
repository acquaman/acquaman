#include "AMGCS2GetRecordRateCommand.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"
AMGCS2GetRecordRateCommand::AMGCS2GetRecordRateCommand()
{
	recordRate_ = 0;
}

int AMGCS2GetRecordRateCommand::recordRate() const
{
	return recordRate_;
}

bool AMGCS2GetRecordRateCommand::runImplementation()
{
	// Clear record rate
	recordRate_ = 0;

	bool success = PI_qRTR(controller_->id(), &recordRate_);

	if(!success) {

		lastError_ = controllerErrorMessage();
	}

	return success;
}
