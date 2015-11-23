#ifndef AMGCS2ASYNCREFERENCEMOVECOMMAND_H
#define AMGCS2ASYNCREFERENCEMOVECOMMAND_H
#include "AMGCS2AsyncCommand.h"

#include <QList>
#include "../AMGCS2.h"
#include "../AMPIC887AxisCollection.h"
#include "../AMPIC887AxisMap.h"

/*!
  * An asynchronous command which performs a reference move on the provided axes,
  * then periodically checks if the reference move has finished. Completed signal
  * is emitted in the case that the command completes successfully. Failed signal
  * is emitted if the command fails or the timer times out.
  */
class AMGCS2AsyncReferenceMoveCommand : public AMGCS2AsyncCommand
{
	Q_OBJECT
public:
	/*!
	  * Creates an instance of a reference move command on the provided list of
	  * axes.
	  * \param axesToReference ~ An optional list of the axes on whom a reference move will
	  * be performed. If none is provided a reference move will be performed on
	  * all axes.
	  */
	AMGCS2AsyncReferenceMoveCommand(const AMPIC887AxisCollection& axesToReference = AMPIC887AxisCollection());

	/*!
	  * Virtual destructor for an asynchronous reference move command.
	  */
	virtual ~AMGCS2AsyncReferenceMoveCommand() {}

protected:
	/*!
	  * Defines the steps taken to start the reference move command on a controller.
	  */
	virtual bool runImplementation();

	/*!
	  * Defines the steps taken to check whether the current state of the reference
	  * move. If a change of state is detected, sets the current running state of
	  * the command and, if failed, and relevant error messages.
	  */
	virtual void checkRunningState();

	AMPIC887AxisCollection axesToReference_;
};

#endif // AMGCS2ASYNCREFERENCEMOVECOMMAND_H
