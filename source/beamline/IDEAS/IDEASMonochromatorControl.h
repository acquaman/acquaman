#ifndef IDEASMONOCHROMATORCONTROL_H
#define IDEASMONOCHROMATORCONTROL_H

#include "beamline/AMPVControl.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"


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
    /// Called while one of our moves is in progress, and the move action fails or is cancelled.
    void onMoveActionFailed();
    /// Called while one of our moves is in progress, and the complete move action succeeded.
    void onMoveActionSucceeded();


private:
    double lastPositiveMove_;
    /// Used to run the steps of a move. 0 if one of our special moves is not in progress.
    AMListAction3 *moveAction_;


};







class IDEASDirectMonochromatorControl : public AMPVwStatusControl
{
    Q_OBJECT

public:
    /// Constructor.  Builds all the important aspects of the mono for the IDEAS beamline.
    IDEASDirectMonochromatorControl(QObject *parent = 0);

public slots:
    /// Re-implemented move method.
    AMControl::FailureExplanation move(double setpoint);
protected slots:
    /// Handles setting the units of the control after all the controls have initialized.
    void onAllControlsConnected();

};

#endif // IDEASMONOCHROMATORCONTROL_H

