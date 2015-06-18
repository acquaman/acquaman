#ifndef AMGCS2GETREFERENCERESULTCOMMAND_H
#define AMGCS2GETREFERENCERESULTCOMMAND_H

#include "AMGCS2Command.h"
#include "AMGCS2.h"
#include <QList>
#include <QHash>

/*!
  * A command representing a query to obtain the reference status of the provided
  * axes to a controller. Move commands cannot be issued to axes which have not
  * yet had a reference move performed.
  */
class AMGCS2GetReferenceResultCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of a get reference result command which will report
	  * the referenced status of the provided axes.
	  * \param axes ~ An optional list of the axes whose reference status is to
	  * be returned. If none is provided the command will request the reference
	  * status of all axes.
	  */
	AMGCS2GetReferenceResultCommand(const QList<AMGCS2::Axis> & axes = QList<AMGCS2::Axis>());

	/*!
	  * Virtual destructor for a get reference results command
	  */
	virtual ~AMGCS2GetReferenceResultCommand() {}

	/*!
	  * Outputs the reference status of each of the queried axes if the command
	  * was run successfully, the empty string otherwise.
	  */
	virtual QString outputString() const;

	/*!
	  * A mapping of the axes provided in the initialization of the command to their
	  * current reference status if the command was run successfully, otherwise
	  * an empty map.
	  */
	QHash<AMGCS2::Axis, bool> axesReferenceResults() const;
protected:
	/*!
	  * Ensures that the arugments passed to the command are valid:
	  *  - Ensures that all the axes the command was initialized with are not the
	  *    UnknownAxis.
	  */
	virtual bool validateArguments();

	/*!
	  * Defines the steps taken to query the reference status of the axes from
	  * a controller.
	  */
	virtual bool runImplementation();

	QList<AMGCS2::Axis> axes_;
	QHash<AMGCS2::Axis, bool> axisReferenceResults_;
};

#endif // AMGCS2GETREFERENCERESULTCOMMAND_H
