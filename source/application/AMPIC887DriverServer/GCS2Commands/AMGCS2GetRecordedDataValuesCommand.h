#ifndef AMGCS2GETRECORDEDDATAVALUESCOMMAND_H
#define AMGCS2GETRECORDEDDATAVALUESCOMMAND_H

#include "AMGCS2Command.h"
#include <QList>
/*!
  * A command representing a request for the values within a data record table
  * from a C887.11 controller.
  */
class AMGCS2GetRecordedDataValuesCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates a new instance of a get recorded data values command which will
	  * return the provided number of values from the record table with the given
	  * id, with an optional offset from the first record data point.
	  * \param recordTableId ~ The id of the record table from which the recorded
	  * data will be returned.
	  * \param numberOfValues ~ The number of values of data which are to be returned.
	  * \param offsetPoint ~ Optional offset position in the record table to return
	  * the data from. If none is provided the position will be the first data point.
	  */
	AMGCS2GetRecordedDataValuesCommand(int recordTableId,
									   int numberOfValues,
									   int offsetPoint = 0);

	/*!
	  * Virtual destructor for a get recorded data values command.
	  */
	virtual ~AMGCS2GetRecordedDataValuesCommand() {}

	/*!
	  * The values retrieved from the data recorder.
	  */
	QList<double> dataValues() const;

protected:
	/*!
	  * Ensures that the command arguments are valid:
	  *  - recordTableId is between 1 and 16
	  *  - offsetPoint is 0 or greater.
	  *  - numberOfValues is greater than 0
	  */
	virtual bool validateArguments();

	/*!
	  * Defines the steps taken to obtain the recorded data values from a controller.
	  */
	virtual bool runImplementation();

	int recordTableId_;
	int offsetPoint_;
	int numberOfValues_;
	QList<double> dataValues_;
};

#endif // AMGCS2GETRECORDEDDATAVALUESCOMMAND_H
