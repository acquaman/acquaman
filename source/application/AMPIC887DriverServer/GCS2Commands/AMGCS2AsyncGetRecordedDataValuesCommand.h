#ifndef AMGCS2ASYNCGETRECORDEDDATAVALUESCOMMAND_H
#define AMGCS2ASYNCGETRECORDEDDATAVALUESCOMMAND_H

#include <QVector>

#include "AMGCS2AsyncCommand.h"
#include "../AMPIC887HexapodPosition.h"
#include "util/AMCArrayHandler.h"
class AMGCS2AsyncGetRecordedDataValuesCommand : public AMGCS2AsyncCommand
{
    Q_OBJECT
public:
	/*!
	  * Creates a new instance of a get recorded data values command which will
	  * return the positional data of the hexapod from the data recorder.
	  * \param positionData ~ The inplace vector to fill with the recorded data
	  */
	AMGCS2AsyncGetRecordedDataValuesCommand(QVector<AMPIC887HexapodPosition>* positionData);

	/// Virtual destructor
	virtual ~AMGCS2AsyncGetRecordedDataValuesCommand() {}

	/*!
	  * The position data retrieved by running the command. If the state of the
	  * command is not Succeeded, then the returned vector will be empty.
	  */
	QVector<AMPIC887HexapodPosition> positionData() const;

signals:

public slots:
protected:

	/*!
	  * Defines the steps to take when the move command is run.
	  */
	virtual bool runImplementation();

	/*!
	  * Defines the steps taken to check whether the move command is still
	  * being performed.
	  */
	virtual void checkRunningState();

	/*!
	  * Helper funciton which parses the read data into the positionData_ vector.
	  */
	void parseReadData();

	/*!
	  * Pointer to the internal (to the GCS Library) array which is used to store
	  * the retrieved values.
	  */
	double* rawPositionData_;

	/// The number of values to obtain from each record table.
	int numberOfValuesToRead_;

	/// The parsed position data read
	QVector<AMPIC887HexapodPosition>* positionData_;

	/// The index of the previously read data index from the controller.
	int lastReadDataIndex_;

	int matchCounter_;

};

#endif // AMGCS2ASYNCGETRECORDEDDATAVALUESCOMMAND_H
