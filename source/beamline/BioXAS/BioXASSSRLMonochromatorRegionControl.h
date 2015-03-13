#ifndef BIOXASSSRLMONOCHROMATORREGIONCONTROL_H
#define BIOXASSSRLMONOCHROMATORREGIONCONTROL_H

#include "beamline/AMCompositeControl.h"
#include "beamline/AMPVControl.h"
#include "actions3/AMActionSupport.h"
#include "actions3/actions/AMControlWaitAction.h"
#include "actions3/actions/AMWaitAction.h"
#include "actions3/AMListAction3.h"
#include "beamline/CLS/CLSMAXvMotor.h"

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
#define TIMEOUT_CRYSTAL_CHANGE_MOVE_WAIT 1

class BioXASSSRLMonochromator;

class BioXASSSRLMonochromatorRegionControl : public AMCompositeControl
{
	Q_OBJECT

public:
	/// Enum marking possible different crystal change steps.
	enum Step { CloseSlits = 0, RemovePaddle, WaitForKeyEnabled, MoveBraggIntoPosition, WaitForBrakeDisabled, MoveCrystalChangeIntoPosition, WaitForBrakeEnabled, MoveBraggIntoRegion, WaitForKeyDisabled, None };

	/// Constructor.
	explicit BioXASSSRLMonochromatorRegionControl(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromatorRegionControl();

	/// Returns the current region.
	virtual double value() const { return value_; }
	/// Returns the current region setpoint.
	virtual double setpoint() const { return setpoint_; }
	/// Returns true if there is a crystal change procedure in progress, as a result of this control's action.
	virtual bool moveInProgress() const { return moveInProgress_; }
	/// Returns Region::None, the smallest value this control can assume.
	virtual double minimumValue() const;
	/// Returns Region::B, the largest value this control can assume.
	virtual double maximumValue() const;
	/// Returns true if the region is always measurable (when the control is connected).
	virtual bool shouldMeasure() const { return true; }
	/// Returns true if a move to a new region is always possible, provided control is connected.
	virtual bool shouldMove() const { return true; }
	/// Returns true if this control can stop a crystal change in progress, provided it is connected.
	virtual bool shouldStop() const { return false; }
	/// Returns true if this control can move right now.
	virtual bool canMove() const;
	/// Returns true if this control can stop a change to a new region right now.
	virtual bool canStop() const { return false; }

	/// Returns the upper slit control.
	virtual AMControl* upperSlitControl() const { return upperSlit_; }
	/// Returns the lower slit control.
	virtual AMControl* lowerSlitControl() const { return lowerSlit_; }
	/// Returns the slits status control.
	virtual AMControl* slitsStatusControl() const { return slitsStatus_; }
	/// Returns the paddle control.
	virtual AMControl* paddleControl() const { return paddle_; }
	/// Returns the paddle status control.
	virtual AMControl* paddleStatusControl() const { return paddleStatus_; }
	/// Returns the key status control.
	virtual AMControl* keyStatusControl() const { return keyStatus_; }
	/// Returns the brake status control.
	virtual AMControl* brakeStatusControl() const { return brakeStatus_; }
	/// Returns the bragg control.
	virtual AMControl* braggControl() const { return bragg_; }
	/// Returns the bragg motor at crystal change position status control.
	virtual AMControl* braggAtCrystalChangePositionStatusControl() const { return braggAtCrystalChangePositionStatus_; }
	/// Returns the crystal change control.
	virtual AMControl* crystalChangeControl() const { return crystalChange_; }
	/// Returns the region A status control.
	virtual AMControl* regionAStatusControl() const { return regionAStatus_; }
	/// Returns the region B status control.
	virtual AMControl* regionBStatusControl() const { return regionBStatus_; }

signals:
	/// Notifier that there has been progress in completing a crystal change. Provides information suitable for a progress bar display.
	void moveProgressChanged(double numerator, double denominator);
	/// Notifier that the current step in a move has changed.
	void moveStepChanged(const QString &newDescription, const QString &newInstruction);

	/// Notifier that the slits status has changed.
	void slitsStatusChanged(double status);
	/// Notifier that the paddle status has changed.
	void paddleStatusChanged(double status);
	/// Notifier that the key status has changed.
	void keyStatusChanged(double status);
	/// Notifier that the brake status has changed.
	void brakeStatusChanged(double status);
	/// Notifier that the bragg at crystal change position status has changed.
	void braggAtCrystalChangePositionStatusChanged(double status);
	/// Notifier that the region A status has changed.
	void regionAStatusChanged(double status);
	/// Notifier that the region B status has changed.
	void regionBStatusChanged(double status);

public slots:
	/// Sets the new region setpoint and performs a crystal change, if necessary.
	virtual FailureExplanation move(double setpoint);
	/// Stops a crystal change in progress.
	virtual bool stop() { return false; }

	// Sets the upper slit motor control.
	void setUpperSlitControl(AMControl *upperSlit);
	/// Sets the lower slit motor control.
	void setLowerSlitControl(AMControl *lowerSlit);
	/// Sets the slits status control.
	void setSlitsStatusControl(AMControl *slitsStatus);
	/// Sets the paddle motor control.
	void setPaddleControl(AMControl *paddle);
	/// Sets the paddle status control.
	void setPaddleStatusControl(AMControl *paddleStatus);
	/// Sets the key status control.
	void setKeyStatusControl(AMControl *keyStatus);
	/// Sets the brake status control.
	void setBrakeStatusControl(AMControl *brakeStatus);
	/// Sets the bragg motor control.
	void setBraggControl(AMControl *bragg);
	/// Sets the bragg at crystal change position status control.
	void setBraggAtCrystalChangePositionStatusControl(AMControl *inPosition);
	/// Sets the crystal change motor control.
	void setCrystalChangeControl(AMControl *crystalChange);
	/// Sets the crystal change cw limit status control.
	void setCrystalChangeCWLimitStatusControl(AMControl *limitStatus);
	/// Sets the crystal change ccw limit status control.
	void setCrystalChangeCCWLimitStatusControl(AMControl *limitStatus);
	/// Sets the region A status control.
	void setRegionAStatusControl(AMControl *regionStatus);
	/// Sets the region B status control.
	void setRegionBStatusControl(AMControl *regionStatus);

protected slots:
	/// Updates the current value_ and emits the valueChanged() signal.
	void setValue(double newValue);
	/// Handles emitting the appropriate signals when the current step in a move has changed.
	void onCurrentMoveStepChanged(int stepIndex);
	/// Handles updating the region value and emitting the value changed signal.
	void onRegionControlValueChanged();
	/// Called when the internal crystal change action has been started. Handles updating the moveInProgress_ member variable and emitting the moveInProgress() signal.
	void onRegionChangeStarted();
	/// Called when the internal crystal change action has been cancelled. Handles emitting moveFailed(...) with the WasStoppedFailure code and deleting the action.
	void onRegionChangeCancelled();
	/// Called when the internal crystal change action has failed. Handles emitting moveFailed(...) with the OtherFailure code and deleting the action.
	void onRegionChangeFailed();
	/// Called when the internal crystal change action has succeeded! Handles emitting moveSucceeded() and deleting the action.
	void onRegionChangeSucceeded();

protected:
	/// Returns a new action that performs a crystal change to change the region.
	AMListAction3* createChangeRegionAction(int newRegion);

	/// Returns a new action that closes the upper slit, 0 if not connected.
	AMAction3* createCloseUpperSlitAction();
	/// Returns a new action that closes the lower slit, 0 if not connected.
	AMAction3* createCloseLowerSlitAction();
	/// Returns a new action that waits for the slits status to change from 'not closed' to 'closed'.
	AMAction3* createWaitForSlitsClosedAction();
	/// Returns a new action that closes both slits, 0 if not connected.
	AMAction3* createCloseSlitsAction();

	/// Returns a new action that moves the paddle motor to the given destination.
	AMAction3* createMovePaddleAction(double destination);
	/// Returns a new action that waits for the paddle to be removed.
	AMAction3* createWaitForPaddleRemovedAction();
	/// Returns a new action that removes the paddle, 0 if not connected.
	AMAction3* createRemovePaddleAction();

	/// Returns a new action that waits for the key status to change to 'Enabled'.
	AMAction3* createWaitForKeyEnabledAction();

	/// Returns a new action that moves the bragg motor control to the given destination (degrees), 0 if not connected.
	AMAction3* createMoveBraggAction(double destination);
	/// Returns a new action that waits for the mono to signal it has reached the crystal change position.
	AMAction3* createWaitForBraggAtCrystalChangePositionAction();
	/// Returns a new action that sends the mono bragg motor to the change position.
	AMAction3* createMoveBraggToCrystalChangePositionAction();

	/// Returns a new action that waits for the mono brake to be disabled, 0 if not connected.
	AMAction3* createWaitForBrakeDisabledAction();

	/// Returns a new action that moves the crystal change motor to the given destination (relative move), 0 if not connected.
	AMAction3* createMoveCrystalChangeAction(double destination);
	/// Returns a new action that waits for the crystal change motor to reach its clockwise limit, 0 if not connected.
	AMAction3* createWaitForCrystalChangeAtCWLimitAction();
	/// Returns a new action that waits for the crystal change motor to reach its counter-clockwise limit, 0 if not connected.
	AMAction3* createWaitForCrystalChangeAtCCWLimitAction();
	/// Returns a new action that waits for the mono crystal change motor to reach a limit. Which particular limit (cw, ccw) depends on the region destination. Returns 0 if not connected, or if the destination is Region::None.
	AMAction3* createWaitForCrystalChangeAtRegionLimitAction(int region);
	/// Returns a new action that moves the crystal change motor to the appropriate limit for the given region destination, 0 if not connected or if the destination is Region::None.
	AMAction3* createMoveCrystalChangeToRegionLimitAction(int region);

	/// Returns a new action that waits for the mono brake to be turned on, 0 if not connected.
	AMAction3* createWaitForBrakeEnabledAction();

	/// Returns a new action that waits for the mono to move into region A.
	AMAction3* createWaitForRegionChangedToAAction();
	/// Returns a new action that waits for the mono to move into region B.
	AMAction3* createWaitForRegionChangedToBAction();
	/// Returns a new action that waits for the mono to move into a new region, 0 if not connected or if the destination is Region::None (to include this option is trickier to implement).
	AMAction3* createWaitForRegionChangedAction(int destination);
	/// Returns a new action that moves the bragg motor into the destination region, 0 if not connected or if the destination is Region::None.
	AMAction3* createMoveBraggToRegionAction(int destination);

	/// Returns a new action that waits for the mono region key to be turned CW to Disabled, 0 if not connected.
	AMAction3* createWaitForKeyDisabledAction();

	/// Returns true if the given value is a valid region.
	static bool validRegion(int region);
	/// Returns the description associated with the given step index. The step index is the index of an action in the crystal change list action.
	static QString stepDescription(int stepIndex);
	/// Returns the instruction associated with the given step index, an empty string if there is none. The step index in the index of an action in the crystal change list action.
	static QString stepInstruction(int stepIndex);
	/// Handles the region change cleanup: making sure moveInProgress_ is updated, we disconnect from crystal change action signals, and the action is queued for deletion.
	void moveCleanup(QObject *action);

protected:
	/// The current region.
	double value_;
	/// The region setpoint.
	int setpoint_;
	/// The current move state, true if this control has intiated a move.
	bool moveInProgress_;

	/// The upper slit motor.
	AMControl *upperSlit_;
	/// The lower slit motor.
	AMControl *lowerSlit_;
	/// The slits status control.
	AMControl *slitsStatus_;
	/// The paddle motor.
	AMControl *paddle_;
	/// The paddle status control.
	AMControl *paddleStatus_;
	/// The key status.
	AMControl *keyStatus_;
	/// The bragg motor.
	AMControl *bragg_;
	/// The bragg motor at crystal change position status.
	AMControl *braggAtCrystalChangePositionStatus_;
	/// The brake status.
	AMControl *brakeStatus_;
	/// The crystal change motor.
	AMControl *crystalChange_;
	/// The crystal change clockwise limit status.
	AMControl *crystalChangeCWLimitStatus_;
	/// The crystal change counter-clockwise limit status.
	AMControl *crystalChangeCCWLimitStatus_;
	/// The region A status.
	AMControl *regionAStatus_;
	/// The region B status.
	AMControl *regionBStatus_;
};

#endif // BIOXASSSRLMONOCHROMATORREGIONCONTROL_H
