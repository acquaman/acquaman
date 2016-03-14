#include "AMGCS2SetRecordTriggerSourceCommand.h"
#include "util/AMCArrayHandler.h"
#include "../AMGCS2Support.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"
AMGCS2SetRecordTriggerSourceCommand::AMGCS2SetRecordTriggerSourceCommand(
		AMGCS2::DataRecordTrigger tableTrigger)
{

	tableTrigger_ = tableTrigger;
}

bool AMGCS2SetRecordTriggerSourceCommand::validateArguments()
{
	if(tableTrigger_ == AMGCS2::UnknownRecordTrigger) {
		lastError_ = "Provided record trigger is not valid";
		return false;
	}

	return true;
}

bool AMGCS2SetRecordTriggerSourceCommand::runImplementation()
{
	// Although the library and the manual suggest the ability to set the record
	// trigger per table, actually setting it for one sets it for all. As such
	// this command only takes a record trigger, sets it for the first table - which
	// sets it globally.

	AMCArrayHandler<int> recordTableIds(1);
	AMCArrayHandler<int> triggerSources(1);
	// Library requires dummy values to be included in the call...for some reason.
	QString dummyValue = "0";

	recordTableIds.cArray()[0] = 1;
	triggerSources.cArray()[0] = AMGCS2Support::dataRecordTriggerToInt(tableTrigger_);

	bool success = PI_DRT(controller_->id(),
						  recordTableIds.cArray(),
						  triggerSources.cArray(),
						  dummyValue.toStdString().c_str(),
						  1);

	if(!success) {
		lastError_ = controllerErrorMessage();
	}

	return success;
}
