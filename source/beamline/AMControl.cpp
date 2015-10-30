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


#include "AMControl.h"

 AMControl::~AMControl(){}
AMControl::AMControl(const QString& name, const QString& units, QObject* parent, const QString description) : QObject(parent), units_(units), description_(description)
{
	setObjectName(name);
	tolerance_ = AMCONTROL_TOLERANCE_DONT_CARE;
	attemptMoveWhenWithinTolerance_ = true;
	allowsMovesWhileMoving_ = false;
	displayPrecision_ = 3;
}

bool AMControl::hasChildControl(AMControl *control) const
{
	bool result = false;

	if (control && !children_.isEmpty()) {
		bool controlFound = false;

		for (int i = 0, count = children_.count(); i < count && !controlFound; i++) {
			AMControl *child = children_.at(i);

			if (child) {
				if (child == control)
					controlFound = true;
				else
					controlFound = child->hasChildControl(control);
			}
		}

		result = controlFound;
	}

	return result;
}
