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


#ifndef REIXSXESSCANACTION_H
#define REIXSXESSCANACTION_H

#include "actions2/actions/AMScanControllerAction.h"
#include "actions2/actions/REIXS/REIXSXESScanActionInfo.h"

/// This is a trivial subclass of AMScanControllerAction to allow registering it and REIXSXESScanActionInfo with AMActionRegistry. It enables storable, re-loadable REIXS XES Scans inside the action system.
class REIXSXESScanAction : public AMScanControllerAction
{
    Q_OBJECT
public:
	/// Constructor, as required by the registry system.
	Q_INVOKABLE REIXSXESScanAction(REIXSXESScanActionInfo* info, QObject *parent = 0) : AMScanControllerAction(info, parent) {}
	/// This convenience constructor creates the REIXSXESScanActionInfo for you.
	REIXSXESScanAction(REIXSXESScanConfiguration* scanConfig, QObject* parent = 0) : AMScanControllerAction(new REIXSXESScanActionInfo(scanConfig), parent) {}
	/// Copy Constructor
	REIXSXESScanAction(const REIXSXESScanAction& other) : AMScanControllerAction(other) {}
	/// Virtual copy constructor
	virtual AMAction* createCopy() const { return new REIXSXESScanAction(*this); }
};

#endif // REIXSXESSCANACTION_H
