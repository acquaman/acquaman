#include "AMGCS2GetRecordTriggerSourceCommand.h"
#include "../AMGCS2Support.h"
#include "PI_GCS2_DLL.h"
#include "util/AMCArrayHandler.h"
AMGCS2GetRecordTriggerSourceCommand::AMGCS2GetRecordTriggerSourceCommand(const QList<int> recordTableIds)
{
	recordTablesToQuery_ = recordTableIds;
}

QString AMGCS2GetRecordTriggerSourceCommand::outputString() const
{
	if(!wasSuccessful_) {
		return "";
	}

	QString returnString("Record Table\t\tTrigger Source\n");

	for(int iRecordTableId = 0, recordTableCount = recordTablesToQuery_.count();
		iRecordTableId < recordTableCount;
		++iRecordTableId) {

		int currentRecordTable = recordTablesToQuery_.at(iRecordTableId);

		AMGCS2::DataRecordTrigger currentRecordTrigger =
				tableRecordTriggers().value(currentRecordTable);

		returnString.append(QString("%1\t\t\t%2\n")
							.arg(currentRecordTable)
							.arg(AMGCS2Support::dataRecordTriggerToString(currentRecordTrigger)));
	}

	return returnString.trimmed();
}

QHash<int, AMGCS2::DataRecordTrigger> AMGCS2GetRecordTriggerSourceCommand::tableRecordTriggers() const
{
	return tableRecordTriggers_;
}

bool AMGCS2GetRecordTriggerSourceCommand::validateArguments()
{
	if(recordTablesToQuery_.isEmpty()) {
		lastError_ = "Cannot obtain record triggers. No tables provided.";
		return false;
	}

	for(int iRecordTable = 0, recordTableCount = recordTablesToQuery_.count();
		iRecordTable < recordTableCount;
		++iRecordTable) {

		int currentRecordTableId = recordTablesToQuery_.at(iRecordTable);
		if(currentRecordTableId < 1 || currentRecordTableId > 16) {
			lastError_ = QString("Record table id %1 not valid. Must be between 1 and 16").arg(currentRecordTableId);
			return false;
		}
	}

	return true;
}

bool AMGCS2GetRecordTriggerSourceCommand::runImplementation()
{
	// Clear previous results;
	tableRecordTriggers_.clear();

	int recordTableCount = recordTablesToQuery_.count();
	AMCArrayHandler<int> recordTableIds(recordTableCount);
	AMCArrayHandler<int> triggerSourceCodes(recordTableCount);
	AMCArrayHandler<char> dummyValueResults(BUFFER_SIZE);

	for(int iRecordTable = 0;
		iRecordTable < recordTableCount;
		++iRecordTable) {

		recordTableIds.cArray()[iRecordTable] = recordTablesToQuery_.at(iRecordTable);
	}

	bool success = PI_qDRT(controllerId_,
						   recordTableIds.cArray(),
						   triggerSourceCodes.cArray(),
						   dummyValueResults.cArray(),
						   recordTableCount,
						   BUFFER_SIZE);

	if(!success) {

		lastError_ = controllerErrorMessage();
	} else {

		for(int iRecordTable = 0;
			iRecordTable < recordTableCount;
			++iRecordTable) {

			int currentRecordTableId = recordTableIds.cArray()[iRecordTable];
			int currentTriggerCode = triggerSourceCodes.cArray()[iRecordTable];
			AMGCS2::DataRecordTrigger currentTrigger =
					AMGCS2Support::intCodeToDataRecordTrigger(currentTriggerCode);

			tableRecordTriggers_.insert(currentRecordTableId, currentTrigger);
		}
	}

	return success;
}
