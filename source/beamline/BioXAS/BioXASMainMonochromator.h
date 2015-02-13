#ifndef BIOXASMAINMONOCHROMATOR_H
#define BIOXASMAINMONOCHROMATOR_H

#include <QObject>

#include "beamline/AMControl.h"
#include "beamline/AMPVControl.h"
#include "actions3/AMAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "actions3/actions/AMControlWaitAction.h"
#include "actions3/AMListAction3.h"
#include "beamline/CLS/CLSMAXvMotor.h"

#include "beamline/BioXAS/BioXASMainMonochromatorControl.h"

// {setpoint}_{motor}_{property} VALUE
#define SETPOINT_PADDLE_OUT -55.0
#define SETPOINT_BRAGG_MOTOR_CRYSTAL_CHANGE_POSITION 108
#define SETPOINT_BRAGG_MOTOR_REGION_A_DESTINATION -10.0
#define SETPOINT_BRAGG_MOTOR_REGION_B_DESTINATION 330.0
#define SETPOINT_CRYSTAL_CHANGE_MOTOR_REGION_A_DESTINATION 15000.0
#define SETPOINT_CRYSTAL_CHANGE_MOTOR_REGION_B_DESTINATION -15000.0

// {timeout}_{component}_{condition} VALUE
#define TIMEOUT_SLITS_CLOSED 20.0
#define TIMEOUT_PADDLE_OUT 70.0
#define TIMEOUT_KEY_STATUS_CHANGE 100.0
#define TIMEOUT_CRYSTAL_CHANGE_POSITION_REACHED 220.0
#define TIMEOUT_BRAKE_STATUS_CHANGE 500.0
#define TIMEOUT_CRYSTAL_CHANGE_MOTOR_LIMIT_REACHED 60
#define TIMEOUT_BRAGG_MOTOR_LIMIT_REACHED 200
#define TIMEOUT_REGION_STATE_CHANGED 200

class BioXASMainMonochromatorCrystalChangeControl;

class BioXASMainMonochromator : public QObject
{
    Q_OBJECT

public:
    /// Enumerates the possible mono region states.
    enum Region { None = 0, A, B };
    /// Constructor.
    explicit BioXASMainMonochromator(QObject *parent = 0);
    /// Destructor.
    virtual ~BioXASMainMonochromator();

    /// Returns true if the mono is connected to all of its pvs, false otherwise.
    bool isConnected() const { return connected_; }
    /// Returns the current region.
    Region region() const { return region_; }
    /// Returns true if both slits are closed, false otherwise.
    bool slitsClosed() const { return ((int)slitsClosed_->value() == 1); }
    /// Returns true if the phosphor paddle is completely out, false otherwise.
    bool paddleOut() const { return ((int)phosphorPaddleOut_->value() == 1); }
    /// Returns true if crystal change (key status) is enabled, false if disabled.
    bool keyStatusEnabled() const { return ((int)crystalChangeEnabled_->value() == 1); }
    /// Returns true if the crystal stage is at the change position, false otherwise.
    bool atCrystalChangePosition() const { return ((int)atCrystalChangePosition_->value() == 1); }
    /// Returns true if the brake is enabled, false otherwise.
    bool brakeStatusEnabled() const { return ((int)crystalChangeBrakeEnabled_->value() == 1); }
    /// Returns true if the crystal change motor is at clockwise limit.
    bool crystalChangeMotorAtCWLimit() const { return ((int)crystalChangeMotorCWLimit_->value() == 1); }
    /// Returns true if the crystal change motor is at the counterclockwise limit.
    bool crystalChangeMotorAtCCWLimit() const { return ((int)crystalChangeMotorCCWLimit_->value() == 1); }
    /// Returns true if the bragg motor is at the clockwise limit.
    bool braggMotorAtCWLimit() const { return ((int)braggMotorCWLimit_->value() == 1); }
    /// Returns true if the bragg motor is at the counterclockwise limit.
    bool braggMotorAtCCWLimit() const { return ((int)braggMotorCCWLimit_->value() == 1); }
    /// Returns the energy setpoint.
    double energySetpoint() const { return energy_->setpoint(); }
    /// Returns the energy feedback.
    double energy() const { return energy_->value(); }

