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
    /// Returns true if the tank is closed, false otherwise.
    double tankClosed() const { return tankClosed_->value(); }
    /// Returns true if the brake is off, false otherwise.
    double brakeOff() const { return brakeOff_->value(); }
    /// Returns true if the key status is enabled, false if disabled.
    double keyStatus() const { return keyStatus_->value(); }
    /// Returns true if the paddle is extracted, false otherwise.
    double paddleExtracted() const { return paddleExtracted_->value(); }
    /// Returns true if the mono is in the crystal change position, false otherwise.
    double atCrystalChangePosition() const { return crystalChangePosition_->value(); }
    /// Returns true if the upper slit is closed, false otherwise.
    double upperSlitClosed() const { return upperSlitClosed_->value(); }
    /// Returns true if the lower slit is closed, false otherwise.
    double lowerSlitClosed() const { return lowerSlitClosed_->value(); }
    /// Returns true if both upper and lower slits are closed, false otherwise.
    double slitsClosed() const { return slitsClosed_->value(); }
    /// Returns true if the mono is at the theta clockwise limit in region A, false otherwise.
    double atRegionAThetaCWLimit() const { return regionAThetaCWLimit_->value(); }
    /// Returns true if the mono is at the theta counter-clockwise limit in region A, false otherwise.
    double atRegionAThetaCCWLimit() const { return regionAThetaCCWLimit_->value(); }
    /// Returns true if the mono is at the theta clockwise limit in region B, false otherwise.
    double atRegionBThetaCWLimit() const { return regionBThetaCWLimit_->value(); }
    /// Returns true if the mono is at the theta counter-clockwise limit in region B, false otherwise.
    double atRegionBThetaCCWLimit() const { return regionBThetaCCWLimit_->value(); }
    /// Returns true if the mono is in region A, false otherwise.
    double inRegionA() const { return regionA_->value(); }
    /// Returns true if the mono is in region B, false otherwise.
    double inRegionB() const { return regionB_->value(); }
    /// Returns true if the mono is at the translation limit in region A, false otherwise.
    double atRegionATranslateLimit() const { return regionATranslateLimit_->value(); }
    /// Returns true if the mono is at the translation limit in region B, false otherwise.
    double atTranslateBLimit() const { return regionBTranslateLimit_->value(); }
    /// Returns true if translation is permitted, false otherwise.
    double translatePermitted() const { return translatePermitted_->value(); }
    /// Returns true if the theta paddle is permitted, false otherwise.
    double thetaPaddlePermitted() const { return thetaPaddlePermitted_->value(); }
    /// Returns true if the slits are enabled, false otherwise.
    double slitsEnabled() const { return slitsEnabled_->value(); }
    /// Returns the energy setpoint.
    double energySetpoint() const { return energy_->setpoint(); }
    /// Returns the energy feedback.
    double energy() const { return energy_->value(); }

    /// Returns the tankClosed control.
    AMControl* tankClosedControl() const { return tankClosed_; }
    /// Returns the brakeOff control.
    AMControl* brakeOffControl() const { return brakeOff_; }
    /// Returns the keyStatus control.
    AMControl* keyStatusControl() const { return keyStatus_; }
    /// Returns the paddleExtracted control.
    AMControl* paddleExtractedControl() const { return paddleExtracted_; }
    /// Returns the crystalChangePosition control.
    AMControl* crystalChangePositionControl() const { return crystalChangePosition_; }
    /// Returns the upperSlitClosed control.
    AMControl* upperSlitClosedControl() const { return upperSlitClosed_; }
    /// Returns the lowerSlitClosed control.
    AMControl* lowerSlitClosedControl() const { return lowerSlitClosed_; }
    /// Returns the slitsClosed control.
    AMControl* slitsClosedControl() const { return slitsClosed_; }
    /// Returns the thetaACWLimit control.
    AMControl* thetaACWLimitControl() const { return regionAThetaCWLimit_; }
    /// Returns the thetaACCWLimit control.
    AMControl* thetaACCWLimitControl() const { return regionAThetaCCWLimit_; }
    /// Returns the thetaBCWLimit control.
    AMControl* thetaBCWLimitControl() const { return regionBThetaCWLimit_; }
    /// Returns the thetaBCCWLimit control.
    AMControl* thetaBCCWLimitControl() const { return regionBThetaCCWLimit_; }
    /// Returns the regionA control.
    AMControl* regionAControl() const { return regionA_; }
    /// Returns the regionB control.
    AMControl* regionBControl() const { return regionB_; }
    /// Returns the translateALimit control.
    AMControl* translateALimitControl() const { return regionATranslateLimit_; }
    /// Returns the translateBLimit control.
    AMControl* translateBLimitControl() const { return regionBTranslateLimit_; }
    /// Returns the translatePermitted control.
    AMControl* translatePermittedControl() const { return translatePermitted_; }
    /// Returns the thetaPaddlePermitted control.
    AMControl* thetaPaddlePermittedControl() const { return thetaPaddlePermitted_; }
    /// Returns the slitsEnabled control.
    AMControl* slitsEnabledControl() const { return slitsEnabled_; }
    /// Returns the energy setpoint control.
    AMControl* energyControl() const { return energy_; }

    /// Returns a new close slits action, 0 if not connected.
    AMAction3* createCloseSlitsAction();
    /// Returns a new action that waits for the brake to be turned off, 0 if not connected.
    AMAction3* createWaitForBrakeOffAction();
    /// Returns a new action that waits for the brake to be turned on, 0 if not connected.
    AMAction3* createWaitForBrakeOnAction();
    /// Returns a new action that waits for the mono to move to crystal change position, 0 if not connected.
    AMAction3* createWaitForMoveToCrystalChangePositionAction();
    /// Returns a new action that waits for the mono to leave the crystal change position, 0 if not connected.
    AMAction3* createWaitForMoveOffCrystalChangePositionAction();
    /// Returns a new action that waits for the region key to be turned CW, 0 if not connected.
    AMAction3* createWaitForKeyStatusCWAction();
    /// Returns a new action that waits for the region key to be turned CCW, 0 if not connected.
    AMAction3* createWaitForKeyStatusCCWAction();
    /// Returns a new crystal change action, a list of wait actions. 0 if not connected.
    AMAction3* createWaitForCrystalChangeAction();
    /// Returns a new set energy action, 0 if not connected.
    AMAction3* createSetEnergyAction(double newEnergy);

