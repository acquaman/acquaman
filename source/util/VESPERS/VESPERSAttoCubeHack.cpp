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


#include "VESPERSAttoCubeHack.h"
#include "beamline/AMPVControl.h"

 VESPERSAttoCubeHack::~VESPERSAttoCubeHack(){}
VESPERSAttoCubeHack::VESPERSAttoCubeHack(AMControl *Rz, QObject *parent)
	: QObject(parent)
{
	Rz_ = Rz;
	Rzfbk_ = new AMReadOnlyPVControl("RzFeedback", "SVM1607-2-B21-09:deg:fbk", this);
	connect(Rzfbk_, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged()));
}

void VESPERSAttoCubeHack::onValueChanged()
{
	if (Rz_->isMoving() && fabs(Rzfbk_->value()-Rz_->value()) < 0.01)
		Rz_->stop();
}
