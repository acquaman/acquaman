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
    void tankClosedChanged(double);
    /// Notifier that the brakeOff status has changed.
    void brakeOffChanged(double);
    /// Notifier that the key status has changed.
    void keyStatusChanged(double);
    /// Notifier that the paddle extracted status has changed.
    void paddleExtractedChanged(double);

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
