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


#include "REIXSActionBasedEnumControlEditor.h"
#include "actions2/actions/REIXS/REIXSControlMoveAction.h"
#include "actions2/AMActionRunner.h"
#include "util/AMErrorMonitor.h"
#include <QApplication>
#include "beamline/AMControl.h"

REIXSActionBasedEnumControlEditor::REIXSActionBasedEnumControlEditor(AMControl *control, bool okToRunInBackground, QWidget *parent) :
	AMBasicEnumControlEditor(control, parent)
{
	okToRunInBackground_ = okToRunInBackground;
}

void REIXSActionBasedEnumControlEditor::onNewEnumValueChosen(int value)
{
	if(!control_)
		return;

	AMControlInfo setpoint = control_->toInfo();
	setpoint.setValue(value);

	if(okToRunInBackground_) {
		AMAction* action = new REIXSControlMoveAction(new REIXSControlMoveActionInfo(setpoint));
		AMActionRunner::s()->runActionImmediately(action);
	}
	else {
		if(AMActionRunner::s()->actionRunning()) {
			QApplication::beep();
			// cannot run, because other actions are running in the queue.
			AMErrorMon::alert(this, -1, QString("Cannot move '%1' right now, because this would interfere with other actions that are currently running in the workflow. You can try again when they are finished, or pause the workflow queue.").arg(control_->description().isEmpty() ? control_->name() : control_->description()));
			return;
		}
		else {
			AMAction* action = new REIXSControlMoveAction(new REIXSControlMoveActionInfo(setpoint));
			AMActionRunner::s()->runActionImmediatelyInQueue(action);
		}
	}
}
