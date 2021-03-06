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


#ifndef AMAXISVALUEFINISHEDACTIONINFO_H
#define AMAXISVALUEFINISHEDACTIONINFO_H

#include "actions3/AMActionInfo3.h"

class AMAxisValueFinishedActionInfo : public AMActionInfo3
{
	Q_OBJECT

public:
	/// Constructor.
	Q_INVOKABLE AMAxisValueFinishedActionInfo(QObject *parent = 0);
	/// Copy constructor.
	AMAxisValueFinishedActionInfo(const AMAxisValueFinishedActionInfo &other);
	/// Destructor.
	virtual ~AMAxisValueFinishedActionInfo(){}

	/// This function is used as a virtual copy constructor.
	virtual AMActionInfo3 *createCopy() const;

	/// Returns the description of the action.
	virtual QString typeDescription() const { return "Axis Value Finished"; }
};

#endif // AMAXISVALUEFINISHEDACTIONINFO_H
