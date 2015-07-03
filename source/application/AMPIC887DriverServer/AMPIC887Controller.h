#ifndef AMPIC887CONTROLLER_H
#define AMPIC887CONTROLLER_H

#define CONTROLLER_PORT 50000 // PI C887.11 controller are hardwired to use port 50000

#include <QObject>
#include <QString>
#include <QTimer>
#include "GCS2Commands/AMGCS2Command.h"
#include "GCS2Commands/AMGCS2AsyncCommand.h"

/*!
  * A class representing a single PI C887.11 Controller. Commands and queries issued
  * to instances of this class will, in turn, pass them along to the controller
  * they represent.
  */
class AMPIC887Controller : public QObject
{
	Q_OBJECT
public:
	/*!
	  * Creates a new instance of an C887.11 controller, with the provided hostname
	  * and id. The controller will attempt to make a conneciton immediately
	  * (connection status can be checked with connectionEstablished()).
	  *
	  * \param name ~ A human readable name which this controller should be known
	  * by.
	  * \param hostname ~ Currently the PI_C887 library will only connect to a controller
	  * by ip address. Therefore the hostname specified should be the ip address
	  * of the controller.
	  */
	AMPIC887Controller(const QString& name, const QString& hostname);

	/*!
	  * Virtual destructor for a pi c887 controller.
	  */
	virtual ~AMPIC887Controller(){}

	/*!
	  * Attempts to interpret the provided command text as a GCS2 command, and
	  * if it can, run it.
	  * \param commandText ~ The text input to the console which is to be interpreted
	  * as a GCS2 command
	  */
	void interpretAndRunCommand(const QString& commandText);

	/*!
	  * Runs the provided command.
	  * \param command ~ The command to run.
	  */
	void runCommand(AMGCS2Command* command);

	/*!
	  * Connects/reconnects to the controller represented by this class.
	  * \returns True if a connection could be established with the controller,
	  * false otherwise
	  */
	bool connectToController();

	/*!
	  * Disconnects from the controller represented by this class.
	  */
	void disconnectFromController();

	/*!
	  * Whether a connection has yet been established with this controller.
	  */
	bool connectionEstablished() const;

	/*!
	  * The name by which the controller is known.
	  */
	QString name() const;

	/*!
	  * The address of the controller on the network.
	  */
	QString hostname() const;

	/*!
	  * The unique key identifier of the controller.
	  */
	int id() const;

	/*!
	  * Returns a string describing the status of the controller (its name,
	  * id and hostname).
	  */
	QString status() const;

	/*!
	  * Whether the controller is currently not in a state to receive commands.
	  */
	bool isBusy() const;

	/*!
	  * Instructs the controller to clear its error message. This cannot be done
	  * instantly, and so the controller will wait asynchronously for a small amount
	  * of time before clearing the error. Check whether the controller is busy
	  * clearning an error message with isBusy().
	  */
	void clearErrorMessage();

signals:
	/*!
	  * Signal indicating that the controller has some output text which. For commands
	  * this signal is only emitted for those input manually into the console terminal.
	  * \param outputText ~ The text which the controller is outputting.
	  */
	void output(const QString& outputText);

	/*!
	  * Signal indicating that the controller has encountered an error of some
	  * description. For commands this signal is only emitted for those input manually
	  * into the console terminal.
	  * \param errorMessage ~ The error message associated with the error.
	  */
	void errorEncountered(const QString& errorMessage);
protected slots:
	/*!
	  * Handles an asynchronous command indicating that it has failed.
	  * \param command ~ The command which failed.
	  */
	void onAsyncCommandFailed(AMGCS2AsyncCommand* command);

	/*!
	  * Handles an asynchronous command indicating that it has succeeded.
	  * \param command ~ The command which succeeded.
	  */
	void onAsyncCommandSucceeded(AMGCS2AsyncCommand* command);

	/*!
	  * Handles the error clearning timer indicating that its delay time for the
	  * error message to be cleared is complete.
	  */
	void onErrorClearingTimerTimedOut();
protected:

	QString name_;
	QString hostname_;
	QTimer errorClearingTimer_;
	bool isBusy_;
	int id_;
};

#endif // AMPIC887CONTROLLER_H
