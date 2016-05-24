#ifndef BIOXASSSRLMONOCHROMATORREGIONCONTROL_H
#define BIOXASSSRLMONOCHROMATORREGIONCONTROL_H

#include "actions3/AMActionSupport.h"
#include "actions3/actions/AMControlWaitAction.h"
#include "actions3/actions/AMWaitAction.h"
#include "actions3/AMListAction3.h"
#include "beamline/AMEnumeratedControl.h"
#include "util/AMErrorMonitor.h"
#include "beamline/BioXAS/BioXASMAXvMotor.h"
#include "beamline/CLS/CLSMAXvMotor.h"

// {setpoint}_{motor}_{property} VALUE
//#define SETPOINT_SLIT_CLOSED 0.0
//#define SETPOINT_PADDLE_REMOVED 0.0
//#define SETPOINT_BRAGG_MOTOR_CRYSTAL_CHANGE_POSITION 55.0
//#define SETPOINT_BRAGG_MOTOR_REGION_A_DESTINATION -10.0
//#define SETPOINT_BRAGG_MOTOR_REGION_B_DESTINATION 330.0
//#define SETPOINT_CRYSTAL_CHANGE_MOTOR_REGION_A_DESTINATION 15000.0
//#define SETPOINT_CRYSTAL_CHANGE_MOTOR_REGION_B_DESTINATION -15000.0

#define BIOXASSSRLMONOCHROMATORREGIONCONTROL_MASK_UPPER_CLOSED_LIMIT CLSMAXvMotor::LimitCW;

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

// Error codes
#define BioXAS_MONO_REGION_NOT_CONNECTED 1407701
#define BioXAS_MONO_REGION_ALREADY_MOVING 1407702
#define BioXAS_MONO_REGION_CANNOT_MOVE 1407703
#define BioXAS_MONO_REGION_INVALID_SETPOINT 1407704
#define BioXAS_MONO_REGION_INVALID_ACTION 1407705
#define BioXAS_MONO_REGION_CLOSE_UPPER_SLIT_FAILED 1407706
#define BioXAS_MONO_REGION_CLOSE_LOWER_SLIT_FAILED 1407707
#define BioXAS_MONO_REGION_CLOSE_SLITS_FAILED 1407708
#define BioXAS_MONO_REGION_MOVE_PADDLE_FAILED 1407709
#define BioXAS_MONO_REGION_PADDLE_WAIT_FAILED 1407710
#define BioXAS_MONO_REGION_KEY_ENABLED_WAIT_FAILED 1407711
#define BioXAS_MONO_REGION_CHANGE_BRAGG_TOLERANCE_FAILED 14077110
#define BioXAS_MONO_REGION_MOVE_BRAGG_FAILED 1407712
#define BioXAS_MONO_REGION_BRAGG_WAIT_FAILED 1407713
#define BioXAS_MONO_REGION_BRAKE_DISABLED_WAIT_FAILED 1407714
#define BioXAS_MONO_REGION_MOVE_CRYSTAL_CHANGE_FAILED 1407715
#define BioXAS_MONO_REGION_CRYSTAL_CHANGE_WAIT_FAILED 1407716
#define BioXAS_MONO_REGION_BRAKE_ENABLED_WAIT_FAILED 1407717
#define BioXAS_MONO_REGION_REGION_A_WAIT_FAILED 1407718
#define BioXAS_MONO_REGION_REGION_B_WAIT_FAILED 1407719
#define BioXAS_MONO_REGION_KEY_DISABLED_WAIT_FAILED 1407720

class BioXASSSRLMonochromatorRegionControl : public AMEnumeratedControl
{
	Q_OBJECT

public:
	/// Enum marking possible different crystal change steps.
	enum Step { CloseSlits = 0, RemovePaddle, WaitForKeyEnabled, MoveBraggIntoPosition, WaitForBrakeDisabled, MoveCrystalChangeIntoPosition, WaitForBrakeEnabled, MoveBraggIntoRegion, WaitForKeyDisabled, None };

	/// Constructor.
	explicit BioXASSSRLMonochromatorRegionControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromatorRegionControl();

