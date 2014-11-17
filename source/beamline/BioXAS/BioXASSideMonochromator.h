#ifndef BIOXASSIDEMONOCHROMATOR_H
#define BIOXASSIDEMONOCHROMATOR_H

#include <QObject>

#include "beamline/AMControl.h"
#include "beamline/AMPVControl.h"
#include "actions3/AMAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "actions3/actions/AMControlWaitAction.h"
#include "actions3/AMListAction3.h"
#include "beamline/CLS/CLSMAXvMotor.h"

#include "beamline/BioXAS/BioXASSideMonochromatorControl.h"

class BioXASSideMonochromator : public QObject
{
    Q_OBJECT

public:
    /// Enumerates the possible mono region states.
    enum Region { None = 0, A, B };
    /// Constructor.
    explicit BioXASSideMonochromator(QObject *parent = 0);
    /// Destructor.
    virtual ~BioXASSideMonochromator();

    /// Returns true if the mono is connected to all of its pvs, false otherwise.
    bool isConnected() const { return connected_; }
    /// Returns the current region.
    Region region() const { return region_; }

    /// Returns true if both upper and lower slits are closed, false otherwise.
    bool slitsClosed() const { return ((int)slitsClosed_->value() == 1); }
    /// Returns true if the paddle is out, false otherwise.
    bool paddleOut() const { return ((int)paddleOut_->value() == 1); }
    /// Returns true if crystal change (key status) is enabled, false if disabled.
    bool crystalChangeEnabled() const { return ((int)crystalChangeEnabled_->value() == 1); }
    /// Returns true if the crystal stage is at the change position, false otherwise.
    bool atCrystalChangePosition() const { return ((int)stageAtCrystalChangePosition_->value() == 0); }
    /// Returns true if the brake is enabled, false otherwise.
    bool brakeEnabled() const { return ((int)crystalChangeBrakeEnabled_->value() == 1); }
    /// Returns true if the crystal change motor is at clockwise limit.
    bool crystalChangeMotorAtCWLimit() const { return ((int)crystalChangeMotorCWLimit_->value() == 0); }
    /// Returns true if the crystal change motor is at the counterclockwise limit.
    bool crystalChangeMotorAtCCWLimit() const { return ((int)crystalChangeMotorCCWLimit_->value() == 0); }
    /// Returns the energy setpoint.
    double energySetpoint() const { return energy_->setpoint(); }
    /// Returns the energy feedback.
    double energy() const { return energy_->value(); }

    /// Returns the phosphor paddle motor.
    CLSMAXvMotor* phosphorPaddleMotor() const { return phosphorPaddleMotor_; }
    /// Returns the bragg motor.
    CLSMAXvMotor* braggMotor() const { return braggMotor_; }
    /// Returns the vertical motor.
    CLSMAXvMotor* verticalMotor() const { return verticalMotor_; }
    /// Returns the lateral motor.
    CLSMAXvMotor* lateralMotor() const { return lateralMotor_; }
    /// Returns the crystal change motor.
    CLSMAXvMotor* crystalExchangeMotor() const { return crystalExchangeMotor_; }
    /// Returns the crystal 1 pitch motor.
    CLSMAXvMotor* crystal1PitchMotor() const { return crystal1PitchMotor_; }
    /// Returns the crystal 1 roll motor.
    CLSMAXvMotor* crystal1RollMotor() const { return crystal1RollMotor_; }
    /// Returns the crystal 2 pitch motor.
    CLSMAXvMotor* crystal2PitchMotor() const { return crystal2PitchMotor_; }
    /// Returns the crystal 2 roll motor.
    CLSMAXvMotor* crystal2RollMotor() const { return crystal2RollMotor_; }

