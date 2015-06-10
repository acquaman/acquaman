#ifndef AMGCS2DEVICEIDENTIFICATIONCOMMAND_H
#define AMGCS2DEVICEIDENTIFICATIONCOMMAND_H

#include "AMGCS2Command.h"
/*!
  * A command representing a request for a controller's device identification
  * (model, serial # etc.).
  */
class AMGCS2DeviceIdentificationCommand : public AMGCS2Command
{
public:
	/*!
	  * Creates an instance of a device identification command ready to run on
	  * a controller.
	  */
	AMGCS2DeviceIdentificationCommand();

	/*!
	  * Virtual destructor for a device identification command
	  */
	~AMGCS2DeviceIdentificationCommand() {}

	/*!
	  * The device identification for the controller if the command was successfully
	  * run, or the empty string otherwise.
	  */
	virtual QString outputString() const;
protected:
	/*!
	  * Defines the steps taken to obtain the device identification from a controller.
	  */
	virtual bool runImplementation();

	QString deviceIdenfiticationString_;
};

#endif // AMGCS2DEVICEIDENTIFICATIONCOMMAND_H
