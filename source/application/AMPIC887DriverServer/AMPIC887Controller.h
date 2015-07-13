#ifndef AMPIC887CONTROLLER_H
#define AMPIC887CONTROLLER_H

#define CONTROLLER_PORT 50000 // PI C887.11 controller are hardwired to use port 50000

#include <QObject>
#include <QString>
#include <QTimer>
#include <QHash>
#include "GCS2Commands/AMGCS2Command.h"
#include "GCS2Commands/AMGCS2AsyncCommand.h"
#include "AMPIC887AxisCollection.h"
#include "AMPIC887AxisMap.h"

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
	  * Whether the data has been initialized for this controller. That is, a
	  * connection has been made and the starting state of the controller loaded.
	  */
	bool isInitialized() const;

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

	/*!
	  * Whether any axis of the hexapod is currently in motion.
	  */
	bool isMoving() const;

	/*!
	  * A string containing all the parameters which can be used to alter the
	  * controller's behaviour.
	  */
	QString availableParametersString() const;

	/*!
	  * The current command level of the controller.
	  * \returns The command level which the controller is in if the controller
	  * has been initialized, UnknownCommandLevel otherwise.
	  */
	AMGCS2::ControllerCommandLevel commandLevel() const;

	/*!
	  * The current position of the provided axis.
	  * \returns The current position of axis if the controller has been initialized,
	  * 0 otherwise.
	  */
	double currentPosition(AMGCS2::Axis axis);

	/*!
	  * The target position of the provided axis.
	  * \returns The target position of the axis if the controller has been initialized,
	  * 0 otherwise.
	  */
	double targetPosition(AMGCS2::Axis axis);

	AMPIC887AxisMap<double> targetPositions();

	/*!
	  * The cycle time of the controller.
	  * \returns The controller's cycle time if the controller has been initialized,
	  * 0 otherwise.
	  */
	double cycleTime() const;

	/*!
	  * The record option of the data table with the provided id.
	  * \returns The data table's record option if the controller has been initialized
	  * and the tableId is valid, UnknownRecordOption otherwise.
	  */
	AMGCS2::DataRecordOption recordOption(int tableId) const;

	/*!
	  * The record source of the data table with the provided id.
	  * \returns The data table's record source if the controller has been initialized
	  * abd the tableId is valid, UnknownRecordSource otherwise.
	  */
	AMGCS2::DataRecordSource recordSource(int tableId) const;

	/*!
	  * The serial number and make information of the controller.
	  */
	QString identificationString() const;

	/*!
	  * The low soft limit of the provided axis.
	  * \returns The low soft limit of the provided axis if the controller has been
	  * initialized and the axis is not the UnknownAxis, 0 otherwise.
	  */
	double lowSoftLimit(AMGCS2::Axis axis) const;

	/*!
	  * The high soft limit of the provided axis.
	  * \returns The high soft limit of the provided axis if the controller has
	  * been initialized and the axis is not the UnknownAxis, 0 otherwise.
	  */
	double highSoftLimit(AMGCS2::Axis axis) const;

	/*!
	  * Whether the soft limits are active for the provided axis.
	  * \returns The soft limit status of the provided axis if the controller has
	  * been initialized and the axis is not the UnknownAxis, false otherwise.
	  */
	bool softLimitStatus(AMGCS2::Axis axis) const;

	/*!
	  * The minimum commandable position of the provided axis
	  * \returns The minimum commandable position of the provided axis if the controller
	  * has been initialized and the axis is not the UnknownAxis, 0 otherwise.
	  */
	double minCommandablePosition(AMGCS2::Axis axis) const;

	/*!
	  * The maximum commandable position of the provided axis
	  * \returns The maximum commandable position of the provided axis if the controller
	  * has been initialized and the axis is not the UnknownAxis, 0 otherwise.
	  */
	double maxCommandablePosition(AMGCS2::Axis axis) const;

	/*!
	  * Whether the provided axis is currently on target (i.e. it's current position
	  * is equal to its target position).
	  * \returns The on target state of the provided axis if the controller has
	  * been initialized and the axis is not the UnknownAxis, false otherwise.
	  */
	bool onTargetState(AMGCS2::Axis axis);

	/*!
	  * The pivot point of the provided axis.
	  * \returns The pivot point of the provided axis if the controller has been
	  * initialized and the axis is X, Y or Z; 0 otherwise.
	  */
	double pivotPoint(AMGCS2::Axis axis) const;

	/*!
	  * The position units of the provided axis.
	  * \returns The position units of the provided axis if the controller has been
	  * initialized and the axis is not the UnknownAxis, UnknownPositionUnits otherwise.
	  */
	AMGCS2::PositionUnits positionUnits(AMGCS2::Axis axis) const;

	/*!
	  * The data recorded in the data table with the provided tableId during the
	  * last recording instance.
	  * \param tableId ~ The table from which to return the data.
	  * \returns The data stored in the data table with the provided id, if the
	  * controller has been initialized and the provided tableId is valid, an
	  * empty list otherwise.
	  */
	QList<int> recordedData(int tableId) const;

	/*!
	  * A string containing all the recorder options which the controller can have
	  * \returns The controller record options in text format if the controller
	  * has been initialized, the empty string otherwise.
	  */
	QString recorderOptionsString() const;

	/*!
	  * The current trigger event which beings a recording in the controller's
	  * data tables.
	  * \returns The controller's data record trigger if the controller has been
	  * initialized, UnknownRecordTrigger otherwise.
	  */
	AMGCS2::DataRecordTrigger recordTrigger() const;

	/*!
	  * Whether the provided axis has has a successful reference move performed.
	  * \param axis ~ The axis whose referenced status is to be returned.
	  * \returns The referenced status of the provided axis if the controller has
	  * been initialized and the axis is not the UnknownAxis, false otherwise.
	  */
	bool isAxisReferenced(AMGCS2::Axis axis) const;

	/*!
	  * Whether a reference move has been performed across all axes.
	  * \returns True if all axes of the controller have been referenced and the
	  * controller has been initialized, false otherwise.
	  */
	bool areAxesReferenced() const;

	/*!
	  * Whether the controller's motions are performed in servo (closed loop) mode
	  * \returns True if the controller is in servo mode and the controller has
	  * been initialized, false otherwise.
	  */
	bool isInServoMode() const;

	/*!
	  * The step size of the provided axis.
	  * \param axis ~ The axis whose step size is to be returned.
	  * \returns The step size of the provided axis if the controller has been
	  * initialized and the axis is not the UnknownAxis, 0 otherwise.
	  */
	double stepSize(AMGCS2::Axis axis);

	/*!
	  * The current velocity of motions across all axes of the controller's hexapod.
	  * \returns The system velocity of the controller if the controller has been
	  * initialized, 0 otherwise.
	  */
	double systemVelocity() const;

public slots:
	// Command Functions
	/////////////////////

	/*!
	  * Instructs the controller to make an immediate stop.
	  */
	void stop();

	/*!
	  * Instructs the controller to bring the motions of the axes to a stop
	  * in a smooth motion.
	  */
	void haltSmoothly();

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

	mutable bool currentPositionRefreshRequired_;
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
