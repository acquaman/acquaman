#ifndef AMPIC887DATARECORDERSTATE_H
#define AMPIC887DATARECORDERSTATE_H
#include <QList>
#include <QString>
#include "AMGCS2.h"
#include "AMPIC887DataRecorderTableState.h"
/*!
  * A class for storing the data relating to a PI C887.11 controller's data recorder.
  * The structure of this state class mirrors that of the controller's recorder,
  * in that it contains 16 data recorder table states, each of which hold the state
  * for the 16 tables of the controller's recorder.
  */
class AMPIC887DataRecorderState
{
public:
	/*!
	  * Creates an instance of a data recorder state, with uninitialized data.
	  */
    AMPIC887DataRecorderState();

	/*!
	  * Frees the resources owned by this data recorder state.
	  */
	~AMPIC887DataRecorderState();

	/*!
	  * Whether all data for this data recorder state has been initialized. This
	  * includes both the top level data for this recorder state, and all the data
	  * for the contained 16 data recorder table states.
	  */
	bool isAllInitialized() const;

	/*!
	  * Initializes the top level data for this data record trigger store.
	  *
	  * NOTE: Data for the individual data tables will require initialization before
	  * isAllInitialized() will return true.
	  * \param recordTrigger ~ The trigger to initialize the state with.
	  * \param recordOptionsString ~ A string containing the record options to
	  * initialize the state with.
	  */
	void initialize(AMGCS2::DataRecordTrigger recordTrigger,
					const QString& recordOptionsString);

	/*!
	  * The current trigger setting for the recorder stored within this state.
	  */
	AMGCS2::DataRecordTrigger recordTrigger() const;

	/*!
	  * Sets the trigger setting for the recorder storted within this state.
	  * \param recordTrigger ~ The new trigger setting to store in this state.
	  */
	void setRecordTrigger(AMGCS2::DataRecordTrigger recordTrigger);

	/*!
	  * The state of the individual recorder table at the provided index.
	  *
	  * NOTE: Indices for the PI C887.11 controller run from 1-16. No other index
	  * is valid.
	  * \param tableId ~ The id of the table whose recorder state is to be returned.
	  */
	AMPIC887DataRecorderTableState* stateAt(int tableId);

	/*!
	  * The record options string stored within this state.
	  */
	QString recordOptionsString() const;

	/*!
	  * A string describing the status of the data recorder and all its contained
	  * tables.
	  */
	QString statusString() const;
protected:

	bool isInitialized_;

	AMGCS2::DataRecordTrigger recordTrigger_;
	QString recordOptionsString_;
	QList<AMPIC887DataRecorderTableState*> tableStates_;
};

#endif // AMPIC887DATARECORDERSTATE_H
