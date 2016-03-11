#ifndef AMGCS2HALTSMOOTHLYCOMMAND_H
#define AMGCS2HALTSMOOTHLYCOMMAND_H
#include "AMGCS2Command.h"
#include "../AMPIC887AxisCollection.h"
#include "../AMGCS2.h"
/*!
  * A command which represents an instruction for a PI C887.11 Controller to smoothly
  * halt the motions of its axes.
  */
class AMGCS2HaltSmoothlyCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of a halt motion smoothly command which is ready to
	  * run on a controller.
	  * \param axes ~ An optional list of the axes to be halted. If none is provided
	  * then all the axes of the controller will be issued the halt command
	  */
	explicit AMGCS2HaltSmoothlyCommand(const AMPIC887AxisCollection& axes
									   = AMPIC887AxisCollection());

	/*!
	  * Virtual destructor for a halt smoothly command.
	  */
	virtual ~AMGCS2HaltSmoothlyCommand() {}

protected:
	/*!
	  * Defines the steps necessary to instruct a controller to halt the motions
	  * of its axes in a smooth manner.
	  */
	virtual bool runImplementation();

	AMPIC887AxisCollection axesToHalt_;
};

#endif // AMGCS2HALTSMOOTHLYCOMMAND_H
