#include "AMGCS2SetDataRecorderConfigurationCommand.h"
#include "util/AMCArrayHandler.h"
#include "AMGCS2Support.h"
#include "PI_GCS2_DLL.h"
AMGCS2SetDataRecorderConfigurationCommand::AMGCS2SetDataRecorderConfigurationCommand(
		const QList<AMPIC887DataRecorderConfiguration*>& configurations)
{
	configurations_ = configurations;
}

AMGCS2SetDataRecorderConfigurationCommand::~AMGCS2SetDataRecorderConfigurationCommand()
{
	foreach (AMPIC887DataRecorderConfiguration* configuration, configurations_) {
		delete configuration;
	}
}

bool AMGCS2SetDataRecorderConfigurationCommand::validateArguments()
{
	if (configurations_.isEmpty()) {
		lastError_ = "No configurations provided";
		return false;
	}

	foreach(AMPIC887DataRecorderConfiguration* configuration, configurations_) {
		if(!configuration->isValid()) {
			lastError_ = "Invalid configuration provided";
			return false;
		}
	}

	return true;
}

bool AMGCS2SetDataRecorderConfigurationCommand::runImplementation()
{
	AMCArrayHandler<int> recordTableIds(RECORD_TABLE_COUNT);
	AMCArrayHandler<int> recordOptionsArray(RECORD_TABLE_COUNT);
	QString recordSourcesString;

	for (int iRecordConfig = 0, configCount = configurations_.count();
		 iRecordConfig < configCount;
		 ++iRecordConfig) {

		AMPIC887DataRecorderConfiguration* currentConfig = configurations_.at(iRecordConfig);

		recordSourcesString.append(QString(" %1")
								   .arg(AMGCS2Support::dataRecordSourceToCharCode(currentConfig->recordSource())));

		recordTableIds.cArray()[iRecordConfig] = currentConfig->recordTableId();
		recordOptionsArray.cArray()[iRecordConfig] = AMGCS2Support::dataRecordOptionToInt(currentConfig->recordOption());
	}

	bool success = PI_DRC(controllerId_,
						  recordTableIds.cArray(),
						  recordSourcesString.toStdString().c_str(),
						  recordOptionsArray.cArray());

	if(!success) {
		lastError_ = controllerErrorMessage();
	}

	return success;

}

