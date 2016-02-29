#ifndef AMPIC887DATARECORDERTABLESTATE_H
#define AMPIC887DATARECORDERTABLESTATE_H

#include "AMGCS2.h"
#include <QList>
#include <QString>
/*!
  * A class representing the state of a PI C887.11 Controller Data Recorder Table.
  * Subclasses from QList in order to provide an interface for storing the data
  * collected by the recorder in the table.
  *
  * NOTE: Table ids for the controller run from 1-16.
  */
class AMPIC887DataRecorderTableState : QList<double>
{
public:
	/*!
	  * Creates an instance of a data recorder table state with no data, and an
	  * uninitialized state which will store the state of the table with the provided
	  * id.
	  * \param tableId ~ The id of the table which this instance will store the
	  * state data of.
	  */
	AMPIC887DataRecorderTableState(int tableId);

	/*!
	  * Whether the state data has been initialized for this data store.
	  *
	  * NOTE: This does not necessarily reflect the initialized state of the recorded
	  * data (ie, the doubles stored in the list).
	  */
	bool isInitialized() const;

	/*!
	  * Initializes the basic state data for this data store.
	  * \param recordSource ~ The source of the recording to store in this state.
	  * \param recordOption ~ The record option (ie what will be recorded) to store
	  * in this state.
	  */
	void initialize(AMGCS2::DataRecordSource recordSource,
					AMGCS2::DataRecordOption recordOption);

	/*!
	  * The id of the data record table this instance is storing that data of.
	  */
	int id() const;

	/*!
	  * The source of recording stored in this state.
	  */
	AMGCS2::DataRecordSource recordSource() const;

	/*!
	  * Sets the source of recording which is stored in this state.
	  * \param store ~ The new source of recording which is stored in this state.
	  */
	void setRecordSource(AMGCS2::DataRecordSource recordSource);

	/*!
	  * The record option (i.e. what is being recorded) stored in this state.
	  */
	AMGCS2::DataRecordOption recordOption() const;

	/*!
	  * Sets the record option stored in this state.
	  * \param option ~ The new record option (i.e. what is being recorded) stored
	  * in this state.
	  */
	void setRecordOption(AMGCS2::DataRecordOption recordOption);

	/*!
	  * A status string describing the current state of the recorder table.
	  */
	QString statusString() const;
protected:
	bool isInitialized_;
	int id_;
	AMGCS2::DataRecordSource recordSource_;
	AMGCS2::DataRecordOption recordOption_;
};

#endif // AMPIC887DATARECORDERTABLESTATE_H
