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


#ifndef REIXSCONTROLMOVEACTION_H
#define REIXSCONTROLMOVEACTION_H

#include "actions2/actions/AMControlMoveAction.h"
#include "actions2/actions/REIXS/REIXSControlMoveActionInfo.h"

/// This subclass of AMControlMoveAction provides the required lookup of controls on the REIXS beamline at the CLS.
class REIXSControlMoveAction : public AMControlMoveAction
{
    Q_OBJECT
public:
	// Constructors and copying
	//////////////////////////////

	/// Constructor. Requires and takes ownership of an existing AMControlMoveActionInfo \c info.
	Q_INVOKABLE REIXSControlMoveAction(REIXSControlMoveActionInfo* info, QObject *parent = 0) : AMControlMoveAction(info, parent) {}
	/// Copy constructor: must re-implement, but can simply use the base class copy constructor.
	REIXSControlMoveAction(const REIXSControlMoveAction& other) : AMControlMoveAction(other) {}
	/// Virtual copy constructor
	virtual AMAction* createCopy() const { return new REIXSControlMoveAction(*this); }

	// Re-implemented virtual functions
	/////////////////////////

	/// Re-implemented to return a control from the REIXS beamline.
	AMControl* getControlForAction(const AMControlInfo &controlInfo) const;

};

#endif // REIXSCONTROLMOVEACTION_H
