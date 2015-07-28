#include "AMGCS2GetNumberOfRecordedPointsCommand.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"
AMGCS2GetNumberOfRecordedPointsCommand::AMGCS2GetNumberOfRecordedPointsCommand(const QList<int>& recordTables)
{
	recordTablesToQuery_ = recordTables;
}

QHash<int, int> AMGCS2GetNumberOfRecordedPointsCommand::numberOfDataPointsRecorded() const
{
	return numberOfDataPointsRecorded_;
}

bool AMGCS2GetNumberOfRecordedPointsCommand::validateArguments()
{
	if(recordTablesToQuery_.isEmpty()) {
		lastError_ = "No record table ids provided";
		return false;
	}

	foreach(int recordTableId, recordTablesToQuery_) {
		if(recordTableId < 1 || recordTableId > 16) {
			lastError_ = QString("%1 is not a valid record table id. Must be between 1 and 16")
						 .arg(recordTableId);
			return false;
		}
	}

	return true;
}

bool AMGCS2GetNumberOfRecordedPointsCommand::runImplementation()
{
	// clear any previous results.
	numberOfDataPointsRecorded().clear();

	int recordTableIdCount = recordTablesToQuery_.count();
	AMCArrayHandler<int> recordTableIdsArray(recordTableIdCount);
	AMCArrayHandler<int> numberOfDataPointsRecordedArray(recordTableIdCount);


	for (int iRecordTableId = 0;
		 iRecordTableId < recordTableIdCount;
		 ++iRecordTableId) {

		recordTableIdsArray.cArray()[iRecordTableId] = recordTablesToQuery_.at(iRecordTableId);
	}

	bool success = PI_qDRL(controller_->id(),
						   recordTableIdsArray.cArray(),
						   numberOfDataPointsRecordedArray.cArray(),
						   recordTablesToQuery_.count());

	if(!success) {
		lastError_ = controllerErrorMessage();
	} else {
		for(int iResult = 0;
			iResult < recordTableIdCount;
			++iResult) {

			int currentNumberOfDataPointsRecorded = numberOfDataPointsRecordedArray.cArray()[iResult];
			int currentTableId = recordTableIdsArray.cArray()[iResult];

			numberOfDataPointsRecorded_.insert(currentTableId, currentNumberOfDataPointsRecorded);
		}

	}

	return success;
}