    /// Returns the upper slit blade motor.
    CLSMAXvMotor* upperSlitBladeMotor() const { return upperSlitBladeMotor_; }
    /// Returns the lower slit blade motor.
    CLSMAXvMotor* lowerSlitBladeMotor() const { return lowerSlitBladeMotor_; }
    /// Returns the bragg motor.
    CLSMAXvMotor* braggMotor() const { return braggMotor_; }
    /// Returns the vertical motor.
    CLSMAXvMotor* verticalMotor() const { return verticalMotor_; }
    /// Returns the lateral motor.
    CLSMAXvMotor* lateralMotor() const { return lateralMotor_; }
    /// Returns the crystal change motor.
    CLSMAXvMotor* crystalChangeMotor() const { return crystalExchangeMotor_; }
    /// Returns the crystal 1 pitch motor.
    CLSMAXvMotor* crystal1PitchMotor() const { return crystal1PitchMotor_; }
    /// Returns the crystal 1 roll motor.
    CLSMAXvMotor* crystal1RollMotor() const { return crystal1RollMotor_; }
    /// Returns the crystal 2 pitch motor.
    CLSMAXvMotor* crystal2PitchMotor() const { return crystal2PitchMotor_; }
    /// Returns the crystal 2 roll motor.
    CLSMAXvMotor* crystal2RollMotor() const { return crystal2RollMotor_; }

    /// Returns the slits closed motor control.
    AMControl* slitsClosedControl() const { return slitsClosed_; }
    /// Returns the paddle out control.
    AMControl* phosphorPaddleOutControl() const { return phosphorPaddleOut_; }
    /// Returns the phosphor paddle motor control.
    AMControl* phosphorPaddleMotorControl() const { return phosphorPaddle_; }
    /// Returns the control for whether the crystal change operation is enabled.
    AMControl* keyStatusControl() const { return crystalChangeEnabled_; }
    /// Returns the control for whether the bragg motor is at the crystal change position.
    AMControl* braggMotorAtCrystalChangePositionStatusControl() const { return atCrystalChangePosition_; }
    /// Returns the absolute crystal stage motor control.
    AMControl* crystalChangeMotorAbsControl() const { return stageMotorAbs_; }
    /// Returns the relative crystal change motor control.
    AMControl* crystalChangeMotorRelControl() const { return crystalChangeMotorRel_; }
    /// Returns the crystal stage clockwise limit control.
    AMControl* crystalChangeMotorCWLimitStatusControl() const { return crystalChangeMotorCWLimit_; }
    /// Returns the crystal stage counterclockwise limit control.
    AMControl* crystalChangeMotorCCWLimitStatusControl() const { return crystalChangeMotorCCWLimit_; }
    /// Returns the bragg motor clockwise limit control.
    AMControl* braggMotorCWLimitStatusControl() const { return braggMotorCWLimit_; }
    /// Returns the bragg motor counterclockwise limit control.
    AMControl* braggMotorCCWLimitStatusControl() const { return braggMotorCCWLimit_; }
    /// Returns the crystal change brake enabled control.
    AMControl* brakeStatusControl() const { return crystalChangeBrakeEnabled_; }
    /// Returns the region A status control.
    AMControl* regionAStatusControl() const { return regionAStatus_; }
    /// Returns the region B status control.
    AMControl* regionBStatusControl() const { return regionBStatus_; }
    /// Returns the energy setpoint control.
    AMControl* energyControl() const { return energy_; }
    /// Returns the mono's crystal change control.
    BioXASMainMonochromatorCrystalChangeControl* crystalChangeControl() const { return crystalChangeControl_; }

