#ifndef AMPIC887CONTROLLER_H
#define AMPIC887CONTROLLER_H

#define CONTROLLER_PORT 50000 // PI C887.11 controller are hardwired to use port 50000

#include <QObject>
#include <QString>
#include <QTimer>
#include <QHash>
#include "GCS2Commands/AMGCS2Command.h"
#include "GCS2Commands/AMGCS2AsyncCommand.h"

#include "AMPIC887ControllerState.h"
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
	  * Virtual destructor for a pi c887 controller. Frees the resources owned by
	  * this controller.
	  */
	virtual ~AMPIC887Controller();

	/*!
	  * Attempts to interpret the provided command text as a GCS2 command, and
	  * if it can, run it.
	  * \param commandText ~ The text input to the console which is to be interpreted
	  * as a GCS2 command
	  */
	void interpretAndRunCommand(const QString& commandText);

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

	/*!
	  * The current movement status of the controller's hexapod.
	  */
	QFlags<AMGCS2::AxisMovementStatus> movementStatus() const;
public slots:
	// Command Functions
	/////////////////////

	/*!
	  * Moves the controller hexapod axes to the provided positions.
	  * \param axisPositions ~ A mapping of axis to its new target position.
	  */
	void move(const QHash<AMGCS2::Axis, double>& axisPositions);

	/*!
	  * Performs a relative move on the controller hexapod axes, setting their
	  * target positions to the current target positions plus the values found
	  * in the provided hash.
	  * \param relativePositions ~ A mapping of axis to the values to add to
	  * the current target positions to provide the new target positions.
	  */
	void moveRelative(const QHash<AMGCS2::Axis, double>& relativePositions);

	/*!
	  * Performs a reference move upon the provided list of axes.
	  * \param axes ~ An optional list of the axes to reference. If none is provided
	  * a reference move will be performed on all axes.
	  */
	void referenceMove(const QList<AMGCS2::Axis>& axes = QList<AMGCS2::Axis>());
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
	void errorEncountered(const QString& errorMessage) const;


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
	  * Handles signals indicating that an asynchronous move command has started
	  * \param command ~ The asynchronous move command that has started.
	  */
	void onAsyncMoveStarted(AMGCS2AsyncCommand* command);

	/*!
	  * Handles signals indicating that an asynchronous move command has succeeded.
	  * \param command ~ The asynchronous move command that has succeeded.
	  */
	void onAsyncMoveSucceeded(AMGCS2AsyncCommand* command);

	/*!
	  * Handles signals indicating that an asynchronous move command has failed.
	  * \param command ~ The asynchronous move command that has failed.
	  */
	void onAsyncMoveFailed(AMGCS2AsyncCommand* command);

	/*!
	  * Handles signals indicating that an asynchronous move relative command has
	  * started.
	  * \param command ~ The asynchronous move relative command that has started.
	  */
	void onAsyncMoveRelativeStarted(AMGCS2AsyncCommand* command);

	/*!
	  * Handles signals indicating that an asynchronous move relative command has
	  * succeeded.
	  * \param command ~ The asynchronous move relative command that has succeeded.
	  */
	void onAsyncMoveRelativeSucceeded(AMGCS2AsyncCommand* command);

	/*!
	  * Handles signals indicating that an asynchronous move relative command has
	  * failed.
	  * \param command ~ The asynchronous move relative command that has failed.
	  */
	void onAsyncMoveRelativeFailed(AMGCS2AsyncCommand* command);

	/*!
	  * Handles signals indicating that an asynchronous reference move has started.
	  */
	void onAsyncReferenceMoveStarted(AMGCS2AsyncCommand*);

	/*!
	  * Handles signals indicating that an asynchronous reference move has succeeded.
	  */
	void onAsyncReferenceMoveSucceeded(AMGCS2AsyncCommand*);

	/*!
	  * Handles signals indicating that an asynchronous reference move has failed.
	  */
	void onAsyncReferenceMoveFailed(AMGCS2AsyncCommand*);

	/*!
	  * Handles the error clearning timer indicating that its delay time for the
	  * error message to be cleared is complete.
	  */
	void onErrorClearingTimerTimedOut();
protected:

	/*!
	  * Initializes the controller state data by running a command to obtain the
	  * data from the controller.
	  */
	void initializeControllerStateData();

	/*!
	  * Runs the provided command.
	  * \param command ~ The command to run.
	  */
	void runCommand(AMGCS2Command* command);

	AMPIC887ControllerState* controllerState_;
	QString name_;
	QString hostname_;
	QTimer errorClearingTimer_;
	bool isBusy_;
	int id_;

	//State data
	////////////
	int xMotions_;
	int yMotions_;
	int zMotions_;
	int uMotions_;
	int vMotions_;
	int wMotions_;
};

#endif // AMPIC887CONTROLLER_H
