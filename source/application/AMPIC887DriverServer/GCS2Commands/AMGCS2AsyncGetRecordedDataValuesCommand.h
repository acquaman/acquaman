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
	AMGCS2AsyncGetRecordedDataValuesCommand(AMPIC887HexapodPosition* positionData);

	/// Virtual destructor
	virtual ~AMGCS2AsyncGetRecordedDataValuesCommand() {}

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

	/// The parsed position data read
	AMPIC887HexapodPosition* positionData_;

	/// The index of the previously read data index from the controller.
	int lastReadDataIndex_;

	int matchCounter_;

};

#endif // AMGCS2ASYNCGETRECORDEDDATAVALUESCOMMAND_H
