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

#ifndef PGMMONOCHROMATORCONTROL_H
#define PGMMONOCHROMATORCONTROL_H

#include "beamline/AMPVControl.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"


class PGMMonochromatorControl : public AMPVwStatusControl
{
    Q_OBJECT
public:
    /// Constructor. Builds all the important aspects of the mono for the PGM beamline
    PGMMonochromatorControl(QObject *parent = 0);

public slots:
    /// Re-implemented move method.
    AMControl::FailureExplanation move(double setpoint);

protected slots:
    /// Handles setting the units of the control after all the controls have been initialized
    void onAllControlsConnected();
    /// Called when a move is in progress, and the move action fails or is cancelled.
    void onMoveActionsFailed();
    /// Called while a move is in progress, and the complete move action succeeded.
    void onMoveActionSucceeded();

private:
    /// Position of last positive move
    double lastPositiveMove_;
    /// Used to run the steps of a move. 0 if one of the moves is not in progress.
    AMListAction3 *moveAction_;
};


class PGMDirectMonochromatorControl : public AMPVwStatusControl
{
    Q_OBJECT

public:
    /// Constructor. Builds all the important aspects of the mono for the PGM beamline.
    PGMDirectMonochromatorControl(QObject *parent = 0);

public slots:
    /// Re-implemented move method.
    AMControl::FailureExplanation move(double setpoint);

protected slots:
    /// Handles setting the units of the control after all the controls have been initialized.
    void onAllControlsConnected();
};


#endif // PGMMONOCHROMATORCONTROL_H
