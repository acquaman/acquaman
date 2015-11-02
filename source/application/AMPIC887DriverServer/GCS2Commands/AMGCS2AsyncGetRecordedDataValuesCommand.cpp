#include "AMGCS2AsyncGetRecordedDataValuesCommand.h"

#include "PI_GCS2_DLL.h"

#include "../AMPIC887Controller.h"
AMGCS2AsyncGetRecordedDataValuesCommand::AMGCS2AsyncGetRecordedDataValuesCommand(QVector<AMPIC887HexapodPosition>* positionData) :
	AMGCS2AsyncCommand()
{
	positionData_ = positionData;
	numberOfValuesToRead_ = 100;
	positionData_->resize(numberOfValuesToRead_);
	rawPositionData_ = 0;
	lastReadDataIndex_ = -2;
	matchCounter_ = 0;
}

#include <QDebug>
bool AMGCS2AsyncGetRecordedDataValuesCommand::runImplementation()
{
	qDebug() << "Retrieving data command";
	positionData_->clear();
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

	qDebug() << QString(header.cArray());

	if(!success) {
		lastError_ = controllerErrorMessage();
		qDebug() << lastError_;
	}


	return success;
}

void AMGCS2AsyncGetRecordedDataValuesCommand::checkRunningState()
{
	int lastReadIndex = PI_GetAsyncBufferIndex(controller_->id());
	numberOfValuesToRead_ = lastReadIndex;
	qDebug() << controller_->movementStatuses();
	if(lastReadIndex == -1) {

		lastError_ = "Failed to retrieve any data";
		runningState_ = Failed;
	} else if(matchCounter_ > 10 && controller_->movementStatuses() == 0) {

		parseReadData();
		runningState_ = Succeeded;
	} else if(lastReadIndex == lastReadDataIndex_) {

		matchCounter_++;
	} else {
		matchCounter_ = 0;
		lastReadDataIndex_ = lastReadIndex;
	}
}

void AMGCS2AsyncGetRecordedDataValuesCommand::parseReadData()
{
	qDebug() << "Attempting to parse data";
	QString iValues = "[";
	for(int i = 0; i < numberOfValuesToRead_; ++i) {
		iValues.append(QString("%2%1,").arg(rawPositionData_[i]).arg(i%7 ? "\t\t" : "\n"));
/*		iValues.append(QString("%1, ").arg(rawPositionData_[i]))*/;
	}

	iValues.append("]\n\n");
	qDebug() << iValues;
//	if(runningState_ == Succeeded && rawPositionData_ != 0) {

//		for(int parsedIndex = 0, xPos = 0, yPos = 1, zPos = 2, uPos = 3, vPos = 4, wPos = 5, time = 6;
//			parsedIndex < numberOfValuesToRead_;
//			++parsedIndex, xPos+=7, yPos+=7, zPos+=7, uPos+=7, vPos+=7, wPos+=7, time+=7) {

//			double* valuesArray = *rawPositionData_;

//			(*positionData_)[parsedIndex] = AMPIC887HexapodPosition(valuesArray[xPos],
//																 valuesArray[yPos],
//																 valuesArray[zPos],
//																 valuesArray[uPos],
//																 valuesArray[vPos],
//																 valuesArray[wPos],
//																 valuesArray[time]);
//		}
//	}
}
