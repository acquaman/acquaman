/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "PIDLoopControl.h"

PIDLoopControl::PIDLoopControl(QString name, AMControl *pidX, AMControl *pidY, AMControl *pidZ, QObject *parent)
	: QObject(parent)
{
	name_ = name;

	// Need read/write control.
	x_ = qobject_cast<AMPVControl *>(pidX);
	y_ = qobject_cast<AMPVControl *>(pidY);
	z_ = qobject_cast<AMPVControl *>(pidZ);

	connect(x_, SIGNAL(valueChanged(double)), this, SLOT(onXStateChanged()));
	connect(y_, SIGNAL(valueChanged(double)), this, SLOT(onYStateChanged()));
	connect(z_, SIGNAL(valueChanged(double)), this, SLOT(onZStateChanged()));
}
