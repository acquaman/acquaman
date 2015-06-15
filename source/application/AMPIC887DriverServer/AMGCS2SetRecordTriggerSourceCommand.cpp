#include "AMGCS2SetRecordTriggerSourceCommand.h"
#include "util/AMCArrayHandler.h"
#include "AMGCS2Support.h"
#include "PI_GCS2_DLL.h"

AMGCS2SetRecordTriggerSourceCommand::AMGCS2SetRecordTriggerSourceCommand(
		const QHash<int, AMGCS2::DataRecordTrigger> tableTriggers)
{

	tableTriggers_ = tableTriggers;
}

bool AMGCS2SetRecordTriggerSourceCommand::validateArguments()
{
	if(tableTriggers_.isEmpty()) {

		lastError_ = "Cannot set table triggers. None provided.";
		return false;
	}

	QList<int> tableTriggerIds = tableTriggers_.keys();
	for(int iTableTrigger = 0, triggerCount = tableTriggerIds.count();
		iTableTrigger < triggerCount;
		++iTableTrigger) {

		int recordTableId = tableTriggerIds.at(iTableTrigger);

		if(recordTableId < 1 || recordTableId > 16) {
			lastError_ = QString("Record table id of %1 is out of valid range (1 to 16)").arg(recordTableId);
		}

		if(tableTriggers_.value(recordTableId) == AMGCS2::UnknownRecordTrigger) {
			lastError_ = "Provided record trigger is not valid";
			return false;
		}
	}

	return true;
}

bool AMGCS2SetRecordTriggerSourceCommand::runImplementation()
{
	int tableTriggersCount = tableTriggers_.count();
	AMCArrayHandler<int> recordTableIds(tableTriggersCount);
	AMCArrayHandler<int> triggerSources(tableTriggersCount);
	// Library requires dummy values to be included in the call...for some reason.
	QString dummyValues;

	QList<int> tableIdKeys = tableTriggers_.keys();

	for(int iRecordTableId = 0;
		iRecordTableId < tableTriggersCount;
		++iRecordTableId) {

		int currentTableId = tableIdKeys.at(iRecordTableId);
		int currentTriggerCode = AMGCS2Support::dataRecordTriggerToInt(tableTriggers_.value(currentTableId));

		recordTableIds.cArray()[iRecordTableId] = currentTableId;
		triggerSources.cArray()[iRecordTableId] = currentTriggerCode;
		dummyValues.append(QString(" DUMMY"));
	}

	bool success = PI_DRT(controllerId_,
						  recordTableIds.cArray(),
						  triggerSources.cArray(),
						  dummyValues.toStdString().c_str(),
						  tableTriggersCount);

	if(!success) {
		lastError_ = controllerErrorMessage();
	}

	return success;
}
