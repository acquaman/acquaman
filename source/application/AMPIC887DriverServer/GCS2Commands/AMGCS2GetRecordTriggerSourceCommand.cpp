#include "AMGCS2GetRecordTriggerSourceCommand.h"
#include "../AMGCS2Support.h"
#include "PI_GCS2_DLL.h"
#include "util/AMCArrayHandler.h"
#include "../AMPIC887Controller.h"
AMGCS2GetRecordTriggerSourceCommand::AMGCS2GetRecordTriggerSourceCommand()
{
	recordTrigger_ = AMGCS2::UnknownRecordTrigger;
}

AMGCS2::DataRecordTrigger AMGCS2GetRecordTriggerSourceCommand::recordTrigger() const
{
	return recordTrigger_;
}

bool AMGCS2GetRecordTriggerSourceCommand::runImplementation()
{
	// Clear previous result;
	recordTrigger_ = AMGCS2::UnknownRecordTrigger;

	// The GCS2 Syntax suggests that each table has its own trigger, however triggers
	// for all tables are always the same. This command is structured to only return
	// a single trigger, to remove this confusion.
	int triggersToObtain = 1;

	AMCArrayHandler<int> dataTableIdsHandler(triggersToObtain);
	AMCArrayHandler<int> triggerCodesHandler(triggersToObtain);
	AMCArrayHandler<char> dummyValuesHandler(BUFFER_SIZE);

	// Set the only table id to query to be the id of table 1.
	dataTableIdsHandler.cArray()[0] = 1;

	bool success = PI_qDRT(controller_->id(),
						   dataTableIdsHandler.cArray(),
						   triggerCodesHandler.cArray(),
						   dummyValuesHandler.cArray(),
						   triggersToObtain,
						   BUFFER_SIZE);

	if(!success) {

		lastError_ = controllerErrorMessage();
	} else {

		recordTrigger_ =
				AMGCS2Support::intCodeToDataRecordTrigger(triggerCodesHandler.cArray()[0]);
	}

	return success;
}


