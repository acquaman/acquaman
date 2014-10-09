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
    /// Returns true if the key status is enabled, false if disabled.
    bool keyStatusEnabled() const { return ((int)keyStatus_->value() == 1); }
    /// Returns true if the crystal stage is at the change position, false otherwise.
    bool atCrystalChangePosition() const { return ((int)crystalChangePositionStatus_->value() == 0); }
    /// Returns true if the brake is enabled, false otherwise.
    bool brakeEnabled() const { return ((int)brakeEnabledStatus_->value() == 1); }
    /// Returns true if the crystal stage motor is at clockwise limit.
    bool crystalStageAtCWLimit() const { return ((int)crystalStageCWLimitStatus_->value() == 0); }
    /// Returns true if the crystal stage motor is at the counterclockwise limit.
    bool crystalStageAtCCWLimit() const { return ((int)crystalStageCCWLimitStatus_->value() == 0); }
    /// Returns true if the mono is in region A, false otherwise.
    bool inRegionA() const { return ((int)regionAStatus_->value() == 1); }
    /// Returns true if the mono is in region B, false otherwise.
    double inRegionB() const { return ((int)regionBStatus_->value() == 1); }
    /// Returns the energy setpoint.
    double energySetpoint() const { return energy_->setpoint(); }
    /// Returns the energy feedback.
    double energy() const { return energy_->value(); }

    /// Returns the slitsClosed control.
    AMControl* slitsClosedControl() const { return slitsClosed_; }
    /// Returns the paddleOut control.
    AMControl* paddleOutControl() const { return paddleOut_; }
    /// Returns the keyStatus control.
    AMControl* keyStatusControl() const { return keyStatus_; }
    /// Returns the crystal change position status control.
    AMControl* crystalChangePositionStatusControl() const { return crystalChangePositionStatus_; }
    /// Returns the absolute crystal stage motor control.
    AMControl* crystalStageMotorAbsControl() const { return crystalStageMotorAbs_; }
    /// Returns the relative crystal stage motor control.
    AMControl* crystalStageMotorRelControl() const { return crystalStageMotorRel_; }
    /// Returns the crystal stage clockwise limit control.
    AMControl* crystalStageCWLimitStatusControl() const { return crystalStageCWLimitStatus_; }
    /// Returns the crystal stage counterclockwise limit control.
    AMControl* crystalStageCCWLimitStatusControl() const { return crystalStageCCWLimitStatus_; }
    /// Returns the brake enabled control.
    AMControl* brakeEnabledStatusControl() const { return brakeEnabledStatus_; }
    /// Returns the region A status control.
    AMControl* regionAStatusControl() const { return regionAStatus_; }
    /// Returns the region B status control.
    AMControl* regionBStatusControl() const { return regionBStatus_; }
    /// Returns the energy setpoint control.
    AMControl* energyControl() const { return energy_; }

    /// Returns a new close slits action, 0 if not connected.
    AMAction3* createCloseSlitsAction();
    /// Returns a new extract paddle action, 0 if not connected.
    AMAction3* createPaddleOutAction();
    /// Returns a new action that waits for the region key to be turned CCW to Disabled, 0 if not connected.
    AMAction3* createWaitForKeyStatusEnabledAction();
    /// Returns a new action that sends the crystal motor to the change position.
    AMAction3* createMoveToCrystalChangePositionAction();
    /// Returns a new action that waits for the brake to be turned off, 0 if not connected.
    AMAction3* createWaitForBrakeDisabledAction();
    /// Returns a new action that sends the crystal motor off the change position.
    AMAction3* createMoveOffCrystalChangePositionAction(int relDestination);
    /// Returns a new action that waits for the brake to be turned on, 0 if not connected.
    AMAction3* createWaitForBrakeEnabledAction();
    /// Returns a new action that sets the crystal stage to the given destination angle.
    AMAction3* createSetCrystalStageAngleAction(double degDestination);
    /// Returns a new action that waits for the region key to be turned CW to Enabled, if not connected.
    AMAction3* createWaitForKeyStatusDisabledAction();
    /// Returns a new crystal change action, a list of wait actions. 0 if not connected.
    AMAction3* createWaitForCrystalChangeAction();
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
    void keyStatusChanged(bool isEnabled);
    /// Notifier that the crystal change position status has changed.
    void crystalChangePositionStatusChanged(bool atPosition);
    /// Notifier that the brake enabled status has changed.
    void brakeEnabledStatusChanged(bool isEnabled);
    /// Notifier that the crystal stage clockwise limit status has changed.
    void crystalStageCWLimitStatusChanged(bool atLimit);
    /// Notifier that the crystal stage counterclockwise limit status has changed.
    void crystalStageCCWLimitStatusChanged(bool atLimit);
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
    void onConnectedChanged();

protected:
    bool connected_;

    AMControl* slitsClosed_;
    AMControl* paddleOut_;
    AMControl* keyStatus_;
    AMControl* crystalChangePositionStatus_;
    AMControl* brakeEnabledStatus_;
    AMControl* crystalStageMotorAbs_;
    AMControl* crystalStageMotorRel_;
    AMControl* crystalStageCWLimitStatus_;
    AMControl* crystalStageCCWLimitStatus_;
    AMControl* regionAStatus_;
    AMControl* regionBStatus_;
    AMControl* energy_;

};

#endif // BIOXASSIDEMONOCHROMATOR_H
