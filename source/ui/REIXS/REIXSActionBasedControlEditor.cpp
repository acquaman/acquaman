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


#include "REIXSActionBasedControlEditor.h"

#include "beamline/AMControl.h"
#include "actions3/AMAction3.h"
#include "actions3/AMActionSupport.h"
#include "util/AMErrorMonitor.h"


REIXSActionBasedControlEditor::REIXSActionBasedControlEditor(AMControl* control, bool okToRunInBackground, QWidget *parent) :
	AMControlEditor(control, 0, parent)
{
	okToRunInBackground_ = okToRunInBackground;
}

REIXSActionBasedControlEditor::~REIXSActionBasedControlEditor(){}

void REIXSActionBasedControlEditor::onNewSetpointChosen(double value)
{
	if(!control_)
		return;

	AMAction3* action = AMActionSupport::buildControlMoveAction(control_, value);
	connect(action, SIGNAL(succeeded()), action, SLOT(deleteLater()));
	connect(action, SIGNAL(failed()), action, SLOT(deleteLater()));
	connect(action, SIGNAL(cancelled()), action, SLOT(deleteLater()));
	action->start();
}
