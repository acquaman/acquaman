#ifndef BIOXASMAINMONOCHROMATORCRYSTALCHANGECONTROL_H
#define BIOXASMAINMONOCHROMATORCRYSTALCHANGECONTROL_H

#include <QObject>

#include "BioXASMainMonochromator.h"

// {setpoint}_{motor}_{property} VALUE
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

class BioXASMainMonochromatorCrystalChangeControl : public QObject
{
    Q_OBJECT

public:
    /// Enum marking possible different control states.
    enum State { NotInitialized = 0, Initialized, Running, CompleteSuccess, CompleteFail };
    /// Enum marking possible different crystal change steps.
	enum Step { None = 0, SlitsNotClosed, SlitsClosed, PaddleNotOut, PaddleOut, KeyNotEnabled, KeyEnabled, BraggNotAtCrystalChangePosition, BraggAtCrystalChangePosition, BrakeNotDisabled, BrakeDisabled, CrystalChangeMotorNotAtCWLimit, CrystalChangeMotorNotAtCCWLimit, CrystalChangeMotorAtCWLimit, CrystalChangeMotorAtCCWLimit, BrakeNotEnabled, BrakeEnabled, BraggNotAtNewRegion, BraggAtNewRegion, KeyNotDisabled, KeyDisabled };
    /// Constructor.
    explicit BioXASMainMonochromatorCrystalChangeControl(BioXASMainMonochromator *mono, QObject *parent = 0);
    /// Destructor.
    virtual ~BioXASMainMonochromatorCrystalChangeControl();
    /// Returns the monochromator being controlled.
    BioXASMainMonochromator* mono() const { return mono_; }
    /// Returns the current state of the control.
    State state() const { return state_; }
    /// Returns the control's current step in a crystal change.
    Step step() const { return step_; }
	/// Returns a string representation of the given state.
	static QString stateToString(BioXASMainMonochromatorCrystalChangeControl::State state);
	/// Returns a string description of the given step.
	static QString stepDescription(BioXASMainMonochromatorCrystalChangeControl::Step step);
	/// Returns a string instruction related to the given step.
	static QString stepInstruction(BioXASMainMonochromatorCrystalChangeControl::Step step);

signals:
    /// Notifier that the mono being controlled has changed.
    void monoChanged(BioXASMainMonochromator *newMono);
    /// Notifier that the control's current state has changed.
    void stateChanged(BioXASMainMonochromatorCrystalChangeControl::State newState);
    /// Notifier that the currently running action has changed, and so has the corresponding description and user information.
    void currentActionChanged(const QString &newDescription, const QString &newInformation);
    /// Notifier that the action execution progress has changed. Contains info suitable for a progress bar.
    void progressChanged(double value, double valueMax);

public slots:
    /// Sets the monochromator.
    void setMono(BioXASMainMonochromator *newMono);
    /// Starts the crystal change, if current mono is valid and there isn't a crytal change running already.
    void startCrystalChange();
    /// Resets the state of the control. State goes to either None or Initialized, as conditions allow. The idea is to reset the state before and/or after a crystal change, to make sure we don't dwell in a CompleteSuccess or CompleteFail state from a previous change.
    void reset();

protected slots:
    /// Sets the control's current state.
    void setState(State newState);
    /// Handles updating the state when the mono's connection state has changed.
    void onMonoConnectedChanged();
    /// Handles updating the state when the crystal change actions have started.
    void onCrystalChangeActionsStarted();
    /// Handles when the currently executing subaction has changed.
    void onCrystalChangeSubActionChanged(int actionIndex);
    /// Handles updating the state when the crystal change actions have succeeded.
    void onCrystalChangeActionsSucceeded();
    /// Handles updating the state when the crystal change actions report themselves cancelled/failed/destroyed.
    void onCrystalChangeActionsFailed();

protected:
	/// Returns a new action that waits for the mono slits to signal they are closed.
	AMAction3* createWaitForSlitsClosedAction();
	/// Returns a new action that waits for the mono paddle to confirm that it is removed.
	AMAction3* createWaitForPaddleRemovedAction();
	/// Returns a new action that waits for the mono region key to be turned CCW to Enabled, 0 if not connected.
	AMAction3* createWaitForKeyEnabledAction();
	/// Returns a new action that sends the mono bragg motor to the change position.
	AMAction3* createMoveToCrystalChangePositionAction();
	/// Returns a new action that waits for the mono to signal it has reached the crystal change position.
	AMAction3* createWaitForAtCrystalChangePositionAction();
	/// Returns a new action that waits for the mono brake to be disabled, 0 if not connected.
	AMAction3* createWaitForBrakeDisabledAction();
	/// Returns a new action that waits for the mono crystal change motor to reach a limit.
	AMAction3* createWaitForCrystalChangeMotorLimitReached(bool cwLimit);
	/// Returns a new action that waits for the mono brake to be turned on, 0 if not connected.
	AMAction3* createWaitForBrakeEnabledAction();
	/// Returns a new action that waits for the bragg motor to reach a limit.
	AMAction3* createWaitForBraggMotorLimitReachedAction(bool cwLimit);
	/// Returns a new action that waits for the mono to move into a new region.
	AMAction3* createWaitForMoveToNewRegion(BioXASMainMonochromator::Region destinationRegion);
	/// Returns a new action that waits for the mono region key to be turned CW to Disabled, 0 if not connected.
	AMAction3* createWaitForKeyDisabledAction();
	/// Returns a new crystal change action that changes the current mono crystal to the desired crystal, 0 if not connected. If no destination region is provided and current region is valid, the opposite region is the assumed destination. If the current region is invalid, the destination is automatically A.
	AMAction3* createCrystalChangeAction(BioXASMainMonochromator::Region newRegion);
	AMAction3* createCrystalChangeAction();

protected:
    /// The monochromator being controlled.
    BioXASMainMonochromator *mono_;
    /// The current state of the control.
    State state_;
    /// The current step of a crystal change.
    Step step_;

private:
    /// Disconnects from the given action and marks it for deletion.
    void deleteAction(QObject *crystalChangeAction);
    /// Handles updating the state to match present action and mono conditions.
    void updateState();

};

#endif // BIOXASMAINMONOCHROMATORCRYSTALCHANGECONTROL_H
