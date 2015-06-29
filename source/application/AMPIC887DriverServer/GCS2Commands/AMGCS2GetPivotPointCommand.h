#ifndef AMGCS2GETPIVOTPOINTCOMMAND_H
#define AMGCS2GETPIVOTPOINTCOMMAND_H
#include "AMGCS2Command.h"
#include "../AMGCS2.h"
#include <QList>
#include <QHash>

/*!
  * A command representing a query for the rotational axes pivot points of a PI
  * C887.11 controller.
  */
class AMGCS2GetPivotPointCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of a get pivot point command which will report the
	  * current pivot point position within the provided axes.
	  * \param ~ Optional list of the axes whose pivot point is to be reported.
	  * Must be of axes X, Y and Z (the pivot point is defined in the plane of the
	  * platforms motions). If none are provided then the pivot point positions
	  * in the X, Y and Z axes will be returned.
	  */
	AMGCS2GetPivotPointCommand(const QList<AMGCS2::Axis> axes = QList<AMGCS2::Axis>());

	/*!
	  * Virtual destructor for a get pivot point command
	  */
	virtual ~AMGCS2GetPivotPointCommand() {}

	/*!
	  * Outputs the pivot point poisition as a human readable string.
	  * \returns Stringified version of the pivot point if the command has been
	  * run successfully, the empty string otherwise.
	  */
	virtual QString outputString() const;

	/*!
	  * A mapping of Axis to the position of the pivot point within that axis.
	  * \returns Mapping of axis to pivot point position if the command has been
	  * run successfully, an empty hash otherwise.
	  */
	QHash<AMGCS2::Axis, double> axisPivotPoints() const;

protected:
	/*!
	  * Ensures the arguments with which the command was initialized are valid:
	  *  - Ensures that none of the axes in the parameter the command was initialized
	  *    with are the UnknownAxis, UAxis, VAxis or WAxis.
	  *  - The number of axes provided is not greater than the total possible axes
	  *    number.
	  */
	virtual bool validateArguments();

	/*!
	  * Defines the steps taken to query the pivot point position from a controller.
	  */
	virtual bool runImplementation();

	QList<AMGCS2::Axis> axesToQuery_;

	QHash<AMGCS2::Axis, double> axisPivotPoints_;
};

#endif // AMGCS2GETPIVOTPOINTCOMMAND_H