    /// Returns a new close upper slit action, 0 if not connected.
    AMAction3* createCloseUpperSlitAction();
    /// Returns a new close lower slit action, 0 if not connected.
    AMAction3* createCloseLowerSlitAction();
    /// Returns a new close slits action, 0 if not connected.
    AMAction3* createCloseSlitsAction();
    /// Returns a new action that waits for the slits to signal they are closed.
    AMAction3 *createWaitForSlitsClosedAction();
    /// Returns a new remove paddle action, 0 if not connected.
    AMAction3* createRemovePaddleAction();
    /// Returns a new action that waits for the paddle to confirm that it is removed.
    AMAction3* createWaitForPaddleRemovedAction();
    /// Returns a new action that waits for the region key to be turned CCW to Disabled, 0 if not connected.
    AMAction3* createWaitForCrystalChangeEnabledAction();
    /// Returns a new action that sends the crystal motor to the change position.
    AMAction3* createMoveToCrystalChangePositionAction();
    /// Returns a new action that waits for the mono to signal it has reached the crystal change position.
    AMAction3* createWaitForAtCrystalChangePositionAction();
    /// Returns a new action that waits for the brake to be disabled, 0 if not connected.
    AMAction3* createWaitForBrakeDisabledAction();
    /// Returns a new action that moves the crystal change motor by the given degrees (relative).
    AMAction3* createMoveCrystalChangeMotorAction(int relDestination);
    /// Returns a new action that waits for the crystal change motor to reach a limit.
    AMAction3* createWaitForCrystalChangeMotorLimitReached(bool cwLimit);
    /// Returns a new action that waits for the brake to be turned on, 0 if not connected.
    AMAction3* createWaitForBrakeEnabledAction();
    /// Returns a new action that waits for the bragg motor to reach a limit.
    AMAction3* createWaitForBraggMotorLimitReachedAction(bool cwLimit);
    /// Returns a new action that sets the crystal stage to the given absolute destination angle.
    AMAction3* createMoveBraggMotorAction(double degDestination);
    /// Returns a new action that waits for the mono to move into a new region.
    AMAction3* createWaitForMoveToNewRegion(BioXASMainMonochromator::Region destinationRegion);
    /// Returns a new action that waits for the region key to be turned CW to Disabled, 0 if not connected.
    AMAction3* createWaitForCrystalChangeDisabledAction();
    /// Returns a new crystal change action, 0 if not connected.
    AMAction3* createCrystalChangeAction();
    /// Returns a new set energy action, 0 if not connected.
    AMAction3* createSetEnergyAction(double newEnergy);
    /// Returns a new action that turns on the bragg motor, 0 if not connected.
    AMAction3* createSetBraggMotorPowerOnAction();
    /// Returns a new action that sets the bragg motor power to auto-software.
    AMAction3* createSetBraggMotorPowerAutoAction();

signals:
    /// Notifier that the mono's connections with its pvs have changed.
    void connected(bool isConnected);
    /// Notifier that the mono's region has changed.
    void regionChanged(BioXASMainMonochromator::Region newRegion);
    /// Notifier that the bragg motor power status has changed.
    void braggMotorPowerChanged(bool isOn);
    /// Notifier that the bragg motor position has changed.
    void braggMotorPositionChanged(double newPosition);
    /// Notifier that the crystal change position status has changed.
    void braggMotorAtCrystalChangePositionStatusChanged(bool atPosition);
    /// Notifier that the slits closed status has changed.
    void slitsClosedStatusChanged(bool closed);
    /// Notifier that the paddle out status has changed.
    void paddleOutStatusChanged(bool isOut);
    /// Notifier that crystal changes are enabled/disabled.
    void keyEnabledStatusChanged(bool isEnabled);
    /// Notifier that the crystal change motor position has changed.
    void crystalChangeMotorPositionChanged(double newPosition);
    /// Notifier that the brake enabled status has changed.
    void crystalChangeBrakeEnabledChanged(bool isEnabled);
    /// Notifier that the crystal stage clockwise limit status has changed.
    void crystalChangeMotorCWLimitStatusChanged(bool atLimit);
    /// Notifier that the crystal stage counterclockwise limit status has changed.
    void crystalChangeMotorCCWLimitStatusChanged(bool atLimit);
    /// Notifier that the energy setpoint has changed.
    void energySetpointChanged(double newSetpoint);
    /// Notifier that the energy feedback has changed.
    void energyFeedbackChanged(double newFeedback);

public slots:
    /// Sets the energy setpoint.
    void setEnergy(double newEnergy) { energy_->move(newEnergy); }

protected slots:
    /// Updates the mono's general connected state based on the connected state of each of its pvs.
    void onConnectedChanged();
    /// Updates the mono's current region state based on the values of regionAStatus_ and regionBStatus_ controls.
    void onRegionChanged();
    /// Emits the appropriate signal when the mono's bragg motor has changed power states.
    void onBraggMotorPowerChanged(double value) { emit braggMotorPowerChanged((int)value == 1); }
    /// Emits the appropriate signal when the mono's bragg motor has changed position.
    void onBraggMotorPositionChanged(double newPosition) { emit braggMotorPositionChanged(newPosition); }
    /// Emits the appropriate signal when the crystal change position status has changed.
    void onCrystalChangePositionStatusChanged(double value) { emit braggMotorAtCrystalChangePositionStatusChanged((int)value == 0); }
    /// Emits the appropriate signal when the slits closed status has changed.
    void onSlitsClosedStatusChanged(double value) { emit slitsClosedStatusChanged((int)value == 1); }
    /// Emits the appropriate signal when the paddle is out.
    void onPaddleOutChanged(double value) { emit paddleOutStatusChanged((int)value == 1); }
    /// Emits the appropriate signal when the crystal change motor is enabled/disabled.
    void onCrystalChangeEnabled(double value) { emit keyEnabledStatusChanged((int)value == 1); }
    /// Emits the appropriate signal when the crystal change brake is enabled/disabled.
    void onCrystalChangeBrakeEnabledChanged(double value) { emit crystalChangeBrakeEnabledChanged((int) value == 1); }
    /// Emits the appropriate signal when the crystal change motor reaches/leaves the CW limit.
    void onCrystalChangeMotorCWLimitStatusChanged(double value) { emit crystalChangeMotorCWLimitStatusChanged((int)value == 0); }
    /// Emits the appropriate signal when the crystal change motor reaches/leaves the CCW limit.
    void onCrystalChangeMotorCCWLimitStatusChanged(double value) { emit crystalChangeMotorCCWLimitStatusChanged((int)value == 0); }

protected:

