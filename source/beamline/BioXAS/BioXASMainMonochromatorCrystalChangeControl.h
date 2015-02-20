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
    /// Enum marking possible different crystal change steps.
	enum Step { None = 0, SlitsNotClosed, SlitsClosed, PaddleNotOut, PaddleOut, KeyNotEnabled, KeyEnabled, BraggNotAtCrystalChangePosition, BraggAtCrystalChangePosition, BrakeNotDisabled, BrakeDisabled, CrystalChangeMotorNotAtCWLimit, CrystalChangeMotorNotAtCCWLimit, CrystalChangeMotorAtCWLimit, CrystalChangeMotorAtCCWLimit, BrakeNotEnabled, BrakeEnabled, BraggNotAtNewRegion, BraggAtNewRegion, KeyNotDisabled, KeyDisabled };
	/// Enum marking the possible different limit changes.
	enum Limit { Unknown = 0, CW, CCW };
	/// Constructor.
    explicit BioXASMainMonochromatorCrystalChangeControl(BioXASMainMonochromator *mono, QObject *parent = 0);
    /// Destructor.
    virtual ~BioXASMainMonochromatorCrystalChangeControl();
    /// Returns the monochromator being controlled.
    BioXASMainMonochromator* mono() const { return mono_; }
	/// Returns the desired region.
	BioXASMainMonochromator::Region nextRegion() const { return nextRegion_; }
	/// Returns the bragg motor crystal change position.
	double braggMotorCrystalChangePosition() const { return braggMotorCrystalChangePosition_; }
    /// Returns the control's current step in a crystal change.
    Step step() const { return step_; }
	/// Returns a string description of the given step.
	static QString stepDescription(BioXASMainMonochromatorCrystalChangeControl::Step step);
	/// Returns a string instruction related to the given step.
	static QString stepInstruction(BioXASMainMonochromatorCrystalChangeControl::Step step);

signals:
    /// Notifier that the mono being controlled has changed.
    void monoChanged(BioXASMainMonochromator *newMono);
	/// Notifier that the desired region has changed.
	void nextRegionChanged(BioXASMainMonochromator::Region newRegion);
	/// Notifier that the bragg motor crystal change position has changed.
	void braggMotorCrystalChangePositionChanged(double newPosition);
	/// Notifier that the bragg motor destination for the next region has changed.
	void braggMotorRegionDestinationChanged(double newPosition);
	/// Notifier that the control's current step has changed.
	void stepChanged(BioXASMainMonochromatorCrystalChangeControl::Step newStep);
    /// Notifier that the currently running action has changed, and so has the corresponding description and user information.
    void currentActionChanged(const QString &newDescription, const QString &newInformation);
    /// Notifier that the action execution progress has changed. Contains info suitable for a progress bar.
    void progressChanged(double value, double valueMax);
	/// Notifier that the crystal change process is complete. Argument is true if successful, false if failed.
	void crystalChangeComplete(bool success);

public slots:
    /// Sets the monochromator.
    void setMono(BioXASMainMonochromator *newMono);
	/// Sets the bragg motor crystal change position.
	void setBraggMotorCrystalChangePosition(double newPosition);
	/// Sets the desired region.
	void setNextRegion(BioXASMainMonochromator::Region newRegion);
	/// Creates a new action and starts the crystal change.
    void startCrystalChange();

protected slots:
	/// Sets the control's current step.
	void setStep(Step newStep);
	/// Sets the bragg motor destination for the next region.
	void setBraggMotorRegionDestination(double newPosition);
	/// Handles updating the current step when the mono's connection state has changed.
	void onConnectedChanged();
	/// Handles updating the current step when some mono condition has changed. 'onConnectedChanged' would be the primary case of this more general slot.
	void onConditionChanged();

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
	/// Connects to the given action and runs it.
	void runAction(AMAction3 *action);

protected:
    /// The monochromator being controlled.
    BioXASMainMonochromator *mono_;
	/// The desired region.
	BioXASMainMonochromator::Region nextRegion_;
	/// The bragg motor crystal change position.
	double braggMotorCrystalChangePosition_;
	/// The desired crystal change motor limit.
	Limit crystalChangeMotorLimit_;
	/// The destination for the bragg motor for change to crystal A.
	double braggMotorRegionADestination_;
	/// The destination for the bragg motor for change to crystal B.
	double braggMotorRegionBDestination_;
	/// The destination for the bragg motor in the next region.
	double braggMotorRegionDestination_;
    /// The current step of a crystal change.
    Step step_;

private:
	/// Returns the next step in the crystal change process.
	BioXASMainMonochromatorCrystalChangeControl::Step nextStep(BioXASMainMonochromatorCrystalChangeControl::Step step);
	/// Returns the current step, testing all mono conditions to be sure.
	BioXASMainMonochromatorCrystalChangeControl::Step findCurrentStep();
    /// Disconnects from the given action and marks it for deletion.
    void deleteAction(QObject *crystalChangeAction);
};

#endif // BIOXASMAINMONOCHROMATORCRYSTALCHANGECONTROL_H
