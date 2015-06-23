#ifndef AMGCS2GETSERVOMODECOMMAND_H
#define AMGCS2GETSERVOMODECOMMAND_H

#include "AMGCS2Command.h"
#include "../AMGCS2.h"
#include <QList>
#include <QHash>
/*!
  * A command representing a query for a PI C887.11 controller to report the servo
  * mode statuses of a list of axes.
  */
class AMGCS2GetServoModeCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of a get servo mode command which will query the servo
	  * mode status of the provided axes.
	  * \param axes ~ An optional list of the axes whose servo mode statuses the
	  * command will query. If none is provided the servo mode statuses of all axes
	  * will be queried.
	  */
	AMGCS2GetServoModeCommand(const QList<AMGCS2::Axis>& axes = QList<AMGCS2::Axis>());

	/*!
	  * Virtual destructor for the get servo mode command
	  */
	virtual ~AMGCS2GetServoModeCommand() {}

	/*!
	  * The servo mode status of the axes the command was initialized with
	  * in a human readable form.
	  * \returns Stringified form of the servo mode statuses if the command was
	  * run successfully, the empty string otherwise.
	  */
	virtual QString outputString() const;

	/*!
	  * The servo mode statuses reported by the controller.
	  * \param A mapping of axes to servo mode status (true = servo mode active)
	  * if the command was run successfully, an empty map otherwise.
	  */
	QHash<AMGCS2::Axis, bool> servoModeStatuses() const;

protected:
	/*!
	  * Ensures the arguments that the command was initialized with are valid:
	  *  - Ensures that none of the axes in the list of queried axes are the UnknownAxis.
	  *  - Ensures that the number of axes provided is not greater than the total
	  *    number of possible axes.
	  */
	virtual bool validateArguments();

	/*!
	  * Defines the steps taken to query the servo mode status of axes from a controller.
	  */
	virtual bool runImplementation();

	QList<AMGCS2::Axis> axesToQuery_;
	QHash<AMGCS2::Axis, bool> servoModeStatuses_;
};

#endif // AMGCS2GETSERVOMODECOMMAND_H
