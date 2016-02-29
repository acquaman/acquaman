#include "AMGCS2AsyncGetRecordedDataValuesCommand.h"

#include "PI_GCS2_DLL.h"

#include "../AMPIC887Controller.h"
AMGCS2AsyncGetRecordedDataValuesCommand::AMGCS2AsyncGetRecordedDataValuesCommand(AMPIC887HexapodPosition *positionData) :
	AMGCS2AsyncCommand()
{
	positionData_ = positionData;
	rawPositionData_ = 0;
	lastReadDataIndex_ = -2;
	matchCounter_ = 0;
}


bool AMGCS2AsyncGetRecordedDataValuesCommand::runImplementation()
{
	for(int iPosition = 0; iPosition < 1024; ++iPosition) {

		positionData_[iPosition] = AMPIC887HexapodPosition();
	}
	int numberOfRecordTables = 7;
	AMCArrayHandler<int> tableIdsArray = AMCArrayHandler<int>(numberOfRecordTables);
	AMCArrayHandler<char> header = AMCArrayHandler<char>(BUFFER_SIZE + 1);

	tableIdsArray.cArray()[0] = 2;	// Position of X
	tableIdsArray.cArray()[1] = 4;  // Position of Y
	tableIdsArray.cArray()[2] = 6;  // Position of Z
	tableIdsArray.cArray()[3] = 8;  // Position of U
	tableIdsArray.cArray()[4] = 10; // Position of V
	tableIdsArray.cArray()[5] = 12; // Position of W
	tableIdsArray.cArray()[6] = 13; // Record time

	int offsetOfFirstData = 1;
	bool success = PI_qDRR(controller_->id(),
						   tableIdsArray.cArray(),
						   numberOfRecordTables,
						   offsetOfFirstData,
						   -1,
						   &rawPositionData_,
						   header.cArray(),
						   BUFFER_SIZE + 1);

	if(!success) {
		lastError_ = controllerErrorMessage();
	}


	return success;
}

void AMGCS2AsyncGetRecordedDataValuesCommand::checkRunningState()
{
	int lastReadIndex = PI_GetAsyncBufferIndex(controller_->id());

	if(lastReadIndex == -1) {

		lastError_ = "Failed to retrieve any data";
		runningState_ = Failed;
	// We wait for 10 consecutive calls to lastReadIndex to return the same value,
	// indicating that the controller has finished writing its data to the recorder.
	// 10 was obtained through repeat testing of values.
	} else if(matchCounter_ > 10 && controller_->movementStatuses() == 0) {

		runningState_ = Succeeded;
		parseReadData();
	} else if(lastReadIndex == lastReadDataIndex_) {

		matchCounter_++;
	} else {
		matchCounter_ = 0;
	}
	lastReadDataIndex_ = lastReadIndex;
}

void AMGCS2AsyncGetRecordedDataValuesCommand::parseReadData()
{
	if(runningState_ == Succeeded && rawPositionData_ != 0) {

		for(int parsedIndex = 0, xPos = 0, yPos = 1, zPos = 2, uPos = 3, vPos = 4, wPos = 5, time = 6;
			parsedIndex < lastReadDataIndex_/7;
			++parsedIndex, xPos+=7, yPos+=7, zPos+=7, uPos+=7, vPos+=7, wPos+=7, time+=7) {


			positionData_[parsedIndex] = AMPIC887HexapodPosition(rawPositionData_[xPos],
																 rawPositionData_[yPos],
																 rawPositionData_[zPos],
																 rawPositionData_[uPos],
																 rawPositionData_[vPos],
																 rawPositionData_[wPos],
																 rawPositionData_[time]);
		}
	}
}
