#include "AMGCS2GetRecordedDataValuesCommand.h"
#include "PI_GCS2_DLL.h"
#include "util/AMCArrayHandler.h"
#include "../AMPIC887Controller.h"
AMGCS2GetRecordedDataValuesCommand::AMGCS2GetRecordedDataValuesCommand(int recordTableId,
																	   int numberOfValues,
																	   int offsetPoint)
{
	recordTableId_ = recordTableId;
	numberOfValues_ = numberOfValues;
	offsetPoint_ = offsetPoint;
}

QList<double> AMGCS2GetRecordedDataValuesCommand::dataValues() const
{
	return dataValues_;
}

bool AMGCS2GetRecordedDataValuesCommand::validateArguments()
{
	if(recordTableId_ < 1 || recordTableId_ > 16) {
		lastError_ = QString("%1 is not a valid record table id. Must be between 1 and 16")
					 .arg(recordTableId_);
		return false;
	}

	if(offsetPoint_ < 0) {
		lastError_ = QString("Offset value of %1 not valid. Must be 0 or greater.").arg(offsetPoint_);
		return false;
	}

	if(numberOfValues_ < 1) {
		lastError_ = QString("Number of values %1 not valid. Must be 1 or greater.").arg(numberOfValues_);
		return false;
	}

	return true;
}

bool AMGCS2GetRecordedDataValuesCommand::runImplementation()
{
	//Clear previous results
	dataValues_.clear();

	AMCArrayHandler<double> resultsArray(numberOfValues_);

	bool success = PI_qDRR_SYNC(controller_->id(),
								recordTableId_,
								offsetPoint_,
								numberOfValues_,
								resultsArray.cArray());

	if(!success) {
		lastError_ = controllerErrorMessage();
	} else {

		for (int iResult = 0;
			 iResult < numberOfValues_;
			 ++iResult) {

			dataValues_.append(resultsArray.cArray()[iResult]);
		}
	}

	return success;
}




