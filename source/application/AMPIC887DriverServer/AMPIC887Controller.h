#ifndef AMPIC887CONTROLLER_H
#define AMPIC887CONTROLLER_H

#define CONTROLLER_PORT 50000			// PI C887.11 controller are hardwired to use port 50000
#define AXIS_POSITION_TOLERANCE 0.0008	// The accuracy with which the position of an axis can be known (mm)

#include <QObject>
#include <QString>
#include <QTimer>
#include <QHash>
#include <QVector>
#include "GCS2Commands/AMGCS2Command.h"
#include "GCS2Commands/AMGCS2AsyncCommand.h"
#include "AMPIC887AxisCollection.h"
#include "AMPIC887AxisMap.h"
#include "AMPIC887HexapodPosition.h"

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
	  * The last error encountered by the controller.
	  */
	QString lastError() const;

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
	  * Whether this controller is in a valid state to accept commands
	  */
	bool isInValidState() const;

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
	  * Whether the data recorder is set to retrieve the data after a motion.
	  */
	bool isDataRecorderActive() const;

	/*!
	  * Sets whether the data recorder is currently set to retrieve data after a
	  * motion.
	  * \param isActive ~ The new active state of the data recorder.
	  */
	void setDataRecorderActive(bool isActive);

	/*!
	  * The record config of the data table with the provided table Id.
	  * \param tableId ~ The id of the table whose data recorder configuration
	  * will be returned.
	  */
	AMPIC887DataRecorderConfiguration recordConfig(int tableId) const;

	/*!
	  * Sets the record config of the data table with the provided Id.
	  * \param tableId ~ The id of the table whose data recorder configuration
	  * will be set.
	  * \param recordConfig ~ The value to which the table's record config will
	  * be set.
	  * \returns True if the record config could be set, false otherwise.
	  */
	bool setRecordConfig(int tableId,
						 const AMPIC887DataRecorderConfiguration& recordConfig);

	/*!
	  * The record configs stored for each data table within the controller.
	  */
	QHash<int, AMPIC887DataRecorderConfiguration> recordConfigs() const;

	/*!
	  * Sets the record configs for all the data tables whose id is a key in
	  * the provided mapping, to the data record config to which it maps.
	  * \param recordConfigs ~ A mapping of data table id to the record config
	  * to which it will be set.
	  */
	bool setRecordConfigs(const QHash<int, AMPIC887DataRecorderConfiguration>& recordConfigs);

	/*!
	  * The recorded positional data for all axes during the last movement. This
	  * vector is of a constant size of 1024, but may not be entirely filled with
	  * each move.
	  */
	const QVector<AMPIC887HexapodPosition>& lastRecordedPositionData() const;

	/*!
	  * The current movement status of the controller's hexapod.
	  */
	AMGCS2::AxisMovementStatuses movementStatuses() const;

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
	  * Sets the controller's current command level.
	  * \param commandLevel ~ The command level to set the controller to.
	  * \param password ~ An optional password, which need only be provided if
	  * setting the command level to advanced.
	  */
	bool setCommandLevel(AMGCS2::ControllerCommandLevel commandLevel,
						 const QString& password = QString());

	/*!
	  * The current position of the provided axis.
	  * \returns The current position of axis if the controller has been initialized,
	  * 0 otherwise.
	  */
	double currentPosition(AMGCS2::Axis axis);

	/*!
	  * The current positions of the provided axes.
	  * \param axes ~ The collection of axes whose current position is to be
	  * returned.
	  */
	AMPIC887AxisMap<double> currentPositions(const AMPIC887AxisCollection& axes);

	/*!
	  * The target position of the provided axis.
	  * \returns The target position of the axis if the controller has been initialized,
	  * 0 otherwise.
	  */
	double targetPosition(AMGCS2::Axis axis);

	/*!
	  * The target positions of the provided axes.
	  * \param axes ~ A collection of the axes whose target positions are to be
	  * returned.
	  */
	AMPIC887AxisMap<double> targetPositions(const AMPIC887AxisCollection& axes);

	/*!
	  * The cycle time of the controller.
	  * \returns The controller's cycle time if the controller has been initialized,
	  * 0 otherwise.
	  */
	double cycleTime() const;

	/*!
	  * Sets the cycle time for the controller.
	  * \param cycleTime ~ The new cycle time for the controller.
	  */
	bool setCycleTime(double cycleTime);

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
	  * Sets the low soft limit of the provided axis to the provided value.
	  * \param axis ~ The axis to set the low soft limit of.
	  * \param lowSoftLimit ~ The low soft limit to set the axis to.
	  */
	bool setLowSoftLimit(AMGCS2::Axis axis, double lowSoftLimit);

	/*!
	  * The current low soft limits of the provided axes.
	  * \param axes ~ The colleciton of axes whose low soft limits is to be
	  * returned.
	  */
	AMPIC887AxisMap<double> lowSoftLimits(const AMPIC887AxisCollection& axes) const;

	/*!
	  * Sets the low soft limits of the axes contained within the provided map
	  * to their mapped values.
	  * \param lowSoftLimits ~ The map of axis values which will be altered.
	  */
	bool setLowSoftLimits(const AMPIC887AxisMap<double>& lowSoftLimits);

	/*!
	  * The high soft limit of the provided axis.
	  * \returns The high soft limit of the provided axis if the controller has
	  * been initialized and the axis is not the UnknownAxis, 0 otherwise.
	  */
	double highSoftLimit(AMGCS2::Axis axis) const;

	/*!
	  * Sets the high soft limit of the provided axis to the provided value.
	  * \param axis ~ The axis whose low soft limit will be set.
	  * \param highSoftLimit ~ The value which the axis' high soft limit will be
	  * set to.
	  */
	bool setHighSoftLimit(AMGCS2::Axis axis, double highSoftLimit);

	/*!
	  * The current high soft limits of the provided axes.
	  * \param axes ~ The colleciton of axes whose high soft limits is to be
	  * returned.
	  */
	AMPIC887AxisMap<double> highSoftLimits(const AMPIC887AxisCollection& axes) const;

	/*!
	  * Sets the high soft limits based on the values in the provided map.
	  * \param highSoftLimits ~ The map of axes values which will be altered.
	  */
	bool setHighSoftLimits(const AMPIC887AxisMap<double>& highSoftLimits);

	/*!
	  * Whether the soft limits are active for the provided axis.
	  * \returns The soft limit status of the provided axis if the controller has
	  * been initialized and the axis is not the UnknownAxis, false otherwise.
	  */
	bool softLimitStatus(AMGCS2::Axis axis) const;

	/*!
	  * Sets the active state of the soft limit of the provided axis.
	  * \param axis ~ The axis whose soft limit state will be altered.
	  * \param softLimitState ~ The state to set the axis' soft limit to.
	  */
	bool setSoftLimitStatus(AMGCS2::Axis axis, bool softLimitState);

	/*!
	  * The current soft limit statuses for the provided axes.
	  * \param axes ~ The collection of axes whose soft limit statuses are to be
	  * returned.
	  */
	AMPIC887AxisMap<bool> softLimitStatuses(const AMPIC887AxisCollection& axes) const;

	/*!
	  * Sets the soft limit states of the axes based on the provided map of values.
	  * \param softLimitStates ~ A map of active values for the soft limit states
	  * to set.
	  */
	bool setSoftLimitStatuses(const AMPIC887AxisMap<bool>& softLimitStates);

	/*!
	  * Whether the limit switch is active for the provided axis.
	  * \param axis ~ The axis whose limit switch status is to be returned.
	  */
	bool limitSwitchStatus(AMGCS2::Axis axis) const;

	/*!
	  * The current limit switch statuses for the provided collection of axes.
	  * \param axes ~ The collection of axes whose soft limit statuses are to
	  * be returned.
	  */
	AMPIC887AxisMap<bool> limitSwitchStatuses(const AMPIC887AxisCollection& axes) const;

	/*!
	  * The minimum commandable position of the provided axis
	  * \returns The minimum commandable position of the provided axis if the controller
	  * has been initialized and the axis is not the UnknownAxis, 0 otherwise.
	  */
	double minCommandablePosition(AMGCS2::Axis axis) const;

	/*!
	  * The minimum commnandable positions of the provided axes.
	  * \param axes ~ The collection of axes whose min commandable positions are
	  * to be returned.
	  */
	AMPIC887AxisMap<double> minCommandablePositions(const AMPIC887AxisCollection& axes) const;

	/*!
	  * The maximum commandable position of the provided axis
	  * \returns The maximum commandable position of the provided axis if the controller
	  * has been initialized and the axis is not the UnknownAxis, 0 otherwise.
	  */
	double maxCommandablePosition(AMGCS2::Axis axis) const;

	/*!
	  * The maximum commandable position of the provided axes.
	  * \param axes ~ The collection of axes whose max commandable positions are
	  * to be returned.
	  */
	AMPIC887AxisMap<double> maxCommandablePositions(const AMPIC887AxisCollection& axes) const;

	/*!
	  * Whether the provided axis is currently on target (i.e. it's current position
	  * is equal to its target position).
	  * \returns The on target state of the provided axis if the controller has
	  * been initialized and the axis is not the UnknownAxis, false otherwise.
	  */
	bool onTargetState(AMGCS2::Axis axis);

	/*!
	  * The on target states of the provied axes.
	  * \param axes ~ The collection of axes whose on target states are to be
	  * returned.
	  */
	AMPIC887AxisMap<bool> onTargetStates(const AMPIC887AxisCollection& axes);

	/*!
	  * The pivot point of the provided axis.
	  * \returns The pivot point of the provided axis if the controller has been
	  * initialized and the axis is X, Y or Z; 0 otherwise.
	  */
	double pivotPoint(AMGCS2::Axis axis) const;

	/*!
	  * Sets the value of the provided axis' pivot point.
	  * \param axis ~ The axis whose pivot point value will be set.
	  * \param pivotPoint ~ The value to which the axis' pivot point will be set.
	  */
	bool setPivotPoint(AMGCS2::Axis axis, double pivotPoint);

	/*!
	  * The pivot points of the provided axes.
	  * \param The collection of axes whose pivot points are to be returned.
	  */
	AMPIC887AxisMap<double> pivotPoints(const AMPIC887AxisCollection& axes) const;

	/*!
	  * Sets the pivot points of the axes based on the provided map.
	  * \param pivotPoints ~ A map of values for the axes whose pivot points will
	  * be altered.
	  */
	bool setPivotPoints(const AMPIC887AxisMap<double>& pivotPoints);

	/*!
	  * The position units of the provided axis.
	  * \returns The position units of the provided axis if the controller has been
	  * initialized and the axis is not the UnknownAxis, UnknownPositionUnits otherwise.
	  */
	AMGCS2::PositionUnits positionUnits(AMGCS2::Axis axis) const;

	/*!
	  * The position units of the provided axes.
	  * \param axes ~ The axes whose position units are to be returned.
	  */
	AMPIC887AxisMap<AMGCS2::PositionUnits> positionUnits(const AMPIC887AxisCollection& axes) const;

	/*!
	  * The positional data recorded for the hexapod during the last move.
	  */
	QList<double> recordedData(int offset, int numberOfDataPoints, int tableId);

	/*!
	  * A string containing all the recorder options which the controller can have
	  * \returns The controller record options in text format if the controller
	  * has been initialized, the empty string otherwise.
	  */
	QString recorderOptionsString() const;

	/*!
	  * The current record rate of the controller (in hertz).
	  */
	double recordRate() const;

	/*!
	  * Sets the record rate of the controller to the provided value (in hertz).
	  * \param recordRate ~ The record rate of the controller to set (in hertz)
	  */
	bool setRecordRate(double recordRate);

	/*!
	  * The current trigger event which beings a recording in the controller's
	  * data tables.
	  * \returns The controller's data record trigger if the controller has been
	  * initialized, UnknownRecordTrigger otherwise.
	  */
	AMGCS2::DataRecordTrigger recordTrigger() const;

	/*!
	  * Sets the trigger which will begin a record on the controller.
	  * \param recordTrigger ~ The new record trigger for the controller.
	  */
	bool setRecordTrigger(AMGCS2::DataRecordTrigger recordTrigger);

	/*!
	  * Whether the provided axis has has a successful reference move performed.
	  * \param axis ~ The axis whose referenced status is to be returned.
	  * \returns The referenced status of the provided axis if the controller has
	  * been initialized and the axis is not the UnknownAxis, false otherwise.
	  */
	bool isAxisReferenced(AMGCS2::Axis axis) const;

	/*!
	  * The referenced states of the provided axes.
	  * \param axes ~ The axes whose referenced states are to be returned.
	  */
	AMPIC887AxisMap<bool> axisReferencedStates(const AMPIC887AxisCollection& axes) const;

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
	  * Sets the servo mode state for the controller.
	  * \param servoMode ~ The servo mode for the controller. True indicates the
	  * controller will be placed in servo mode (closed loop).
	  */
	bool setServoMode(bool servoMode);

	/*!
	  * The step size of the provided axis.
	  * \param axis ~ The axis whose step size is to be returned.
	  * \returns The step size of the provided axis if the controller has been
	  * initialized and the axis is not the UnknownAxis, 0 otherwise.
	  */
	double stepSize(AMGCS2::Axis axis);

	/*!
	  * Sets the step size of the provided axis to the provided value.
	  * \param axis ~ The axis whose step size will be set.
	  * \param stepSize ~ The value to which the axis' step size will be set.
	  */
	bool setStepSize(AMGCS2::Axis axis, double stepSize);

	/*!
	  * The step sizes of the provided axes.
	  * \param axes ~ The collection of axes whose steps sizes are to be returned.
	  */
	AMPIC887AxisMap<double> stepSizes(const AMPIC887AxisCollection& axes) const;

	/*!
	  * Sets the values of the axes' step sizes based on the provided mapped
	  * values.
	  * \param stepSizes ~ A map containing the values to which the axis' step
	  * sizes will be set.
	  */
	bool setStepSizes(const AMPIC887AxisMap<double>& stepSizes);

	/*!
	  * The current velocity of motions across all axes of the controller's hexapod.
	  * \returns The system velocity of the controller if the controller has been
	  * initialized, 0 otherwise.
	  */
	double systemVelocity() const;

	/*!
	  * Sets the system velocity of the controller's motions.
	  * \param systemVelocity ~ The controller's new system velocity.
	  */
	bool setSystemVelocity(double systemVelocity);