	/// Returns true if the region is always measurable, provided the control is connected.
	virtual bool shouldMeasure() const { return true; }
	/// Returns true if a move to a new region is always possible, provided control is connected.
	virtual bool shouldMove() const { return true; }
	/// Returns true if this control can stop a crystal change in progress, provided it is connected.
	virtual bool shouldStop() const { return false; }

	/// Returns true if this control can take a measurement right now.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now.
	virtual bool canMove() const;
	/// Returns true if this control can stop a change to a new region right now.
	virtual bool canStop() const { return false; }

	/// Returns true if the given value is a valid value for this control, false otherwise.
	virtual bool validValue(double value) const;
	/// Returns true if the given value is a valid setpoint for this control, false otherwise.
	virtual bool validSetpoint(double value) const;

	/// Returns the upper slit blade control.
	BioXASMAXvMotor* upperSlitBladeControl() const { return maskUpperBlade_; }
	/// Returns the lower slit blade control.
	BioXASMAXvMotor* lowerSlitBladeControl() const { return maskLowerBlade_; }
	/// Returns the slits status control.
	AMControl* slitsStatusControl() const { return maskBladesStatus_; }
	/// Returns the paddle control.
	BioXASMAXvMotor* paddleControl() const { return paddle_; }
	/// Returns the paddle status control.
	AMControl* paddleStatusControl() const { return paddleStatus_; }
	/// Returns the key status control.
	AMControl* keyStatusControl() const { return keyStatus_; }
	/// Returns the brake status control.
	AMControl* brakeStatusControl() const { return brakeStatus_; }
	/// Returns the bragg control.
	BioXASMAXvMotor* braggControl() const { return bragg_; }
	/// Returns the bragg motor at crystal change position status control.
	AMControl* braggAtCrystalChangePositionStatusControl() const { return braggAtCrystalChangePositionStatus_; }
	/// Returns the crystal change control.
	BioXASMAXvMotor* crystalChangeControl() const { return crystalChange_; }
	/// Returns the crystal change cw limit status control.
	AMControl* crystalChangeCWLimitStatusControl() const { return crystalChangeCWLimitStatus_; }
	/// Returns the crystal change ccw limit status control.
	AMControl* crystalChangeCCWLimitStatusControl() const { return crystalChangeCCWLimitStatus_; }
	/// Returns the region A status control.
	AMControl* regionAStatusControl() const { return regionAStatus_; }
	/// Returns the region B status control.
	AMControl* regionBStatusControl() const { return regionBStatus_; }

signals:
	/// Notifier that there has been progress in completing a crystal change.
	void moveProgressChanged(double numerator, double denominator);
	/// Notifier that the current step in a move has changed.
	void moveStepChanged(const QString &newDescription, const QString &newInstruction, const QString &newNotes);

public slots:
	/// Sets the upper slit blade control.
	void setUpperSlitBladeControl(BioXASMAXvMotor *newControl);
	/// Sets the lower slit blade control.
	void setLowerSlitBladeControl(BioXASMAXvMotor *newControl);
	/// Sets the slits status control.
	void setSlitsStatusControl(AMControl *newControl);
	/// Sets the paddle motor control.
	void setPaddleControl(BioXASMAXvMotor *paddle);
	/// Sets the paddle status control.
	void setPaddleStatusControl(AMControl *paddleStatus);
	/// Sets the key status control.
	void setKeyStatusControl(AMControl *keyStatus);
	/// Sets the brake status control.
	void setBrakeStatusControl(AMControl *brakeStatus);
	/// Sets the bragg motor control.
	void setBraggControl(BioXASMAXvMotor *bragg);
	/// Sets the bragg at crystal change position status control.
	void setBraggAtCrystalChangePositionStatusControl(AMControl *inPosition);
	/// Sets the crystal change motor control.
	void setCrystalChangeControl(BioXASMAXvMotor *crystalChange);
	/// Sets the crystal change cw limit status control.
	void setCrystalChangeCWLimitStatusControl(AMControl *limitStatus);
	/// Sets the crystal change ccw limit status control.
	void setCrystalChangeCCWLimitStatusControl(AMControl *limitStatus);
	/// Sets the region A status control.
	void setRegionAStatusControl(AMControl *regionStatus);
	/// Sets the region B status control.
	void setRegionBStatusControl(AMControl *regionStatus);

protected slots:
	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the 'is moving' state.
	virtual void updateMoving();

