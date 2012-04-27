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


#ifndef REIXSBEAMONOFFACTIONEDITOR_H
#define REIXSBEAMONOFFACTIONEDITOR_H

#include <QFrame>
#include "actions2/actions/REIXS/REIXSBeamOnOffActionInfo.h"

/// Provides an in-place editor for REIXS Beam on/off actions: lets you choose beam on or off.
class REIXSBeamOnOffActionEditor : public QFrame
{
    Q_OBJECT
public:
	/// Constructor: Must be Q_INVOKABLE, and accept an REIXSBeamOnOffActionInfo as argument
	Q_INVOKABLE REIXSBeamOnOffActionEditor(REIXSBeamOnOffActionInfo* info, QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Called when the beam on radio button is toggled, to set the info_.
	void onBeamOnRadioButtonToggled(bool beamOn);

protected:
	REIXSBeamOnOffActionInfo* info_;

};

#endif // REIXSBEAMONOFFACTIONEDITOR_H
