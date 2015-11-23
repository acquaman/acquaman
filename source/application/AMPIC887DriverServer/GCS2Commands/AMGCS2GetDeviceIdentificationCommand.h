#ifndef AMGCS2GETDEVICEIDENTIFICATIONCOMMAND_H
#define AMGCS2GETDEVICEIDENTIFICATIONCOMMAND_H

#include "AMGCS2Command.h"
/*!
  * A command representing a request for a controller's device identification
  * (model, serial # etc.).
  */
class AMGCS2GetDeviceIdentificationCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of a device identification command ready to run on
	  * a controller.
	  */
	AMGCS2GetDeviceIdentificationCommand();

	/*!
	  * Virtual destructor for a device identification command
	  */
	~AMGCS2GetDeviceIdentificationCommand() {}

	/*!
	  * The obtained device identification if the command was run successfully,
	  * the empty string otherwise.
	  */
	QString deviceIdentification() const;

protected:
	/*!
	  * Defines the steps taken to obtain the device identification from a controller.
	  */
	virtual bool runImplementation();

	QString deviceIdenfiticationString_;
};

#endif // AMGCS2GETDEVICEIDENTIFICATIONCOMMAND_H
