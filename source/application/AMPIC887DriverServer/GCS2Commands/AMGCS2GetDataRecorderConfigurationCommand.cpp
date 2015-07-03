#include "AMGCS2GetDataRecorderConfigurationCommand.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
#include "../AMGCS2Support.h"
#include "../AMPIC887Controller.h"
AMGCS2GetDataRecorderConfigurationCommand::AMGCS2GetDataRecorderConfigurationCommand()
{
}

AMGCS2GetDataRecorderConfigurationCommand::~AMGCS2GetDataRecorderConfigurationCommand()
{
	for(int iRecorderConfig = 0, recorderConfigCount = dataRecorderConfigurations_.count();
		iRecorderConfig < recorderConfigCount;
		++iRecorderConfig) {

		delete dataRecorderConfigurations_.at(iRecorderConfig);
	}

	dataRecorderConfigurations_.clear();
}

QString AMGCS2GetDataRecorderConfigurationCommand::outputString() const
{
	if(runningState_ != Succeeded) {
		return QString();
	}

	QString returnOutput;
	returnOutput.append(QString("Table\t\tSource\t\tOption (code)\n"));

	foreach(AMPIC887DataRecorderConfiguration* configuration, dataRecorderConfigurations_) {
		returnOutput.append(configuration->toString());
		returnOutput.append('\n');
	}

	return returnOutput.trimmed();
}

QList<AMPIC887DataRecorderConfiguration *> AMGCS2GetDataRecorderConfigurationCommand::dataRecorderConfigurations() const
{
	return dataRecorderConfigurations_;
}

bool AMGCS2GetDataRecorderConfigurationCommand::runImplementation()
{
	// Clear previous results
	for(int iRecorderConfig = 0, recorderConfigCount = dataRecorderConfigurations_.count();
		iRecorderConfig < recorderConfigCount;
		++iRecorderConfig) {

		delete dataRecorderConfigurations_.at(iRecorderConfig);
	}

	dataRecorderConfigurations_.clear();


	AMCArrayHandler<int> recordTableIds(RECORD_TABLE_COUNT);

	for (int iRecordTable = 0;
		 iRecordTable < RECORD_TABLE_COUNT;
		 ++iRecordTable) {

		recordTableIds.cArray()[iRecordTable] = iRecordTable + 1;
	}

	AMCArrayHandler<char> recordSourceIds(BUFFER_SIZE);
	AMCArrayHandler<int> recordOptionIds(RECORD_TABLE_COUNT);

	bool success = PI_qDRC(controller_->id(),
						   recordTableIds.cArray(),
						   recordSourceIds.cArray(),
						   recordOptionIds.cArray(),
						   BUFFER_SIZE,
						   RECORD_TABLE_COUNT);

	if(!success) {
		lastError_ = controllerErrorMessage();
	} else {

		QStringList axisCharacterList = QString(recordSourceIds.cArray()).split(" \n");

		for (int iRecordConfiguration = 0;
			 iRecordConfiguration < RECORD_TABLE_COUNT;
			 ++iRecordConfiguration) {

			dataRecorderConfigurations_.append(
						new AMPIC887DataRecorderConfiguration(
							recordTableIds.cArray()[iRecordConfiguration],
							AMGCS2Support::charCodeToDataRecordSource(axisCharacterList.at(iRecordConfiguration).at(0)),
							AMGCS2Support::intCodeToDataRecordOption(recordOptionIds.cArray()[iRecordConfiguration])
							));
		}
	}

	return success;
}
