/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


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