    // Connected state.

    bool connected_;

    // Current region.

    Region region_;

    // Controls

    CLSMAXvMotor *lowerSlitBladeMotor_;
    CLSMAXvMotor *upperSlitBladeMotor_;
    CLSMAXvMotor *braggMotor_;
    CLSMAXvMotor *verticalMotor_;
    CLSMAXvMotor *lateralMotor_;
    CLSMAXvMotor *crystalExchangeMotor_;
    CLSMAXvMotor *crystal1PitchMotor_;
    CLSMAXvMotor *crystal1RollMotor_;
    CLSMAXvMotor *crystal2PitchMotor_;
    CLSMAXvMotor *crystal2RollMotor_;

    AMControl* slitsClosed_;
    AMControl* phosphorPaddle_;
    AMControl* phosphorPaddleOut_;
    AMControl* braggMotorPower_;
    AMControl* crystalChangeEnabled_;
    AMControl* atCrystalChangePosition_;
    AMControl* crystalChangeBrakeEnabled_;
    AMControl* stageMotorAbs_;
    AMControl* crystalChangeMotorRel_;
    AMControl* crystalChangeMotorCWLimit_;
    AMControl* crystalChangeMotorCCWLimit_;
    AMControl* braggMotorCWLimit_;
    AMControl* braggMotorCCWLimit_;
    AMControl* regionAStatus_;
    AMControl* regionBStatus_;

    // Energy control.

    AMControl* energy_;

    // Crystal change control.

    BioXASMainMonochromatorCrystalChangeControl *crystalChangeControl_;
};


#endif // BIOXASMAINMONOCHROMATOR_H
