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

#include "AM4DMotorGroup.h"

AM4DMotorGroupObject::AM4DMotorGroupObject(const QString &name, const QString &prefix, const QString &units, AMControl *control, Orientation orientation, MotionType motionType, QObject *parent) :
	AMMotorGroupObject(name, prefix, units, control, orientation, motionType, parent)
{

}

AM4DMotorGroupObject::AM4DMotorGroupObject(const QString &name, const QStringList &prefixes, const QStringList &units, const QList<AMControl *> controls, QList<Orientation> orientations, QList<MotionType> motionTypes, QObject *parent) :
	AMMotorGroupObject(name, prefixes, units, controls, orientations, motionTypes, parent)
{

}

#include "ui/AM4DMotorGroupView.h"
AMMotorGroupObjectView* AM4DMotorGroupObject::createMotorGroupObjectView(){
	AM4DMotorGroupObjectView *retVal = new AM4DMotorGroupObjectView(this);
	return retVal;
}
