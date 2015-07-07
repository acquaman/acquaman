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
	  * Instructs the controller to begin a motion asynchronously.
	  * \param axisPositions ~ A mapping of axes to target positions.
	  */
	void move(const QHash<AMGCS2::Axis, double>& axisPositions);

	/*!
	  * Instructs the controller to begin a reference move action on the provided
	  * axes.
	  * \param axes ~ An optional list of the axes to perform a reference move upon.
	  * If no axes are provided a reference move is performed on all axes.
	  */
	void referenceMove(const QList<AMGCS2::Axis>& axes = QList<AMGCS2::Axis>());

	/*!
	  * Moves the axes relative to their current positions by the provided amounts.
	  * \param axisRelativePositions ~ A mapping of axes to the amount they are
	  * to move, relative to their targetPosition.
	  */
	void moveRelative(const QHash<AMGCS2::Axis, double>& axisRelativePositions);

	/*!
	  * The target position of the provided axis. If the axis is not moving, the
	  * targetPosition will equal the currentPosition.
	  * \param axis ~ The axis whose target position is to be returned.
	  */
	double targetPosition(AMGCS2::Axis axis) const;

	/*!
	  * The current position of the provided axis.
	  * \param axis ~ The axis whose current position is to be returned.
	  * \returns The current position of the provided axis, or 0 if the UnknownAxis
	  * is provided.
	  */
	double currentPosition(AMGCS2::Axis axis) const;

	/*!
	  * The cycle time used to sample motions for the controller.
	  */
	double cycleTime() const;

	/*!
	  * Sets the controller's cycle time.
	  * \param cycleTime ~ The controller's new cycle time.
	  */
	void setCycleTime(double cycleTime);

	/*!
	  * The data recorders record trigger.
	  */
	AMGCS2::DataRecordTrigger recordTrigger() const;

	/*!
	  * Sets the type of trigger which is used to start recordings on the controller.
	  * \param trigger ~ The new recording trigger for the controller.
	  */
	void setRecordTrigger(AMGCS2::DataRecordTrigger trigger);

	/*!
	  * The record source for the recorder table with the provided id.
	  * \param tableId ~ The id of the table whose record source is to be returned.
	  * \returns The record source for the provided table id if a valid table id
	  * is provided, UnknownDataRecordSource otherwise.
	  */
	AMGCS2::DataRecordSource recordSource(int tableId) const;

	/*!
	  * Sets the record source for record table with the provided id.
	  * \param tableId ~ The id of the table whose record source is to be altered.
	  * \param source ~ The new record source to set for the table with the provided
	  * tableId.
	  */
	void setRecordSource(int tableId, AMGCS2::DataRecordSource source);

	/*!
	  * The record option (i.e. what data is being recorded) for the recorder table
	  * with the provided id.
	  * \param tableId ~ The id of the table whose record option is to be returned.
	  * \returns The record option for the provided table id if a valid table id
	  * is provided, UnknownDataRecordOption otherwise.
	  */
	AMGCS2::DataRecordOption recordOption(int tableId) const;

	/*!
	  * Sets the record option (i.e. what type of data is to be recorded) for the
	  * recorder table with the provided id.
	  * \param tableId ~ The id of the table whose record option i sto be altered.
	  * \param option ~ The new record option to set for the table with the provided
	  * id.
	  */
	void setRecordOption(int tableId, AMGCS2::DataRecordOption option);

	/*!
	  * The low soft limit of the provided axis.
	  * \param axis ~ The axis whose low soft limit is to be returned
	  * \returns The value of the low soft limit of the provided axis, or 0 if
	  * an UnknownAxis is provided.
	  */
	double lowSoftLimit(AMGCS2::Axis axis) const;

	/*!
	  * Sets the low soft limit for the provided axis to the provided value.
	  * NOTE: This limit will not take effect unless the soft limits for this axis
	  * are active (see areSoftLimitsActive()).
	  * \param axis ~ The axis whose low soft limit is to be altered.
	  * \param lowSoftLimit ~ The new low soft limit to set for the provided axis.
	  */
	void setLowSoftLimit(AMGCS2::Axis axis, double lowSoftLimit);

	/*!
	  * The high soft limit of the provided axis.
	  * \param axis ~ The axis whose high soft limit is to be returned.
	  * \returns The value of the high soft limit of the provided axis, or 0 if
	  * an UnknownAxis is provided.
	  */
	double highSoftLimit(AMGCS2::Axis axis) const;

	/*!
	  * Sets the high soft limit for the provided axis to the provided value.
	  * NOTE: This limit will not take effect unless the soft limits for this axis
	  * are active (see areSoftLimitsActive()).
	  * \param axis ~ The axis whose high soft limit is to be altered.
	  * \param highSoftLimit ~ The new high soft limit to set for the provided axis.
	  */
	void setHighSoftLimit(AMGCS2::Axis axis, double highSoftLimit);

	/*!
	  * Whether the soft limits are active for the provided axis.
	  * \param axis ~ The axis whose soft limit active status will be returned.
	  * \returns True if the soft limits are active for the provided axis and
	  * a known axis is provided, false otherwise.
	  */
	bool areSoftLimitsActive(AMGCS2::Axis axis) const;

	/*!
	  * Sets the active status of the high and low soft limits of the provided axis.
	  * \param axis ~ The axis whose soft limits are to be altered.
	  * \param active ~ The state to set the axis' soft limits to.
	  */
	void setSoftLimitsActive(AMGCS2::Axis axis, bool active);

	/*!
	  * The minimum commandable position that the provided axis can take.
	  * \param axis ~ The axis whose min commandable position is to be returned.
	  * \returns The minimum commandable position of the provided axis, or 0 if
	  * the UnknownAxis is provided.
	  */
	double minCommandablePosition(AMGCS2::Axis axis) const;

	/*!
	  * The maximum commandable position that the provided axis can take.
	  * \param axis ~ The axis whose max commandable position is to be returned.
	  * \returns The maximum commandable position of the provided axis, or 0 if
	  * the UnknownAxis is provided.
	  */
	double maxCommandablePosition(AMGCS2::Axis axis) const;

	/*!
	  * A set of flags indicating the movement statuses of all the axes of the
	  * controller.
	  */
	QFlags<AMGCS2::AxisMovementStatus> movingStatuses() const;

	/*!
	  * The pivot point of the provided linear axis.
	  * \param axis ~ The linear axis whose pivot point is to be returned.
	  * \returns The pivot point of the provided axis if it is a linear axis (X,
	  * Y or Z), 0 otherwise.
	  */
	double pivotPoint(AMGCS2::Axis axis) const;

	/*!
	  * Sets the pivot point for the provided axis to the provided value.
	  * NOTE1: Only valid for the linear (X, Y or Z) axes.
	  * NOTE2: Can only be performed when all rotational axes are at position 0.
	  * \param axis ~ The axis whose pivot point is to be altered.
	  * \param pivotPoint ~ The new pivot point position for the axes.
	  */
	void setPivotPoint(AMGCS2::Axis axis, double pivotPoint) const;

	/*!
	  * The data stored in the data recorder table with the provided id.
	  * \param tableId ~ The id of the data recorder table whose data is to be
	  * returned.
	  * \param offset ~ Optional offset to retrieve data in the table starting from.
	  * If none is provided data from the start of the table (0) is returned.
	  * \param count ~ Optional number of data entries to retrieve from the table.
	  * If none is provided all the data obtained in the last record event is returned.
	  */
	QList<int> recordedData(int tableId, int offset = 0, int count = -1);

	/*!
	  * Whether the provided axis has successfully had a reference move performed.
	  * \param axis ~ The axis whose reference status is to be returned.
	  * \returns True if the provided axis has had a reference move successfully
	  * performed and is a known axis, false otherwise.
	  */
	bool isAxisReferenced(AMGCS2::Axis axis) const;

	/*!
	  * Whether all axes of the controller's hexapod have successfully had a reference
	  * move performed.
	  */
	bool areAllAxesReferenced() const;

	/*!
	  * Whether the controller's hexapod is currently in servo (closed loop) mode.
	  */
	bool isInServoMode() const;

	/*!
	  * Sets whether the axes are in servo mode (closed loop) or not.
	  * \param servoModeActive ~ The state to set the controller's servo mode to.
	  */
	void setServoMode(bool servoModeActive);

	/*!
	  * The step size of the controller's hexapod motions.
	  */
	double stepSize(AMGCS2::Axis axis) const;

	/*!
	  * The current velocity of motions for the controller's hexapod.
	  */
	double systemVelocity() const;

	/*!
	  * Stops all the controller's hexapod's motions immediately.
	  */
	void stop() const;

	/*!
	  * Halts the controller's hexapod's motions smoothly according to it's motion
	  * profile.
	  */
	void haltSmoothly() const;

	/*!
	  * Whether the provided movement is safe to perform given the current settings
	  * of the controller.
	  */
	bool isMoveSafe(const QHash<AMGCS2::Axis, double>& axisPositions) const;
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

	/*!
	  * Signal indicating that the controller has begun a move operation.
	  */
	void moveStarted();

	/*!
	  * Signal indicating that the controller has successfully completed a move
	  * operation.
	  */
	void moveSucceeded();

	/*!
	  * Signal indicating that the controller has failed a move operation.
	  */
	void moveFailed();

	/*!
	  * Signal indicating that the controller has begun a reference move operation.
	  */
	void referenceMoveStarted();

	/*!
	  * Signal indicating that the controller has successfully completed a reference
	  * move operation.
	  */
	void referenceMoveSucceeded();

	/*!
	  * Signal indicating that the controller has failed a reference move operation.
	  */
	void referenceMoveFailed();
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
};

#endif // AMPIC887CONTROLLER_H
