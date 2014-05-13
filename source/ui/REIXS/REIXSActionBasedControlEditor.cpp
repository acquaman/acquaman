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


#include "REIXSActionBasedControlEditor.h"
#include "util/AMErrorMonitor.h"
#include "beamline/AMControl.h"
#include "actions3/AMAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"

#include <QApplication>

 REIXSActionBasedControlEditor::~REIXSActionBasedControlEditor(){}
REIXSActionBasedControlEditor::REIXSActionBasedControlEditor(AMControl* control, bool okToRunInBackground, QWidget *parent) :
	AMControlEditor(control, parent)
{
	okToRunInBackground_ = okToRunInBackground;
}

void REIXSActionBasedControlEditor::onNewSetpointChosen(double value)
{
	if(!control_)
		return;

	AMControlInfo setpoint = control_->toInfo();
	setpoint.setValue(value);

	if(okToRunInBackground_) {
		AMAction3* action = new AMControlMoveAction3(new AMControlMoveActionInfo3(setpoint), control_);
		connect(action, SIGNAL(succeeded()), action, SLOT(deleteLater()));
		connect(action, SIGNAL(failed()), action, SLOT(deleteLater()));
		connect(action, SIGNAL(cancelled()), action, SLOT(deleteLater()));
		action->start();
	}
	else {

		AMAction3* action = new AMControlMoveAction3(new AMControlMoveActionInfo3(setpoint), control_);
		connect(action, SIGNAL(succeeded()), action, SLOT(deleteLater()));
		connect(action, SIGNAL(failed()), action, SLOT(deleteLater()));
		connect(action, SIGNAL(cancelled()), action, SLOT(deleteLater()));
		action->start();
	}
}
