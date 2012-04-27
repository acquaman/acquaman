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


#ifndef REIXSMOVETOSAMPLETRANSFERPOSITIONACTION_H
#define REIXSMOVETOSAMPLETRANSFERPOSITIONACTION_H

#include "actions2/AMListAction.h"
#include "actions2/actions/REIXS/REIXSMoveToSampleTransferPositionActionInfo.h"

/// This action tries to safely move the REIXS sample manipulator to the sample transfer position.
class REIXSMoveToSampleTransferPositionAction : public AMListAction
{
    Q_OBJECT
public:
	/// Constructor, requires a REIXSMoveToSampleTransferPositionActionInfo
	Q_INVOKABLE REIXSMoveToSampleTransferPositionAction(REIXSMoveToSampleTransferPositionActionInfo* info = new REIXSMoveToSampleTransferPositionActionInfo(), QObject *parent = 0);
	/// Copy constructor
	REIXSMoveToSampleTransferPositionAction(const REIXSMoveToSampleTransferPositionAction& other) : AMListAction(other) {}
	/// Virtual copy constructor
	virtual AMAction* createCopy() const { return new REIXSMoveToSampleTransferPositionAction(*this); }



protected:
	/// Re-implemented from AMListAction to add a set of move actions
	virtual void startImplementation();

};

#endif // REIXSMOVETOSAMPLETRANSFERPOSITIONACTION_H
