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


#ifndef REIXSACTIONBASEDCONTROLEDITOR_H
#define REIXSACTIONBASEDCONTROLEDITOR_H

#include "ui/beamline/AMControlEditor.h"

/// Re-implements AMControlEditor to do its moves inside AMActionRunner with a REIXSControlMoveAction.
class REIXSActionBasedControlEditor : public AMControlEditor
{
    Q_OBJECT
public:
	/// Construct an editor for \c control. If \c okToRunInBackground is true, the actions will be run regardless of what's going on in the AMActionRunner queue with AMActionRunner::runActionImmediately. Otherwise, they will only be run when no other actions are currently running. (This is best for things that could mess up scans.)
 	virtual ~REIXSActionBasedControlEditor();
	REIXSActionBasedControlEditor(AMControl* control, bool okToRunInBackground = false, QWidget *parent = 0);

protected slots:
	/// Re-implemented from AMControlEditor
	virtual void onNewSetpointChosen(double value);

protected:
	bool okToRunInBackground_;
};

#endif // REIXSACTIONBASEDCONTROLEDITOR_H
