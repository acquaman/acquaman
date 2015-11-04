#ifndef AMPIC887DATARECORDERSTATE_H
#define AMPIC887DATARECORDERSTATE_H
#include <QHash>
#include <QString>
#include "AMGCS2.h"
#include "AMPIC887DataRecorderConfiguration.h"
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
	  * Virtual destructor for a data recorder state
	  */
	~AMPIC887DataRecorderState() {}

	/*!
	  * Whether all data for this data recorder state is valid for the safe
	  * operation of the data recorder.
	  */
	bool isDataValid() const;

	/*!
	  * Initializes the data for this data record store.
	  * \param recordTrigger ~ The trigger to initialize the state with.
	  * \param availableRecordParameters ~ Text describing each of the parameters
	  * available for recording on the controller.
	  * \param recordRate ~ The record Rate of the controller in hertz.
	  * \param recordConfigs ~ The record configs to initialize the state with.
	  */
	void initialize(AMGCS2::DataRecordTrigger recordTrigger,
					const QString& availableRecordParameters,
					double recordRate,
					const QHash<int, AMPIC887DataRecorderConfiguration>& recordConfigs);

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
	AMPIC887DataRecorderConfiguration recordConfig(int tableId) const;

	/*!
	  * Sets the record config for the provided table.
	  */
	void setRecordConfig(int tableId, AMPIC887DataRecorderConfiguration recordConfig);

	/*!
	  * The current data recorder record rate, in hertz.
	  */
	double recordRate() const;

	/*!
	  * Sets the controller record rate to the provided value
	  * \param recordRate ~ The new record rate to set (in hertz).
	  */
	void setRecordRate(double recordRate);

	/*!
	  * The record configs for all the data tables of the controller.
	  */
	QHash<int, AMPIC887DataRecorderConfiguration> recordConfigs() const;

	/*!
	  * Sets the stored record configs based on the values in the provided hash.
	  * \param recordConfigs ~ The hash containing values to update the contained
	  * record configs with.
	  */
	void setRecordConfigs(const QHash<int, AMPIC887DataRecorderConfiguration>& recordConfigs);

	/*!
	  * Text describing all the parameters available for recording on the controller.
	  */
	QString availableParameters() const;

	/*!
	  * A string describing the status of the data recorder and all its contained
	  * tables.
	  */
	QString statusString() const;
protected:

	AMGCS2::DataRecordTrigger recordTrigger_;
	QString availableRecordParameters_;
	QHash<int, AMPIC887DataRecorderConfiguration> recordConfigs_;
	double recordRate_;

};

#endif // AMPIC887DATARECORDERSTATE_H
