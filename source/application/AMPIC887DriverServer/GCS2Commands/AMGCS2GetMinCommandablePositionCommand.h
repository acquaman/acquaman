#ifndef AMGCS2GETMINCOMMANDABLEPOSITIONCOMMAND_H
#define AMGCS2GETMINCOMMANDABLEPOSITIONCOMMAND_H
#include "AMGCS2Command.h"
#include "../AMGCS2.h"
#include <QList>
#include <QHash>

/*!
  * A command representing a query to a PI C887.11 controller to report the minimum
  * commandable position for the provided axes.
  */
class AMGCS2GetMinCommandablePositionCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of a get min commandable position command which will
	  * report the minimum commandable position of the provided axes.
	  * \param axes ~ An optional list of axes whose minimum commandable position
	  * will be queried. If none is provided the minimum commandable position of
	  * all axes will be queried.
	  */
	AMGCS2GetMinCommandablePositionCommand(const QList<AMGCS2::Axis> axes = QList<AMGCS2::Axis>());

	/*!
	  * Virtual destructor for the get min commandable position command.
	  */
	virtual ~AMGCS2GetMinCommandablePositionCommand() {}

	/*!
	  * A human readable string representation of the reported min commandable
	  * positions.
	  * \returns Stringified representation of the reported min commandable positions
	  * if the command was run successfully, the empty string otherwise.
	  */
	virtual QString outputString() const;

	/*!
	  * The reported minimum commandable positions of the axes the command was
	  * initialized with.
	  * \returns A map of axes to min commandable position if the command was run
	  * successfully, an empty hash otherwise.
	  */
	QHash<AMGCS2::Axis, double> minCommandablePositions() const;

protected:
	/*!
	  * Ensures the arguments with which the command was initialzied are valid:
	  *  - Ensures that none of the axes in query list are the UnknownAxis
	  *  - Ensures that the number of provided axes is not greater than the total
	  *    number of axes.
	  */
	virtual bool validateArguments();

	/*!
	  * Defines the steps taken to query the min commandable positions from a
	  * controller.
	  */
	virtual bool runImplementation();

	QList<AMGCS2::Axis> axesToQuery_;
	QHash<AMGCS2::Axis, double> minCommandablePositions_;
};

#endif // AMGCS2GETMINCOMMANDABLEPOSITIONCOMMAND_H
