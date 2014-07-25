#ifndef BIOXASSIDEMONOCHROMATOR_H
#define BIOXASSIDEMONOCHROMATOR_H

#include <QObject>

#include "beamline/AMControl.h"
#include "beamline/AMPVControl.h"
#include "beamline/CLS/CLSBiStateControl.h"

class BioXASSideMonochromator : public QObject
{
    Q_OBJECT

public:    
    /// Constructor.
    explicit BioXASSideMonochromator(QObject *parent = 0);
    /// Destructor.
    virtual ~BioXASSideMonochromator();

protected:
    AMProcessVariable *tankClosed_;
    AMProcessVariable *brakeOff_;
    AMProcessVariable *keyStatus_;
    AMProcessVariable *paddleExtracted_;
    AMProcessVariable *crystalChangePosition_;
    AMProcessVariable *upperSlitClosed_;
    AMProcessVariable *lowerSlitClosed_;
    AMProcessVariable *slitsClosed_;
    AMProcessVariable *thetaACWLimit_;
    AMProcessVariable *thetaACCWLimit_;
    AMProcessVariable *thetaBCWLimit_;
    AMProcessVariable *thetaBCCWLimit_;
    AMProcessVariable *regionA_;
    AMProcessVariable *regionB_;
    AMProcessVariable *translateALimit_;
    AMProcessVariable *translateBLimit_;
    AMProcessVariable *translatePermitted_;
    AMProcessVariable *thetaPaddlePermitted_;
    AMProcessVariable *slitsCloseCmd_;
    AMProcessVariable *slitsEnabled_;


};

#endif // BIOXASSIDEMONOCHROMATOR_H
