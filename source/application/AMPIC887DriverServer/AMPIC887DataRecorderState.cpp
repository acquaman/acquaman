#include "AMPIC887DataRecorderState.h"
#include "AMGCS2Support.h"
AMPIC887DataRecorderState::AMPIC887DataRecorderState()
{
	for(int iRecTable = 1; iRecTable <= 16 ; ++iRecTable) {

		tableStates_.append(new AMPIC887DataRecorderTableState(iRecTable));
	}

	recordTrigger_ = AMGCS2::UnknownRecordTrigger;
	isInitialized_ = false;
}

AMPIC887DataRecorderState::~AMPIC887DataRecorderState()
{
	foreach(AMPIC887DataRecorderTableState* tableState, tableStates_) {

		delete tableState;
	}
}

bool AMPIC887DataRecorderState::isAllInitialized() const
{
	return isInitialized_ &&
			tableStates_.at(0)->isInitialized() &&
			tableStates_.at(1)->isInitialized() &&
			tableStates_.at(2)->isInitialized() &&
			tableStates_.at(3)->isInitialized() &&
			tableStates_.at(4)->isInitialized() &&
			tableStates_.at(5)->isInitialized() &&
			tableStates_.at(6)->isInitialized() &&
			tableStates_.at(7)->isInitialized() &&
			tableStates_.at(8)->isInitialized() &&
			tableStates_.at(9)->isInitialized() &&
			tableStates_.at(10)->isInitialized() &&
			tableStates_.at(11)->isInitialized() &&
			tableStates_.at(12)->isInitialized() &&
			tableStates_.at(13)->isInitialized() &&
			tableStates_.at(14)->isInitialized() &&
			tableStates_.at(15)->isInitialized();
}

void AMPIC887DataRecorderState::initialize(AMGCS2::DataRecordTrigger recordTrigger,
										   const QString& recordOptionsString)
{
	recordTrigger_ = recordTrigger;
	recordOptionsString_ = recordOptionsString;
	isInitialized_ = true;
}

AMGCS2::DataRecordTrigger AMPIC887DataRecorderState::recordTrigger() const
{
	return recordTrigger_;
}

void AMPIC887DataRecorderState::setRecordTrigger(AMGCS2::DataRecordTrigger recordTrigger)
{
	recordTrigger_ = recordTrigger;
}

AMPIC887DataRecorderTableState * AMPIC887DataRecorderState::stateAt(int index)
{
	return tableStates_.at(index - 1);
}

QString AMPIC887DataRecorderState::recordOptionsString() const
{
	return recordOptionsString_;
}

QString AMPIC887DataRecorderState::statusString() const
{
	QString stateString = QString("Data Recorder:\nRecord Trigger: %1\n\nTables:\n")
			.arg(AMGCS2Support::dataRecordTriggerToString(recordTrigger_));

	foreach(AMPIC887DataRecorderTableState* tableState, tableStates_) {

		stateString.append(QString("%1\n\n").arg(tableState->statusString()));
	}

	return stateString;
}