public slots:

	/*!
	  * Instructs the controller to make an immediate stop.
	  */
	bool stop();

	/*!
	  * Instructs the controller to bring the motions of the provided axes to
	  * a halt smoothly.
	  * \param axes ~ The axes whose motions are to be halted.
	  */
	bool haltSmoothly(const AMPIC887AxisCollection& axes);

	/*!
	  * Moves the controller hexapod axes to the provided positions.
	  * \param axisPositions ~ A mapping of axis to its new target position.
	  */
	void move(const AMPIC887AxisMap<double>& axisPositions);

	/*!
	  * Performs a relative move on the controller hexapod axes, setting their
	  * target positions to the current target positions plus the values found
	  * in the provided hash.
	  * \param relativePositions ~ A mapping of axis to the values to add to
	  * the current target positions to provide the new target positions.
	  */
	void moveRelative(const AMPIC887AxisMap<double>& relativePositions);

	/*!
	  * Performs a reference move upon the provided list of axes.
	  * \param axes ~ An optional list of the axes to reference. If none is provided
	  * a reference move will be performed on all axes.
	  */
	void referenceMove(const AMPIC887AxisCollection& axes = AMPIC887AxisCollection());

	/*!
	  * Initializes the controller state data by running a command to obtain the
	  * data from the controller.
	  */
	void initializeControllerStateData();

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

	/*!
	  * Signal indicating that the controller's hexapod has begun moving.
	  */
	void moveStarted(AMGCS2::AxisMovementStatuses movemenStatuses);

	/*!
	  * Signal indicating that the controller's hexapod has completed a movement.
	  */
	void moveComplete();

	/*!
	  * Signal indicating that the controller's hexapod has failed a movement.
	  */
	void moveFailed(AMGCS2::AxisMovementStatuses movementStatuses);

	/*!
	  * Signal indicating that the position of the axes of the controller have
	  * updated. Emitted periodically during a move operation.
	  */
	void positionUpdate(const AMPIC887AxisMap<double>& newPositions);

	/*!
	  * Signal indicating that the system velocity of the controller has been
	  * altered.
	  * \param newSystemVelocity ~ The new value to which the system velocity
	  * has been set.
	  */
	void systemVelocityChanged(double newSystemVelocity);

	/*!
	  * Signal indicating that the target positions of the controller have been
	  * altered.
	  * \param newTarget ~ The new target positions of all the axes of the hexapod.
	  */
	void targetPositionChanged(const AMPIC887AxisMap<double>& newTarget);

	/*!
	  * Signal indicating that the record rate of the controller's data recorder
	  * has been altered.
	  * \param recordRate ~ The new record rate (in Hz) of the controller's data
	  * recorder.
	  */
	void recordRateChanged(double recordRate);

	/*!
	  * Signal indicating that the active state of the data recorder has been
	  * altered.
	  * \param isActive ~ The new active state of the data recorder.
	  */
	void dataRecorderActiveStateChanged(bool isActive);
