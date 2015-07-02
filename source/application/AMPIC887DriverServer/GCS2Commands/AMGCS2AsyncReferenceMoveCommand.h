#ifndef AMGCS2ASYNCREFERENCEMOVECOMMAND_H
#define AMGCS2ASYNCREFERENCEMOVECOMMAND_H
#include "AMGCS2AsyncCommand.h"

#include <QList>
#include "../AMGCS2.h"

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
	  * \param axes ~ An optional list of the axes on whom a reference move will
	  * be performed. If none is provided a reference move will be performed on
	  * all axes.
	  */
	AMGCS2AsyncReferenceMoveCommand(const QList<AMGCS2::Axis>& axes = QList<AMGCS2::Axis>());

	/*!
	  * Virtual destructor for an asynchronous reference move command.
	  */
	virtual ~AMGCS2AsyncReferenceMoveCommand() {}

	/*!
	  * Text which can be read when the reference move has been completed.
	  * \returns A message indicating the success of the reference move if
	  */
	virtual QString outputString() const;

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
	virtual void isFinishedImplementation();

	QList<AMGCS2::Axis> axesToReference_;
};

#endif // AMGCS2ASYNCREFERENCEMOVECOMMAND_H
