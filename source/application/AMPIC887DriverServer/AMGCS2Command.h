#ifndef AMGCS2COMMAND_H
#define AMGCS2COMMAND_H

#define BUFFER_SIZE 1000
#define AXIS_COUNT 6
#define RECORD_TABLE_COUNT 16

#include <QString>
/*!
  * Abstract class which represents a command which can be run on an AMPIC887Controller,
  * reporting whether the command was successful and its last encountered error
  * if one has occurred.
  */
class AMGCS2Command
{
public:
	/*!
	  * Base constructor for an AMGCS2Command. Sets the wasSuccessful() state to
	  * false.
	  * \param controllerId ~ The id of the controller who is to be issued the command.
	  */
	AMGCS2Command();

	/*!
	  * Base virtual destructor for an AMGCS2Command
	  */
	virtual ~AMGCS2Command() {}

	/*!
	  * Whether the AMGCS2Command was run successfully. If the command has not yet
	  * been run then a value of false is returned.
	  */
	bool wasSuccessful() const;

	/*!
	  * The last encountered error message for the AMGCS2Command. If the command
	  * was last run successfully then an empty message will be returned.
	  */
	QString lastError() const;

	/*!
	  * Runs the command.
	  */
	void run();

	/*!
	  * Sets the id of the controller upon which this command will be run.
	  */
	void setControllerId(int id);

	/*!
	  * The output message which contains the output for the command in a stringified
	  * form. Override this function if you wish a command to display some output.
	  */
	virtual QString outputString() const;
protected:

	/*!
	  * Function which is called by the run command, and represents the details
	  * of the command to be run. Implementations of an AMGCS2Command should include
	  * within this funciton all the generic calls to the PI_GCS2 library. If an
	  * error is encountered implementations should set the lastError_ variable
	  * to a descriptive message, and then return false. Successful completion of
	  * the command should then return true.
	  * \returns True if the command was completed successfully, false otherwise.
	  */
	virtual bool runImplementation() = 0;

	/*!
	  * Function which can be implemented by commands which require some validation
	  * of their arguments. In the base class this defaults to true, but if a subclass
	  * wishes to prevent the running of a command upon failed validation of arguments
	  * this function can be overridden in order to return false upon failed validation.
	  * In this case a descriptive error message should be set by the implementation
	  * before returning false.
	  */
	virtual bool validateArguments() { return true;}

	/*!
	  * Helper function which returns the current error message from the PI library.
	  */
	QString controllerErrorMessage();

	bool wasSuccessful_;
	QString lastError_;
	int controllerId_;

};

#endif // AMGCS2COMMAND_H
