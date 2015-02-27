#ifndef BIOXASMAINMONOCHROMATORREGIONCONTROL_H
#define BIOXASMAINMONOCHROMATORREGIONCONTROL_H

#include "beamline/AMCompositeControl.h"
#include "actions3/AMActionSupport.h"
#include "actions3/actions/AMControlWaitAction.h"
#include "actions3/AMListAction3.h"

// {setpoint}_{motor}_{property} VALUE
#define SETPOINT_SLITS_CLOSED 0.0
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

// {status}_{component}_{status option} VALUE
#define STATUS_SLITS_CLOSED 1
#define STATUS_PADDLE_OUT 1
#define STATUS_KEY_ENABLED 1
#define STATUS_BRAGG_AT_CRYSTAL_CHANGE_POSITION 0
#define STATUS_BRAKE_DISABLED 0
#define STATUS_CRYSTAL_CHANGE_AT_LIMIT 1
#define STATUS_CRYSTAL_CHANGE_NOT_AT_LIMIT 0
#define STATUS_BRAKE_ENABLED 1
#define STATUS_KEY_DISABLED 0
#define STATUS_REGION_IN 1
#define STATUS_REGION_NOT_IN 0

class BioXASMainMonochromatorRegionControl : public AMCompositeControl
{
	Q_OBJECT

public:
	/// Enumerates the possible region states.
	class Region { public: enum State { None = 0, A, B }; };

