#ifndef BIOXASSSRLMONOCHROMATORREGIONCONTROL_H
#define BIOXASSSRLMONOCHROMATORREGIONCONTROL_H

#include "beamline/AMCompositeControl.h"
#include "beamline/AMPVControl.h"
#include "actions3/AMActionSupport.h"
#include "actions3/actions/AMControlWaitAction.h"
#include "actions3/AMListAction3.h"

// {setpoint}_{motor}_{property} VALUE
#define SETPOINT_SLIT_CLOSED 0.0
#define SETPOINT_PADDLE_REMOVED -55.0
#define SETPOINT_BRAGG_MOTOR_CRYSTAL_CHANGE_POSITION 55.0
#define SETPOINT_BRAGG_MOTOR_REGION_A_DESTINATION -10.0
#define SETPOINT_BRAGG_MOTOR_REGION_B_DESTINATION 330.0
#define SETPOINT_CRYSTAL_CHANGE_MOTOR_REGION_A_DESTINATION 15000.0
#define SETPOINT_CRYSTAL_CHANGE_MOTOR_REGION_B_DESTINATION -15000.0

// {timeout}_{component}_{condition} VALUE
#define TIMEOUT_SLITS_CLOSED 20.0
#define TIMEOUT_PADDLE_OUT 70.0
#define TIMEOUT_KEY_STATUS_CHANGE 500.0
#define TIMEOUT_CRYSTAL_CHANGE_POSITION_REACHED 220.0
#define TIMEOUT_BRAKE_STATUS_CHANGE 500.0
#define TIMEOUT_CRYSTAL_CHANGE_MOTOR_LIMIT_REACHED 60
#define TIMEOUT_BRAGG_MOTOR_LIMIT_REACHED 200.0
#define TIMEOUT_REGION_STATE_CHANGED 200.0

class BioXASSSRLMonochromator;

class BioXASSSRLMonochromatorRegionControl : public AMCompositeControl
{
	Q_OBJECT

public:
	/// Enum marking possible different crystal change steps.
	enum Step { CloseSlits = 0, WaitForSlitsClosed, RemovePaddle, WaitForPaddleRemoved, WaitForKeyEnabled, MoveBraggIntoPosition, WaitForBraggInPosition, WaitForBrakeDisabled, MoveCrystalChangeIntoPosition, WaitForCrystalChangeInPosition, WaitForBrakeEnabled, MoveBraggIntoRegion, WaitForBraggInRegion, WaitForKeyDisabled, None };

	/// Constructor.
	explicit BioXASSSRLMonochromatorRegionControl(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromatorRegionControl();

	/// Returns the current region.
	virtual double value() const = 0;
	/// Returns the current region setpoint.
	virtual double setpoint() const = 0;
	/// Returns the minimum and maximum values this control can take.
	virtual QPair<double, double> range() const { return QPair<double, double>(minimumValue(), maximumValue()); }
	/// Returns None, the smallest value the region can take.
	virtual double minimumValue() const;
	/// Returns B, the largest value the region can take.
	virtual double maximumValue() const;

	/// Returns true if the region is always measurable (when the control is connected).
	virtual bool shouldMeasure() const { return true; }
	/// Returns true if a region measurement is possible now.
	virtual bool canMeasure() const;
	/// Returns true if a move to a new region is always possible, provided control is connected.
	virtual bool shouldMove() const { return true; }
	/// Returns true if a move to a new region is possible now.
	virtual bool canMove() const;
	/// Returns true if this control can stop a crystal change in progress, provided it is connected.
	virtual bool shouldStop() const { return false; }
	/// Returns true if this control can stop a change to a new region right now.
	virtual bool canStop() const { return false; }

	/// Returns true if there is a crystal change procedure in progress, as a result of this control's action.
	virtual bool moveInProgress() const { return moveInProgress_; }

signals:
	/// Notifier that there has been progress in completing a crystal change. Provides information suitable for a progress bar display.
	void moveProgressChanged(double numerator, double denominator);
	/// Notifier that the current step in a move has changed.
	void moveStepChanged(const QString &newDescription, const QString &newInstruction);

public slots:
	/// Sets the new region setpoint and performs a crystal change, if necessary.
	virtual FailureExplanation move(double setpoint) = 0;
	/// Stops a crystal change in progress.
	virtual bool stop() { return false; }

protected slots:
	/// Handles emitting the appropriate signals when the current step in a move has changed.
	void onCurrentMoveStepChanged(int stepIndex);
	/// Called when the internal crystal change action has been started. Handles updating the moveInProgress_ member variable and emitting the moveInProgress() signal.
	void onRegionChangeStarted();
	/// Called when the internal crystal change action has been cancelled. Handles emitting moveFailed(...) with the WasStoppedFailure code and deleting the action.
	void onRegionChangeCancelled();
	/// Called when the internal crystal change action has failed. Handles emitting moveFailed(...) with the OtherFailure code and deleting the action.
	void onRegionChangeFailed();
	/// Called when the internal crystal change action has succeeded! Handles emitting moveSucceeded() and deleting the action.
	void onRegionChangeSucceeded();

protected:
	/// Returns true if the given value is a valid region.
	static bool validRegion(int region);
	/// Returns the description associated with the given step index. The step index is the index of an action in the crystal change list action.
	static QString stepDescription(int stepIndex);
	/// Returns the instruction associated with the given step index, an empty string if there is none. The step index in the index of an action in the crystal change list action.
	static QString stepInstruction(int stepIndex);
	/// Handles the region change cleanup: making sure moveInProgress_ is updated, we disconnect from crystal change action signals, and the action is queued for deletion.
	void moveCleanup(QObject *action);

protected:
	/// Indicates whether the control is moving presently, as a result of this software.
	bool moveInProgress_;

};

#endif // BIOXASSSRLMONOCHROMATORREGIONCONTROL_H
