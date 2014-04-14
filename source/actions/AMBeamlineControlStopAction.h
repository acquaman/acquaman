/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef AMBEAMLINECONTROLSTOPACTION_H
#define AMBEAMLINECONTROLSTOPACTION_H

#include "AMBeamlineActionItem.h"
#include "beamline/AMControl.h"

class AMBeamlineControlStopAction : public AMBeamlineActionItem
{
Q_OBJECT
public:
    AMBeamlineControlStopAction(AMControl *control, QObject *parent = 0);

    virtual AMBeamlineActionItemView* createView(int index = 0);

    virtual AMControl* control();

public slots:
    virtual void start();
    virtual void setControl(AMControl *control);
    virtual void cleanup(){}

protected slots:
    virtual void onMovingChanged(bool moving);
    virtual void onStopped();
    virtual void onFailed(int explanation);

protected:
	AMControl *control_;
};

#endif // AMBEAMLINECONTROLSTOPACTION_H