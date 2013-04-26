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


#ifndef AMCHANGERUNACTION_H
#define AMCHANGERUNACTION_H

#include "actions2/AMAction.h"
#include "actions2/actions/AMChangeRunActionInfo.h"

/// A workflow action to change the current run (to a new or existing run)
class AMChangeRunAction : public AMAction
{
    Q_OBJECT
public:
	/// Constructor: requires AMChangeRunActionInfo \c info.
	Q_INVOKABLE AMChangeRunAction(AMChangeRunActionInfo* info, QObject *parent = 0) : AMAction(info, parent) {}
	/// Copy constructor:
	AMChangeRunAction(const AMChangeRunAction& other) : AMAction(other) {}
	/// Virtual copy constructor:
	virtual AMAction* createCopy() const { return new AMChangeRunAction(*this); }

protected:
	// Re-implemented functions:
	/////////////////////////////

	/// Re-implemented to change the current run and report success.
	virtual void startImplementation();

};

#endif // AMCHANGERUNACTION_H