	/// Constructor.
	explicit BioXASMainMonochromatorRegionControl(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASMainMonochromatorRegionControl();

	/// Returns the current region.
	virtual double value() const { return region_; }
	/// Returns the current region setpoint.
	virtual double setpoint() const { return regionSetpoint_; }
	/// Returns None, the smallest value the region can take.
	virtual double minimumValue() const { return Region::None; }
	/// Returns B, the largest value the region can take.
	virtual double maximumValue() const { return Region::B; }

	/// Returns true if the region is always measurable (when the control is connected).
	virtual bool shouldMeasure() const { return true; }
	/// Returns true if a region measurement is possible now.
	virtual bool canMeasure() const;
	/// Returns true if a move to a new region is always possible, provided control is connected.
	virtual bool shouldMove() const { return true; }
	/// Returns true if a move to a new region is possible now.
	virtual bool canMove() const;
	/// Returns true if this control can stop a crystal change in progress, provided it is connected.
	virtual bool shouldStop() const { return true; }
	/// Returns true if this control can stop a change to a new region right now.
	virtual bool canStop() const;

	/// Returns true if there is a crystal change procedure in progress, as a result of this control's action.
	virtual bool moveInProgress() const { return false; }

signals:
	/// Notifier that there has been progress in completing a move. Provides information needed for a progress bar display.
	void moveProgressChanged(double numerator, double denominator);
	/// Notifier that the current step in a move has changed.
	void moveStepChanged();

public slots:
	/// Sets the new region setpoint and performs a crystal change, if necessary.
	virtual FailureExplanation move(double setpoint);
	/// Performs a relative move. For this control, relative moves are disabled/they don't make sense.
	virtual FailureExplanation moveRelative(double distance, RelativeMoveType relativeMoveType);
	/// Stops a crystal change in progress.
	virtual bool stop() { return false; }

	/// Sets the upper slit control.
	void setUpperSlitControl(AMControl *upperSlit);
	/// Sets the lower slit control.
	void setLowerSlitControl(AMControl *lowerSlit);
	/// Sets the slits status control.
	void setSlitsStatusControl(AMControl *slitsStatus);
	/// Sets the paddle motor control.
	void setPaddleControl(AMControl *paddleControl);
	/// Sets the paddle out control.
	void setPaddleStatusControl(AMControl *paddleStatus);
	/// Sets the key status control.
	void setKeyStatusControl(AMControl *keyStatusControl);
	/// Sets the bragg motor control.
	void setBraggControl(AMControl *braggControl);
	/// Sets the bragg motor at crystal change position control.
	void setBraggAtCrystalChangePositionControl(AMControl *braggInPositionControl);
	/// Sets the brake status control.
	void setBrakeStatusControl(AMControl *brakeStatusControl);
	/// Sets the crystal change motor control.
	void setCrystalChangeControl(AMControl *crystalChangeControl);
	/// Sets the crystal change cw limit control.
	void setCrystalChangeCWLimitControl(AMControl *cwLimitControl);
	/// Sets the crystal change ccw limit control.
	void setCrystalChangeCCWLimitControl(AMControl *ccwLimitControl);
	/// Sets the region A status control.
	void setRegionAStatusControl(AMControl *regionAStatusControl);
	/// Sets the region B status control.
	void setRegionBStatusControl(AMControl *regionBStatusControl);

protected slots:
	/// Sets the current region in response to a value change from one of the region controls.
	void onRegionControlValueChanged();
	/// Handles emitting the appropriate signals when the current step in a move has changed.
	void onCurrentMoveStepChanged(int stepIndex);
	/// Called when the internal crystal change action has been cancelled. Handles emitting moveFailed(...) with the WasStoppedFailure code and deleting the action.
	void onRegionChangeCancelled();
	/// Called when the internal crystal change action has failed. Handles emitting moveFailed(...) with the OtherFailure code and deleting the action.
	void onRegionChangeFailed();
	/// Called when the internal crystal change action has succeeded! Handles emitting moveSucceeded() and deleting the action.
	void onRegionChangeSucceeded();

protected:
	/// Returns a new action that closes the upper slit, 0 if not connected.
	AMAction3* createCloseUpperSlitAction();
	/// Returns a new action that closes the lower slit, 0 if not connected.
	AMAction3* createCloseLowerSlitAction();
	/// Returns a new action that closes both slits, 0 if not connected.
	AMAction3* createCloseSlitsAction();
	/// Returns a new action that waits for the slits status to change from 'not closed' to 'closed'.
	AMAction3* createWaitForSlitsClosedAction();
	/// Returns a new action that removes the phosphor paddle, 0 if not connected.
	AMAction3* createRemovePaddleAction();
	/// Returns a new action that waits for the mono paddle to confirm that it is removed.
	AMAction3* createWaitForPaddleRemovedAction();
	/// Returns a new action that waits for the mono region key to be turned CCW to Enabled, 0 if not connected.
	AMAction3* createWaitForKeyEnabledAction();
	/// Returns a new action that moves the bragg motor control to the given destination (degrees), 0 if not connected.
	AMAction3* createMoveBraggAction(double destination);
	/// Returns a new action that sends the mono bragg motor to the change position.
	AMAction3* createMoveBraggToCrystalChangePositionAction();
	/// Returns a new action that waits for the mono to signal it has reached the crystal change position.
	AMAction3* createWaitForBraggAtCrystalChangePositionAction();
	/// Returns a new action that waits for the mono brake to be disabled, 0 if not connected.
	AMAction3* createWaitForBrakeDisabledAction();
	/// Returns a new action that moves the crystal change motor to the given destination (relative move), 0 if not connected.
	AMAction3* createMoveCrystalChangeAction(double destination);
	/// Returns a new action that waits for the crystal change motor to reach its clockwise limit, 0 if not connected.
	AMAction3* createWaitForCrystalChangeAtCWLimitAction();
	/// Returns a new action that waits for the crystal change motor to reach its counter-clockwise limit, 0 if not connected.
	AMAction3* createWaitForCrystalChangeAtCCWLimitAction();
	/// Returns a new action that moves the crystal change motor to the appropriate limit for the given region destination, 0 if not connected or if the destination is Region::None.
	AMAction3* createMoveCrystalChangeToRegionLimitAction(Region::State destination);
	/// Returns a new action that waits for the mono crystal change motor to reach a limit. Which particular limit (cw, ccw) depends on the region destination. Returns 0 if not connected, or if the destination is Region::None.
	AMAction3* createWaitForCrystalChangeAtLimitAction(Region::State destination);
	/// Returns a new action that waits for the mono brake to be turned on, 0 if not connected.
	AMAction3* createWaitForBrakeEnabledAction();
	/// Returns a new action that moves the bragg motor into region A, 0 if not connected.
	AMAction3* createMoveBraggToRegionAAction();
	/// Returns a new action that moves the bragg motor into region B, 0 if not connected.
	AMAction3* createMoveBraggToRegionBAction();
	/// Returns a new action that moves the bragg motor into the destination region, 0 if not connected or if the destination is Region::None.
	AMAction3* createMoveBraggToRegionAction(Region::State destination);
	/// Returns a new action that waits for the mono to move into region A.
	AMAction3* createWaitForRegionChangedToAAction();
	/// Returns a new action that waits for the mono to move into region B.
	AMAction3* createWaitForRegionChangedToBAction();
	/// Returns a new action that waits for the mono to move into a new region, 0 if not connected or if the destination is Region::None (to include this option is trickier to implement).
	AMAction3* createWaitForRegionChangedAction(Region::State destination);
	/// Returns a new action that waits for the mono region key to be turned CW to Disabled, 0 if not connected.
	AMAction3* createWaitForKeyDisabledAction();
	/// Returns a new action that changes the current region to the new, desired region. Performs a crystal change.
	AMListAction3 *createChangeRegionAction(Region::State newRegion);

protected:
	/// The current region state.
	Region::State region_;
	/// The region setpoint.
	Region::State regionSetpoint_;

	// Child controls.

	/// The upper slit control.
	AMControl* upperSlitControl_;
	/// The lower slit control.
	AMControl* lowerSlitControl_;
	/// The slits closed status control.
	AMControl* slitsStatus_;
	/// The paddle motor control.
	AMControl* paddleControl_;
	/// The paddle status control.
	AMControl* paddleStatus_;
	/// The key status control.
	AMControl* keyStatus_;
	/// The bragg motor control.
	AMControl* braggControl_;
	/// The bragg motor 'in position' control.
	AMControl* braggAtCrystalChangePositionStatus_;
	/// The brake status control.
	AMControl* brakeStatus_;
	/// The crystal change motor control.
	AMControl* crystalChangeControl_;
	/// The crystal change motor clockwise limit control.
	AMControl* crystalChangeCWLimitStatus_;
	/// The crystal change motor counter-clockwise limit control.
	AMControl* crystalChangeCCWLimitStatus_;
	/// The region A status control.
	AMControl* regionAStatus_;
	/// The region B status control.
	AMControl* regionBStatus_;

};

#endif // BIOXASMAINMONOCHROMATORREGIONCONTROL_H
