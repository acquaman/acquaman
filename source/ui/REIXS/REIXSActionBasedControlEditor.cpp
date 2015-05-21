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
#include "util/AMErrorMonitor.h"
#include "beamline/AMControl.h"
#include "actions3/actions/AMControlMoveAction3.h"

#include <QApplication>

REIXSActionBasedControlEditor::REIXSActionBasedControlEditor(AMControl* control, bool okToRunInBackground, QWidget *parent) :
	AMControlEditor(control, 0, parent)
{
	okToRunInBackground_ = okToRunInBackground;
	controlMoveAction_ = 0;
}

REIXSActionBasedControlEditor::~REIXSActionBasedControlEditor(){}

void REIXSActionBasedControlEditor::onControlMoveActionFinished()
{
	if (controlMoveAction_) {
		disconnect(controlMoveAction_, SIGNAL(succeeded()));
		disconnect(controlMoveAction_, SIGNAL(failed()));
		disconnect(controlMoveAction_, SIGNAL(cancelled()));

		controlMoveAction_->deleteLater();
		controlMoveAction_ = 0;
	}
}

void REIXSActionBasedControlEditor::onNewSetpointChosen(double value)
{
	if(!control_ || controlMoveAction_)
		return;

	AMControlInfo setpoint = control_->toInfo();
	setpoint.setValue(value);

	controlMoveAction_ = new AMControlMoveAction3(new AMControlMoveActionInfo3(setpoint), control_);
	connect(controlMoveAction_, SIGNAL(succeeded()), this, SLOT(onControlMoveActionFinished()));
	connect(controlMoveAction_, SIGNAL(failed()), this, SLOT(onControlMoveActionFinished()));
	connect(controlMoveAction_, SIGNAL(cancelled()), this, SLOT(onControlMoveActionFinished()));
	controlMoveAction_->start();
}
