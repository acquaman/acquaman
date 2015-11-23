#include "AMPIC887DataRecorderTableState.h"
#include "AMGCS2Support.h"
AMPIC887DataRecorderTableState::AMPIC887DataRecorderTableState(int tableId)
{
	id_ = tableId;
	recordSource_ = AMGCS2::UnknownDataRecordSource;
	recordOption_ = AMGCS2::UnknownRecordOption;
	isInitialized_ = false;
}

bool AMPIC887DataRecorderTableState::isInitialized() const
{
	return isInitialized_;
}

void AMPIC887DataRecorderTableState::initialize(AMGCS2::DataRecordSource recordSource,
												AMGCS2::DataRecordOption recordOption)
{
	recordSource_ = recordSource;
	recordOption_ = recordOption;
	isInitialized_ = true;
}

int AMPIC887DataRecorderTableState::id() const
{
	return id_;
}

AMGCS2::DataRecordSource AMPIC887DataRecorderTableState::recordSource() const
{
	return recordSource_;
}

void AMPIC887DataRecorderTableState::setRecordSource(AMGCS2::DataRecordSource recordSource)
{
	recordSource_ = recordSource;
}

AMGCS2::DataRecordOption AMPIC887DataRecorderTableState::recordOption() const
{
	return recordOption_;
}

void AMPIC887DataRecorderTableState::setRecordOption(AMGCS2::DataRecordOption recordOption)
{
	recordOption_ = recordOption;
}

QString AMPIC887DataRecorderTableState::statusString() const
{
	if(!isInitialized_) {
		return QString("Data for table %1 not initialized").arg(id_);
	}

	QString stateString = QString("Recorder Table Id: %1\nData Source: %2\nOption: %3")
			.arg(id_)
			.arg(AMGCS2Support::dataRecordSourceToString(recordSource_))
			.arg(AMGCS2Support::dataRecordOptionToString(recordOption_));

	return stateString;
}