	/// Handles emitting the appropriate signals when the current step in a move has changed.
	void onMoveStepChanged(int stepIndex);

protected:
	/// Returns a new action that performs a crystal change to the new region.
	AMAction3* createMoveAction(double newRegion);

	/// Returns a new action that moves the mask upper blade to the given limit, 0 if not connected.
	AMAction3* createMoveMaskUpperBladeToLimitAction(CLSMAXvMotor::Limit limit);
	/// Returns a new action that moves the mask lower blade to the given limit, 0 if not connected.
	AMAction3* createMoveMaskLowerBladeToLimitAction(CLSMAXvMotor::Limit limit);
	/// Returns a new action that waits for the slits status to change from 'not closed' to 'closed'.
	AMAction3* createWaitForMaskBladesClosedAction();
	/// Returns a new action that closes the mask blades, 0 if not connected.
	AMAction3* createCloseMaskBladesAction();

	/// Returns a new action that moves the paddle motor to the given destination.
	AMAction3* createMovePaddleToLimitAction(CLSMAXvMotor::Limit limit);
	/// Returns a new action that waits for the paddle to be removed.
	AMAction3* createWaitForPaddleRemovedAction();
	/// Returns a new action that removes the paddle, 0 if not connected.
	AMAction3* createRemovePaddleAction();

	/// Returns a new action that waits for the key status to change to 'Enabled'.
	AMAction3* createWaitForKeyEnabledAction();

	/// Returns a new action that moves the bragg motor control to the given limit.
	AMAction3* createMoveBraggToLimitAction(CLSMAXvMotor::Limit limit);
	/// Returns a new action that waits for the mono to signal it has reached the crystal change position.
	AMAction3* createWaitForBraggAtCrystalChangePositionAction();
	/// Returns a new action that sends the mono bragg motor to the change position.
	AMAction3* createMoveBraggToCrystalChangePositionAction();

	/// Returns a new action that waits for the mono brake to be disabled, 0 if not connected.
	AMAction3* createWaitForBrakeDisabledAction();

	/// Returns a new action that moves the crystal change motor to the given limit.
	AMAction3* createMoveCrystalChangeMotorToLimit(CLSMAXvMotor::Limit limit);
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

	/// Returns the current index.
	virtual int currentIndex() const;

	/// Returns a string representation of the given region state.
	static QString regionStateToString(int region);
	/// Returns the description associated with the given step index. The step index is the index of an action in the crystal change list action.
	static QString stepDescription(int stepIndex);
	/// Returns the instruction associated with the given step index, an empty string if there is none. The step index in the index of an action in the crystal change list action.
	static QString stepInstruction(int stepIndex);
	/// Returns the notes associated with the given step index, an empty string if there is none.  The step index in the index of an action in the crystal change list action.
	static QString stepNotes(int stepIndex);

protected:
	/// The upper slit blade control.
	BioXASMAXvMotor *maskUpperBlade_;
	/// The lower slit blade control.
	BioXASMAXvMotor *maskLowerBlade_;
	/// The slits status control.
	AMControl *maskBladesStatus_;
	/// The paddle motor control.
	BioXASMAXvMotor *paddle_;
	/// The paddle status control.
	AMControl *paddleStatus_;
	/// The key status control.
	AMControl *keyStatus_;
	/// The bragg motor control.
	BioXASMAXvMotor *bragg_;
	/// The bragg motor at crystal change position status control.
	AMControl *braggAtCrystalChangePositionStatus_;
	/// The brake status control.
	AMControl *brakeStatus_;
	/// The crystal change motor control.
	BioXASMAXvMotor *crystalChange_;
	/// The crystal change clockwise limit status control.
	AMControl *crystalChangeCWLimitStatus_;
	/// The crystal change counter-clockwise limit status control.
	AMControl *crystalChangeCCWLimitStatus_;
	/// The region A status control.
	AMControl *regionAStatus_;
	/// The region B status control.
	AMControl *regionBStatus_;
};

#endif // BIOXASSSRLMONOCHROMATORREGIONCONTROL_H
