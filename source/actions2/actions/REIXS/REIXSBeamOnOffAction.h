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


#ifndef REIXSBEAMONOFFACTION_H
#define REIXSBEAMONOFFACTION_H

#include "actions2/AMListAction.h"
#include "actions2/actions/REIXS/REIXSBeamOnOffActionInfo.h"

/// This subclass of AMListAction implements turning the beam on (All required valves and shutters), or off (Photon Shutter 4) on REIXS.
class REIXSBeamOnOffAction : public AMListAction
{
	Q_OBJECT
public:
	// Constructors and copying
	//////////////////////////////

	/// Constructor. Requires and takes ownership of an existing REIXSBeamOnOffActionInfo \c info; must be Q_INVOKABLE so we construct it dynamically in AMActionRegistry.
	Q_INVOKABLE REIXSBeamOnOffAction(REIXSBeamOnOffActionInfo* info, QObject *parent = 0) : AMListAction(info, AMListAction::SequentialMode, parent) {}
	/// Convenience constructor: you can specify \c beamOn (true) or beam off.
	REIXSBeamOnOffAction(bool beamOn, QObject* parent = 0) : AMListAction(new REIXSBeamOnOffActionInfo(beamOn), AMListAction::SequentialMode, parent) {}
	/// Copy constructor: must re-implement, but can simply use the base class copy constructor.
	REIXSBeamOnOffAction(const REIXSBeamOnOffAction& other) : AMListAction(other) {}
	/// Virtual copy constructor
	virtual AMAction* createCopy() const { return new REIXSBeamOnOffAction(*this); }


protected:
	// Re-implemented functions:
	////////////////////////////

	/// Re-implemented from AMListAction to add the move actions we need to accomplish this, depending on whether the info says to turn the beam on or off
	virtual void startImplementation();

};

#endif // REIXSBEAMONOFFACTION_H
