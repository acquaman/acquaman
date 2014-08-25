#ifndef BIOXASSIDEMONOCHROMATOR_H
#define BIOXASSIDEMONOCHROMATOR_H

#include <QObject>

#include "beamline/AMControl.h"
#include "beamline/AMPVControl.h"
#include "actions3/AMAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"

class BioXASSideMonochromator : public QObject
{
    Q_OBJECT

public:    
    /// Constructor.
    explicit BioXASSideMonochromator(QObject *parent = 0);
    /// Destructor.
    virtual ~BioXASSideMonochromator();

    /// Returns true if the tank is closed, false otherwise.
    bool tankClosed() const;
    /// Returns true if the brake is off, false otherwise.
    bool brakeOff() const;
    /// Returns true if the key status is enabled, false if disabled.
    bool keyStatus() const;
    /// Returns true if the paddle is extracted, false otherwise.
    bool paddleExtracted() const;
    /// Returns true if the mono is in the crystal change position, false otherwise.
    bool crystalChangePosition() const;
    /// Returns true if the upper slit is closed, false otherwise.
    bool upperSlitClosed() const;
    /// Returns true if the lower slit is closed, false otherwise.
    bool lowerSlitClosed() const;
    /// Returns true if both upper and lower slits are closed, false otherwise.
    bool slitsClosed() const;
    /// Returns true if the mono is at the theta clockwise limit in region A, false otherwise.
    bool thetaACWLimit() const;
    /// Returns true if the mono is at the theta counter-clockwise limit in region A, false otherwise.
    bool thetaACCWLimit() const;
    /// Returns true if the mono is at the theta clockwise limit in region B, false otherwise.
    bool thetaBCWLimit() const;
    /// Returns true if the mono is at the theta counter-clockwise limit in region B, false otherwise.
    bool thetaBCCWLimit() const;
    /// Returns true if the mono is in region A, false otherwise.
    bool regionA() const;
    /// Returns true if the mono is in region B, false otherwise.
    bool regionB() const;
    /// Returns true if the mono is at the translation limit in region A, false otherwise.
    bool translateALimit() const;
    /// Returns true if the mono is at the translation limit in region B, false otherwise.
    bool translateBLimit() const;
    /// Returns true if translation is permitted, false otherwise.
    bool translatePermitted() const;
    /// Returns true if the theta paddle is permitted, false otherwise.
    bool thetaPaddlePermitted() const;
    /// Returns true if the slits are enabled, false otherwise.
    bool slitsEnabled() const;

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
    AMControl* thetaACWLimitControl() const { return thetaACWLimit_; }
    /// Returns the thetaACCWLimit control.
    AMControl* thetaACCWLimitControl() const { return thetaACCWLimit_; }
    /// Returns the thetaBCWLimit control.
    AMControl* thetaBCWLimitControl() const { return thetaBCWLimit_; }
    /// Returns the thetaBCCWLimit control.
    AMControl* thetaBCCWLimitControl() const { return thetaBCCWLimit_; }
    /// Returns the regionA control.
    AMControl* regionAControl() const { return regionA_; }
    /// Returns the regionB control.
    AMControl* regionBControl() const { return regionB_; }
    /// Returns the translateALimit control.
    AMControl* translateALimitControl() const { return translateALimit_; }
    /// Returns the translateBLimit control.
    AMControl* translateBLimitControl() const { return translateBLimit_; }
    /// Returns the translatePermitted control.
    AMControl* translatePermittedControl() const { return translatePermitted_; }
    /// Returns the thetaPaddlePermitted control.
    AMControl* thetaPaddlePermittedControl() const { return thetaPaddlePermitted_; }
    /// Returns the slitsEnabled control.
    AMControl* slitsEnabledControl() const { return slitsEnabled_; }

    /// Returns a new close slits action, 0 if not connected.
    AMAction3* createCloseSlitsAction();