protected slots:

	/*!
	  * Handles signals from the timer indicating that a position update signal
	  * needs to be emitted.
	  */
	void onPositionUpdateTimerInterval();

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
	  * Handles signals indcating that an asynchronous data retrieval has succeeded.
	  */
	void onAsyncDataRetrievalSucceeded(AMGCS2AsyncCommand*);

	/*!
	  * Handles signals indcating that an asynchronous data retrieval has failed.
	  */
	void onAsyncDataRetrievalFailed(AMGCS2AsyncCommand*);
	/*!
	  * Handles the error clearning timer indicating that its delay time for the
	  * error message to be cleared is complete.
	  */
	void onErrorClearingTimerTimedOut();

	/*!
	  * Method which starts asynchronous data retrieval from the data recorder.
	  */
	void startAsyncDataRetrieval();
protected:

	/*!
	  * Runs the provided command.
	  * \param command ~ The command to run.
	  */
	void runCommand(AMGCS2Command* command);

	/*!
	  * Refreshes the current position data within the controller state cache.
	  */
	void refreshCurrentPositions();

	/*!
	  * Updates the state of the controller in the case that a stop has taken
	  * place.
	  * Refreshes the current position.
	  * Sets the target position to the current position.
	  * (NOTE: Updating the motion counts for the axes is handled by the fact that
	  * any async commands which are currently running will fail when a stop occurs).
	  */
	void updateStateOnStop();

	/*!
	  * Convenience method for setting the last error message and emitting the
	  * errorEncountered signal at the same time.
	  */
	void setError(const QString& errorMessage);

	/*!
	  * Helper function which calculates the required delay, in ms, after a move
	  * from the current position to the target position at the current
	  * system velocity before the data retrieval should commence.
	  */
	int calculateDataRetrievalDelay();

protected:
	AMPIC887ControllerState* controllerState_;
	QString name_;
	QString hostname_;
	QTimer errorClearingTimer_;
	bool isBusy_;
	int id_;
	QString lastError_;
	QTimer positionUpdateTimer_;
	mutable bool currentPositionRefreshRequired_;
	QVector<AMPIC887HexapodPosition> lastRecordedPositionData_;
	bool isDataRecorderActive_;
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
