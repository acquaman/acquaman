#include "AMPIC887DataRecorderState.h"
#include "AMGCS2Support.h"
AMPIC887DataRecorderState::AMPIC887DataRecorderState()
{
	recordTrigger_ = AMGCS2::UnknownRecordTrigger;
}

bool AMPIC887DataRecorderState::isDataValid() const
{
	bool allConfigsValid = true;
	QList<int> recordTableIds = recordConfigs_.keys();
	foreach(int currentTableId, recordTableIds ) {

		AMPIC887DataRecorderConfiguration currentRecordConfig = recordConfigs_.value(currentTableId);
		allConfigsValid &= ( currentTableId >= 0 && currentTableId <= RECORD_TABLE_COUNT );
		allConfigsValid &= ( currentRecordConfig.isValid() );
	}

	return allConfigsValid &&
			!availableRecordParameters_.isEmpty() &&
			recordTrigger_ != AMGCS2::UnknownRecordTrigger;
}

void AMPIC887DataRecorderState::initialize(AMGCS2::DataRecordTrigger recordTrigger,
										   const QString& availableRecordParameters,
										   double recordRate,
										   const QHash<int, AMPIC887DataRecorderConfiguration>& recordConfigs)
{
	recordTrigger_ = recordTrigger;
	availableRecordParameters_ = availableRecordParameters;
	recordRate_ = recordRate;
	recordConfigs_ = recordConfigs;
}

AMGCS2::DataRecordTrigger AMPIC887DataRecorderState::recordTrigger() const
{
	return recordTrigger_;
}

void AMPIC887DataRecorderState::setRecordTrigger(AMGCS2::DataRecordTrigger recordTrigger)
{
	recordTrigger_ = recordTrigger;
}

AMPIC887DataRecorderConfiguration AMPIC887DataRecorderState::recordConfig(int tableId) const
{
	return recordConfigs_.value(tableId, AMPIC887DataRecorderConfiguration(AMGCS2::UnknownDataRecordSource,
																		   AMGCS2::UnknownRecordOption));
}

void AMPIC887DataRecorderState::setRecordConfig(int tableId, AMPIC887DataRecorderConfiguration recordConfig)
{
	if(tableId < 1 || tableId > 16) {

		return;
	}

	recordConfigs_.insert(tableId, recordConfig);
}

double AMPIC887DataRecorderState::recordRate() const
{
	return recordRate_;
}

void AMPIC887DataRecorderState::setRecordRate(double recordRate)
{
	recordRate_ = recordRate;
}

QHash<int, AMPIC887DataRecorderConfiguration> AMPIC887DataRecorderState::recordConfigs() const
{
	return recordConfigs_;
}

void AMPIC887DataRecorderState::setRecordConfigs(const QHash<int, AMPIC887DataRecorderConfiguration> &recordConfigs)
{
	QList<int> tableIds = recordConfigs.keys();
	foreach(int currentTableId, tableIds) {

		recordConfigs_.insert(currentTableId, recordConfigs.value(currentTableId));
	}
}

QString AMPIC887DataRecorderState::availableParameters() const
{
	return availableRecordParameters_;
}

QString AMPIC887DataRecorderState::statusString() const
{
	QString stateString = QString("Data Recorder:\nRecord Trigger: %1\n\nTables:\n")
			.arg(AMGCS2Support::dataRecordTriggerToString(recordTrigger_));

	QList<int> tableIds = recordConfigs_.keys();

	foreach(int currentTableId, tableIds) {

		stateString.append(QString("%1\n")
						   .arg(recordConfigs_.value(currentTableId).toString()));
	}

	return stateString;
}