    /// Returns the slitsClosed control.
    AMControl* slitsClosedControl() const { return slitsClosed_; }
    /// Returns the control for whether the paddle is completely removed.
    AMControl* paddleOutControl() const { return paddleOut_; }
    /// Returns the control for whether the crystal change operation is enabled.
    AMControl* crystalChangeEnabledControl() const { return crystalChangeEnabled_; }
    /// Returns the control for whether the mono is at the crystal change position.
    AMControl* crystalChangePositionStatusControl() const { return stageAtCrystalChangePosition_; }
    /// Returns the absolute crystal stage motor control.
    AMControl* crystalStageMotorAbsControl() const { return stageMotorAbs_; }
    /// Returns the relative crystal change motor control.
    AMControl* crystalStageMotorRelControl() const { return crystalChangeMotorRel_; }
    /// Returns the crystal stage clockwise limit control.
    AMControl* crystalStageCWLimitStatusControl() const { return crystalChangeMotorCWLimit_; }
    /// Returns the crystal stage counterclockwise limit control.
    AMControl* crystalStageCCWLimitStatusControl() const { return crystalChangeMotorCCWLimit_; }
    /// Returns the crystal change brake enabled control.
    AMControl* brakeEnabledStatusControl() const { return crystalChangeBrakeEnabled_; }
    /// Returns the region A status control.
    AMControl* regionAStatusControl() const { return regionAStatus_; }
    /// Returns the region B status control.
    AMControl* regionBStatusControl() const { return regionBStatus_; }
    /// Returns the energy setpoint control.
    AMControl* energyControl() const { return energy_; }

    /// Returns a new close slits action, 0 if not connected.
    AMAction3* createCloseSlitsAction();
    /// Returns a new remove paddle action, 0 if not connected.
    AMAction3* createRemovePaddleAction();
    /// Returns a new action that waits for the region key to be turned CCW to Disabled, 0 if not connected.
    AMAction3* createWaitForCrystalChangeEnabledAction();
    /// Returns a new action that sends the crystal motor to the change position.
    AMAction3* createMoveToCrystalChangePositionAction();
    /// Returns a new action that waits for the brake to be disabled, 0 if not connected.
    AMAction3* createWaitForBrakeDisabledAction();
    /// Returns a new action that moves the crystal change motor by the given degrees (relative).
    AMAction3* createMoveCrystalChangeMotorAction(int relDestination);
    /// Returns a new action that waits for the brake to be turned on, 0 if not connected.
    AMAction3* createWaitForBrakeEnabledAction();
    /// Returns a new action that sets the crystal stage to the given absolute destination angle.
    AMAction3* createMoveStageAction(double degDestination);
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
    void regionChanged(BioXASSideMonochromator::Region newRegion);
    /// Notifier that the slits closed general status has changed.
    void slitsClosedChanged(bool areClosed);
    /// Notifier that the paddle out status has changed.
    void paddleOutChanged(bool isOut);
    /// Notifier that crystal changes are enabled/disabled.
    void crystalChangeEnabledChanged(bool isEnabled);
    /// Notifier that the crystal change position status has changed.
    void crystalChangePositionStatusChanged(bool atPosition);
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
    /// Emits the appropriate signal when the mono's slits closed status has changed.
    void onSlitsClosedChanged(double value) { emit slitsClosedChanged((int)value == 1); }
    /// Emits the appropriate signal when the paddle is out.
    void onPaddleOutChanged(double value) { emit paddleOutChanged((int)value == 1); }
    /// Emits the appropriate signal when the crystal change motor is enabled/disabled.
    void onCrystalChangeEnabled(double value) { emit crystalChangeEnabledChanged((int)value == 1); }
    /// Emits the appropriate signal when the crystal change position status has changed.
    void onCrystalChangePositionStatusChanged(double value) { emit crystalChangePositionStatusChanged((int)value == 0); }
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

    // Motors

    CLSMAXvMotor *phosphorPaddleMotor_;
    CLSMAXvMotor *braggMotor_;
    CLSMAXvMotor *verticalMotor_;
    CLSMAXvMotor *lateralMotor_;
    CLSMAXvMotor *crystalExchangeMotor_;
    CLSMAXvMotor *crystal1PitchMotor_;
    CLSMAXvMotor *crystal1RollMotor_;
    CLSMAXvMotor *crystal2PitchMotor_;
    CLSMAXvMotor *crystal2RollMotor_;

    // Controls

    AMControl* braggMotorPower_;
    AMControl* slitsClosed_;
    AMControl* paddleOut_;
    AMControl* crystalChangeEnabled_;
    AMControl* stageAtCrystalChangePosition_;
    AMControl* crystalChangeBrakeEnabled_;
    AMControl* stageMotorAbs_;
    AMControl* crystalChangeMotorRel_;
    AMControl* crystalChangeMotorCWLimit_;
    AMControl* crystalChangeMotorCCWLimit_;
    AMControl* regionAStatus_;
    AMControl* regionBStatus_;
    AMControl* energy_;
};

#endif // BIOXASSIDEMONOCHROMATOR_H
