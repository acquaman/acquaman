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


#include "VESPERSPIDLoopControl.h"

#include "beamline/VESPERS/VESPERSBeamline.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "actions3/AMListAction3.h"

 VESPERSPIDLoopControl::~VESPERSPIDLoopControl(){}
VESPERSPIDLoopControl::VESPERSPIDLoopControl(QString name, AMControl *pidX, AMControl *pidY, AMControl *pidZ, QObject *parent)
	: QObject(parent)
{
	name_ = name;

	// Need read/write control.
	x_ = pidX;
	y_ = pidY;
	z_ = pidZ;

	connect(x_, SIGNAL(valueChanged(double)), this, SLOT(onXStateChanged()));
	connect(y_, SIGNAL(valueChanged(double)), this, SLOT(onYStateChanged()));
	connect(z_, SIGNAL(valueChanged(double)), this, SLOT(onZStateChanged()));
}

AMAction3 *VESPERSPIDLoopControl::createPIDXChangeStateAction(bool turnOn)
{
	if (!x_->isConnected())
		return 0;

	AMControlInfo setpoint = x_->toInfo();
	setpoint.setValue(turnOn ? 1.0 : 0.0);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, x_);

	return action;
}

AMAction3 *VESPERSPIDLoopControl::createPIDYChangeStateAction(bool turnOn)
{
	if (!y_->isConnected())
		return 0;

	AMControlInfo setpoint = y_->toInfo();
	setpoint.setValue(turnOn ? 1.0 : 0.0);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, y_);

	return action;
}

AMAction3 *VESPERSPIDLoopControl::createPIDZChangeStateAction(bool turnOn)
{
	if (!z_->isConnected())
		return 0;

	AMControlInfo setpoint = z_->toInfo();
	setpoint.setValue(turnOn ? 1.0 : 0.0);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, z_);

	return action;
}

AMAction3 *VESPERSPIDLoopControl::createPIDChangeStateAction(bool turnOn)
{
	if (!(x_->isConnected() && y_->isConnected() && z_->isConnected()))
		return 0;

	AMListAction3 *list = new AMListAction3(new AMListActionInfo3("PID repair all action.", "Action that repairs all three PID controls at once."), AMListAction3::Parallel);
	list->addSubAction(createPIDXChangeStateAction(turnOn));
	list->addSubAction(createPIDYChangeStateAction(turnOn));
	list->addSubAction(createPIDZChangeStateAction(turnOn));

	return list;
}

void VESPERSPIDLoopControl::repair()
{
	if (!(x_->isConnected() && y_->isConnected() && z_->isConnected()))
		return;

	// Repairing the sample stage is a two stage process.
	/*
		First: Stop all the motors in the sample stage.  This is required because turning on the PID loop will have no effect if they are still moving.
		Second: Turn on all the PID loops.
	 */

	AMListAction3 *list = new AMListAction3(new AMListActionInfo3("PID repair all action.", "Action that repairs all three PID controls at once."), AMListAction3::Sequential);
	list->addSubAction(VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->createStopAllAction());
	list->addSubAction(createPIDChangeStateAction(true));

	connect(list, SIGNAL(succeeded()), list, SLOT(deleteLater()));
	connect(list, SIGNAL(failed()), list, SLOT(deleteLater()));
	connect(list, SIGNAL(cancelled()), this, SLOT(deleteLater()));

	list->start();
}
