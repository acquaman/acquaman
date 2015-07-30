#ifndef AMGCS2COMMAND_H
#define AMGCS2COMMAND_H

#define BUFFER_SIZE 2500

#include <QString>
/*!
  * Abstract class which represents a command which can be run on an AMPIC887Controller,
  * reporting whether the command was successful and its last encountered error
  * if one has occurred.
  *
  * NOTE: Synchronous commands should inherit directly from AMGCS2Command, and will
  * have the type Synchronous. Asynchronous commands should inherit from AMGCS2AsynCommand
  * and will have the type Asynchronous.
  */
class AMPIC887Controller;
class AMGCS2Command
{
public:

	/*!
	  * Enumerates all the different running states of a command. For syncrhonous
	  * command only the NotStarted, Succeeded or Failed running states should be
	  * used.
	  */
	enum RunningState {
		NotStarted,
		Running,
		Succeeded,
		Failed
	};

	/*!
	  * Enumerates between the synchronous and asynchronous command types.
	  */
	enum CommandType {
		Synchronous,
		Asynchronous
	};

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
	  * The current running state of the command.
	  */
	RunningState runningState() const;

	/*!
	  * The last encountered error message for the AMGCS2Command. If the command
	  * was last run successfully then an empty message will be returned.
	  */
	QString lastError() const;

	/*!
	  * Runs the command. Note, for defining a single command this should not be
	  * overridden. Instead override the runImplementation() command.
	  */
	virtual void run();

	/*!
	  * Sets the controller upon which this command will be run.
	  */
	void setController(AMPIC887Controller* controller);

	/*!
	  * Whether the command is synchronous or asynchronous.
	  */
	CommandType commandType() const;

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

	QString lastError_;
	AMPIC887Controller* controller_;
	RunningState runningState_;
	CommandType commandType_;
};

#endif // AMGCS2COMMAND_H
