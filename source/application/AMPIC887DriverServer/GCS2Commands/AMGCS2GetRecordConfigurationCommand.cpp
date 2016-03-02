#include "AMGCS2GetRecordConfigurationCommand.h"

#include <QStringList>
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
#include "../AMGCS2Support.h"
#include "../AMPIC887Controller.h"
AMGCS2GetRecordConfigurationsCommand::AMGCS2GetRecordConfigurationsCommand(const QList<int>& tableIds)
{
	tablesIdsToQuery_ = tableIds;
}

QHash<int, AMPIC887DataRecorderConfiguration> AMGCS2GetRecordConfigurationsCommand::recordConfigs() const
{
	return recordConfigs_;
}

bool AMGCS2GetRecordConfigurationsCommand::validateArguments()
{
	foreach(int currentTableId, tablesIdsToQuery_) {

		if(currentTableId < 1 || currentTableId > 16) {
			lastError_ = QString("Table id %1 is not valid")
					.arg(currentTableId);
			return false;
		}
	}

	return true;
}

bool AMGCS2GetRecordConfigurationsCommand::runImplementation()
{
	// Clear previous results
	recordConfigs_.clear();

	int tableQueryCount;
	if (tablesIdsToQuery_.isEmpty()) {
		tableQueryCount = RECORD_TABLE_COUNT;
	} else {
		tableQueryCount = tablesIdsToQuery_.count();
	}

	AMCArrayHandler<int> tableIdsHandler(tableQueryCount);
	AMCArrayHandler<int> recordOptionsHandler(tableQueryCount);
	AMCArrayHandler<char> recordSourcesHandler(BUFFER_SIZE);

	if(tablesIdsToQuery_.isEmpty()) {

		// No tables specified - Use all tables (from 1 to RECORD_TABLE_COUNT)
		for (int iTableId = 0; iTableId < RECORD_TABLE_COUNT; ++iTableId) {

			tableIdsHandler.cArray()[iTableId] = ( iTableId + 1 );
		}

	} else {

		for (int iTableId = 0; iTableId < tableQueryCount; ++iTableId) {

			tableIdsHandler.cArray()[iTableId] = tablesIdsToQuery_.at(iTableId);
		}
	}

	bool success = PI_qDRC(controller_->id(),
						   tableIdsHandler.cArray(),
						   recordSourcesHandler.cArray(),
						   recordOptionsHandler.cArray(),
						   BUFFER_SIZE,
						   tableQueryCount);

	if(!success) {
		lastError_ = controllerErrorMessage();
		return false;
	} else {

		QString recordSourcesString = recordSourcesHandler.cArray();
		QStringList recordSourcesStringList = recordSourcesString.split("\n");

		for(int iTableId = 0; iTableId < tableQueryCount; ++iTableId) {

			int tableId = tableIdsHandler.cArray()[iTableId];
			AMGCS2::DataRecordOption recordOption =
					AMGCS2Support::intCodeToDataRecordOption(recordOptionsHandler.cArray()[iTableId]);

			AMGCS2::DataRecordSource recordSource =
					AMGCS2Support::charCodeToDataRecordSource(recordSourcesStringList.at(iTableId).at(0));

			recordConfigs_.insert(tableId, AMPIC887DataRecorderConfiguration(recordSource, recordOption));
		}
	}

	return success;
}


