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


#include "AMCompositeControl.h"

 AMCompositeControl::~AMCompositeControl(){}
AMCompositeControl::AMCompositeControl(const QString& name, const QString& units, QObject* parent, const QString description) :
	AMControl(name, units, parent, description)
{
	wasConnected_ = false;
	wasMoving_ = false;
}

bool AMCompositeControl::addChildControl(AMControl *control)
{
        if(AMControl::addChildControl(control)){
            connect(control, SIGNAL(connected(bool)), this, SLOT(onChildControlConnected()));
            connect(control, SIGNAL(movingChanged(bool)), this, SLOT(onChildControlMovingChanged()));
        }
}

void AMCompositeControl::onChildControlConnected()
{
	bool nowConnected = isConnected();

	// Losing the connection:
	if(wasConnected_ && !nowConnected ) {
		emit connected(wasConnected_ = nowConnected);
	}

	// gaining the connection:
	if(!wasConnected_ && nowConnected ) {
		emit connected(wasConnected_ = nowConnected);
	}
}

void AMCompositeControl::onChildControlMovingChanged()
{
	bool nowMoving = isMoving();

	if(wasMoving_ && !nowMoving)
		emit movingChanged(wasMoving_ = nowMoving);

	if(!wasMoving_ && nowMoving)
		emit movingChanged(wasMoving_ = nowMoving);
}