    /// Returns a new crystal change action, 0 if not connected.
    /// Possibly? Would depend on whether an action that waits for a signal exists/is buildable.

signals:
    /// Notifier that the tankClosed status has changed.
    void tankClosedStatusChanged(bool isClosed);
    /// Notifier that the brakeOff status has changed.
    void brakeOffStatusChanged(bool isOff);
    /// Notifier that the key status has changed.
    void keyStatusChanged(bool isEngaged);
    /// Notifier that the paddle extracted status has changed.
    void paddleExtractedStatusChanged(bool isExtracted);
    /// Notifier that the mono has reached/left the crystal change position.
    void crystalChangePositionStatusChanged(bool atPosition);
    /// Notifier that the upper slit closed status has changed.
    void upperSlitClosedStatusChanged(bool isClosed);
    /// Notifier that the lower slit closed status has changed.
    void lowerSlitClosedStatusChanged(bool isClosed);
    /// Notifier that the slits closed general status has changed.
    void slitsClosedStatusChanged(bool areClosed);
    /// Notifier that the region A theta clockwise limit has been reached/left.
    void thetaACWLimitStatusChanged(bool atLimit);
    /// Notifier that the region A theta counter-clockwise limit has been reached/left.
    void thetaACCWLimitStatusChanged(bool atLimit);
    /// Notifier that the region B theta clockwise limit has been reached/left.
    void thetaBCWLimitStatusChanged(bool atLimit);
    /// Notifier that the region B theta counter-clockwise limit has been reached/left.
    void thetaBCCWLimitStatusChanged(bool atLimit);
    /// Notifier that the mono is now in, or just left, region A.
    void regionAStatusChanged(bool withinRegion);
    /// Notifier that the mono is now in, or just left, region B.
    void regionBStatusChanged(bool withinRegion);
    /// Notifier that the region A translation limit has been reached/left.
    void translateALimitStatusChanged(bool atLimit);
    /// Notifier that the region B translation limit has been reached/left.
    void translateBLimitStatusChanged(bool atLimit);
    /// Notifier that the translation is/is not permitted.
    void translatePermittedStatusChanged(bool isPermitted);
    /// Notifier that using the theta paddle is / is not permitted.
    void thetaPaddlePermittedStatusChanged(bool isPermitted);
    /// Notifier that the slit controls are en/disabled.
    void slitsEnabledStatusChanged(bool isEnabled);

protected slots:
    void onTankClosedChanged(double newValue);
    void onBrakeOffChanged(double newValue);
    void onKeyStatusChanged(double newValue);
    void onPaddleExtractedChanged(double newValue);
    void onCrystalChangePositionChanged(double newValue);
    void onUpperSlitClosedChanged(double newValue);
    void onLowerSlitClosedChanged(double newValue);
    void onSlitsClosedChanged(double newValue);
    void onThetaACWLimitChanged(double newValue);
    void onThetaACCWLimitChanged(double newValue);
    void onThetaBCWLimitChanged(double newValue);
    void onThetaBCCWLimitChanged(double newValue);
    void onRegionAChanged(double newValue);
    void onRegionBChanged(double newValue);
    void onTranslateALimitChanged(double newValue);
    void onTranslateBLimitChanged(double newValue);
    void onTranslatePermittedChanged(double newValue);
    void onThetaPaddlePermittedChanged(double newValue);
    void onSlitsEnabledChanged(double newValue);

protected:
    AMReadOnlyPVControl *tankClosed_;
    AMReadOnlyPVControl *brakeOff_;
    AMReadOnlyPVControl *keyStatus_;
    AMReadOnlyPVControl *paddleExtracted_;
    AMReadOnlyPVControl *crystalChangePosition_;
    AMReadOnlyPVControl *upperSlitClosed_;
    AMReadOnlyPVControl *lowerSlitClosed_;
    AMReadOnlyPVControl *slitsClosed_;
    AMReadOnlyPVControl *thetaACWLimit_;
    AMReadOnlyPVControl *thetaACCWLimit_;
    AMReadOnlyPVControl *thetaBCWLimit_;
    AMReadOnlyPVControl *thetaBCCWLimit_;
    AMReadOnlyPVControl *regionA_;
    AMReadOnlyPVControl *regionB_;
    AMReadOnlyPVControl *translateALimit_;
    AMReadOnlyPVControl *translateBLimit_;
    AMReadOnlyPVControl *translatePermitted_;
    AMReadOnlyPVControl *thetaPaddlePermitted_;
    AMSinglePVControl *slitsCloseCmd_;
    AMReadOnlyPVControl *slitsEnabled_;


};

#endif // BIOXASSIDEMONOCHROMATOR_H
