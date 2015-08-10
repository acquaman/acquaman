#include "AMGCS2SetDataRecorderConfigurationCommand.h"
#include "util/AMCArrayHandler.h"
#include "../AMGCS2Support.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"
AMGCS2SetDataRecorderConfigurationCommand::AMGCS2SetDataRecorderConfigurationCommand(
		const QHash<int, AMPIC887DataRecorderConfiguration>& tableRecordConfigs)
{
	tableRecordConfigs_ = tableRecordConfigs;
}

bool AMGCS2SetDataRecorderConfigurationCommand::validateArguments()
{
	if (tableRecordConfigs_.isEmpty()) {
		lastError_ = "No configurations provided";
		return false;
	}

	QList<int> tableIds = tableRecordConfigs_.keys();
	foreach(int currentTableId, tableIds) {
		if(!(tableRecordConfigs_.value(currentTableId).isValid())) {
			lastError_ = "Invalid record configuration";
			return false;
		}
	}

	return true;
}

bool AMGCS2SetDataRecorderConfigurationCommand::runImplementation()
{
	int recordConfigsCount = tableRecordConfigs_.count();

	AMCArrayHandler<int> tableIdsHandler(recordConfigsCount);
	AMCArrayHandler<int> recordOptionsHandler(recordConfigsCount);
	QString recordSourcesString;

	QList<int> tableIds = tableRecordConfigs_.keys();
	for (int iTableId = 0; iTableId < recordConfigsCount; ++iTableId) {

		int currentTableId = tableIds.at(iTableId);
		AMPIC887DataRecorderConfiguration currentRecordConfig =
				tableRecordConfigs_.value(currentTableId);

		int recordOptionsCode =
				AMGCS2Support::dataRecordOptionToInt(currentRecordConfig.recordOption());

		recordSourcesString.append(QString(" %1")
								   .arg(AMGCS2Support::dataRecordSourceToCharCode(currentRecordConfig.recordSource())));


		tableIdsHandler.cArray()[iTableId] = currentTableId;
		recordOptionsHandler.cArray()[iTableId] = recordOptionsCode;
	}

	bool success = PI_DRC(controller_->id(),
						  tableIdsHandler.cArray(),
						  recordSourcesString.toStdString().c_str(),
						  recordOptionsHandler.cArray());

	if(!success) {
		lastError_ = controllerErrorMessage();
	}

	return success;
}