signals:
    /// Notifier that the mono's connections with its pvs have changed.
    void connected(bool isConnected);
    /// Notifier that the tankClosed status has changed.
    void tankClosedChanged(double newValue);
    /// Notifier that the brakeOff status has changed.
    void brakeStatusChanged(double newValue);
    /// Notifier that the key status has changed.
    void keyStatusChanged(double newValue);
    /// Notifier that the paddle extracted status has changed.
    void paddleExtractedChanged(double newValue);
    /// Notifier that the mono has reached/left the crystal change position.
    void crystalChangePosition(double newValue);
    /// Notifier that the upper slit closed status has changed.
    void upperSlitClosedChanged(double newValue);
    /// Notifier that the lower slit closed status has changed.
    void lowerSlitClosedChanged(double newValue);
    /// Notifier that the slits closed general status has changed.
    void slitsClosedChanged(double newValue);
    /// Notifier that the region A theta clockwise limit has been reached/left.
    void regionAThetaCWLimit(double newValue);
    /// Notifier that the region A theta counter-clockwise limit has been reached/left.
    void regionAThetaCCWLimit(double newValue);
    /// Notifier that the region B theta clockwise limit has been reached/left.
    void regionBThetaCWLimit(double newValue);
    /// Notifier that the region B theta counter-clockwise limit has been reached/left.
    void regionBThetaCCWLimit(double newValue);
    /// Notifier that the mono is now in, or just left, region A.
    void regionA(double newValue);
    /// Notifier that the mono is now in, or just left, region B.
    void regionB(double newValue);
    /// Notifier that the region A translation limit has been reached/left.
    void regionATranslateLimit(double newValue);
    /// Notifier that the region B translation limit has been reached/left.
    void regionBTranslateLimit(double newValue);
    /// Notifier that the translation is/is not permitted.
    void translatePermitted(double newValue);
    /// Notifier that using the theta paddle is / is not permitted.
    void thetaPaddlePermitted(double newValue);
    /// Notifier that the slit controls are en/disabled.
    void slitsEnabled(double newValue);
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

    AMControl* tankClosed_;
    AMControl* brakeOff_;
    AMControl* keyStatus_;
    AMControl* paddleExtracted_;
    AMControl* crystalChangePosition_;
    AMControl* upperSlitClosed_;
    AMControl* lowerSlitClosed_;
    AMControl* slitsClosed_;
    AMControl* regionAThetaCWLimit_;
    AMControl* regionAThetaCCWLimit_;
    AMControl* regionBThetaCWLimit_;
    AMControl* regionBThetaCCWLimit_;
    AMControl* regionA_;
    AMControl* regionB_;
    AMControl* regionATranslateLimit_;
    AMControl* regionBTranslateLimit_;
    AMControl* translatePermitted_;
    AMControl* thetaPaddlePermitted_;
    AMControl* slitsCloseCmd_;
    AMControl* slitsEnabled_;
    AMControl* energy_;

};

#endif // BIOXASSIDEMONOCHROMATOR_H
