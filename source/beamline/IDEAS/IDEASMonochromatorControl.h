#ifndef IDEASMONOCHROMATORCONTROL_H
#define IDEASMONOCHROMATORCONTROL_H

#include "beamline/AMPVControl.h"

class IDEASMonochromatorControl : public AMPVwStatusControl
{
    Q_OBJECT

public:
    /// Constructor.  Builds all the important aspects of the mono for the IDEAS beamline.
    IDEASMonochromatorControl(QObject *parent = 0);

public slots:
    /// Re-implemented move method.
    AMControl::FailureExplanation move(double setpoint);
protected slots:
    /// Handles setting the units of the control after all the controls have initialized.
    void onAllControlsConnected();

//private:
//    double lastPositiveMove;
};

#endif // IDEASMONOCHROMATORCONTROL_H

