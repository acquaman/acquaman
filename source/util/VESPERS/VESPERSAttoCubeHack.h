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


#ifndef VESPERSATTOCUBEHACK_H
#define VESPERSATTOCUBEHACK_H

#include <QObject>

#include "beamline/AMControl.h"

/// This is a hack that detects when the Rz motor reaches it's designated point and then stops the motor.
class VESPERSAttoCubeHack : public QObject
{
	Q_OBJECT

public:
	/// Constructor.  Takes the control to the Rz motor.
 	virtual ~VESPERSAttoCubeHack();
	explicit VESPERSAttoCubeHack(AMControl *Rz, QObject *parent = 0);

signals:

public slots:

protected slots:
	/// Handles the value changed of the motor.
	void onValueChanged();

protected:
	/// Holds the control.
	AMControl *Rz_;
	/// Holds the feedback PV in particular.
	AMControl *Rzfbk_;
};

#endif // VESPERSATTOCUBEHACK_H
