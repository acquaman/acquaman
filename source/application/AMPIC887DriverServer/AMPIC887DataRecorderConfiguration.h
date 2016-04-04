#ifndef AMPIC887DATARECORDERCONFIGURATION_H
#define AMPIC887DATARECORDERCONFIGURATION_H

#include "AMGCS2.h"

/*!
  * A class representing a set of data recorder options for the recording functionality
  * of a PI C887.11 Hexapod Controller.
  */
class AMPIC887DataRecorderConfiguration
{
public:
	/*!
	  * Creates an instance of a data recorder configuration which will set the
	  * provided record tables option to the provided axis source.
	  * \param recordSource ~ The source whose data will be stored in the record
	  * table.
	  * \param recordOption ~ Determines the type of data to be stored in the record
	  * table for the provided axis.
	  */
	AMPIC887DataRecorderConfiguration(AMGCS2::DataRecordSource recordSource = AMGCS2::UnknownDataRecordSource,
									  AMGCS2::DataRecordOption recordOption = AMGCS2::UnknownRecordOption );

	/*!
	  * Ensures that the record configuration is valid:
	  *  - the recordSource is not UnknownRecordSource
	  *  - the recordOption is not UnknownRecordOption
	  */
	bool isValid() const;

	/*!
	  * The source whose data is to be recorded.
	  */
	AMGCS2::DataRecordSource recordSource() const;

	/*!
	  * The option which determines what type of data the configuration specified
	  * should be recorded.
	  */
	AMGCS2::DataRecordOption recordOption() const;

	/*!
	  * A human readable string form of the data recorder configuration.
	  */
	QString toString() const;
protected:
	AMGCS2::DataRecordSource recordSource_;
	AMGCS2::DataRecordOption recordOption_;
};

#endif // AMPIC887DATARECORDERCONFIGURATION_H
