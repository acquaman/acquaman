#ifndef BIOXASSIDEMONOCHROMATOR_H
#define BIOXASSIDEMONOCHROMATOR_H

#include <QObject>

#include "beamline/BioXAS/BioXASSideMonochromatorControl.h"

#include "beamline/AMControl.h"
#include "beamline/AMPVControl.h"
#include "actions3/AMAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "actions3/actions/AMControlWaitAction.h"
#include "actions3/AMListAction3.h"

class BioXASSideMonochromator : public QObject
{
    Q_OBJECT

public:    
    /// Constructor.
    explicit BioXASSideMonochromator(QObject *parent = 0);
    /// Destructor.
    virtual ~BioXASSideMonochromator();

    /// Returns true if the mono is connected to all of its pvs, false otherwise.
    bool isConnected() const { return connected_; }

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
    /// Returns true if the crystal stage motor is at clockwise limit.
    bool crystalStageAtCWLimit() const { return ((int)crystalChangeMotorCWLimit_->value() == 0); }
    /// Returns true if the crystal stage motor is at the counterclockwise limit.
    bool crystalStageAtCCWLimit() const { return ((int)crystalChangeMotorCCWLimit_->value() == 0); }
    /// Returns true if the mono is in region A, false otherwise.
    bool inRegionA() const { return ((int)regionAStatus_->value() == 1); }
    /// Returns true if the mono is in region B, false otherwise.
    bool inRegionB() const { return ((int)regionBStatus_->value() == 1); }
    /// Returns the energy setpoint.
    double energySetpoint() const { return energy_->setpoint(); }
    /// Returns the energy feedback.
    double energy() const { return energy_->value(); }

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
    /// Returns a new crystal change action, a list of wait actions. 0 if not connected.
    AMAction3* createCrystalChangeAction();
    /// Returns a new set energy action, 0 if not connected.
    AMAction3* createSetEnergyAction(double newEnergy);

signals:
    /// Notifier that the mono's connections with its pvs have changed.
    void connected(bool isConnected);
    /// Notifier that the slits closed general status has changed.
    void slitsClosedChanged(bool areClosed);
    /// Notifier that the paddle out status has changed.
    void paddleOutChanged(bool isOut);
    /// Notifier that the key status has changed.
    void crystalChangeEnabledChanged(bool isEnabled);
    /// Notifier that the crystal change position status has changed.
    void crystalChangePositionStatusChanged(bool atPosition);
    /// Notifier that the brake enabled status has changed.
    void crystalChangeBrakeEnabledChanged(bool isEnabled);
    /// Notifier that the crystal stage clockwise limit status has changed.
    void crystalChangeMotorCWLimitStatusChanged(bool atLimit);
    /// Notifier that the crystal stage counterclockwise limit status has changed.
    void crystalChangeMotorCCWLimitStatusChanged(bool atLimit);
    /// Notifier that the region A status has changed.
    void regionAStatusChanged(bool inRegion);
    /// Notifier that the region B status has changed.
    void regionBStatusChanged(bool inRegion);
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
    /// Emits the appropriate signal when the mono reaches/leaves region A.
    void onRegionAStatusChanged(double value) { emit regionAStatusChanged((int)value == 1); }
    /// Emits the appropriate signal when the mono reaches/leaves region B.
    void onRegionBStatusChanged(double value) { emit regionBStatusChanged((int)value == 1); }

protected:
    bool connected_;

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
