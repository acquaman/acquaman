/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "PIDLoopControl.h"

#include "beamline/AMBeamlineControlMoveAction.h"
#include "beamline/AMBeamlineParallelActionsList.h"
#include "beamline/AMBeamlineListAction.h"
#include "beamline/VESPERS/VESPERSBeamline.h"

PIDLoopControl::PIDLoopControl(QString name, AMControl *pidX, AMControl *pidY, AMControl *pidZ, QObject *parent)
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

AMBeamlineActionItem *PIDLoopControl::createPIDXChangeStateAction(bool turnOn)
{
	if (!x_->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(x_);
	action->setSetpoint(turnOn == true ? 1.0 : 0.0);

	return action;
}

AMBeamlineActionItem *PIDLoopControl::createPIDYChangeStateAction(bool turnOn)
{
	if (!y_->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(y_);
	action->setSetpoint(turnOn == true ? 1.0 : 0.0);

	return action;
}

AMBeamlineActionItem *PIDLoopControl::createPIDZChangeStateAction(bool turnOn)
{
	if (!z_->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(z_);
	action->setSetpoint(turnOn == true ? 1.0 : 0.0);

	return action;
}

AMBeamlineActionItem *PIDLoopControl::createPIDChangeStateAction(bool turnOn)
{
	if (!(x_->isConnected() && y_->isConnected() && z_->isConnected()))
		return 0;

	AMBeamlineParallelActionsList *turnOnAllActionsList = new AMBeamlineParallelActionsList;
	AMBeamlineListAction *turnOnAllAction = new AMBeamlineListAction(turnOnAllActionsList);

	turnOnAllActionsList->appendStage(new QList<AMBeamlineActionItem *>());
	turnOnAllActionsList->appendAction(0, createPIDXChangeStateAction(turnOn));
	turnOnAllActionsList->appendAction(0, createPIDYChangeStateAction(turnOn));
	turnOnAllActionsList->appendAction(0, createPIDZChangeStateAction(turnOn));

	return turnOnAllAction;
}

void PIDLoopControl::repair()
{
	if (VESPERSBeamline::vespers()->pseudoSampleStage()->isConnected() && !(x_->isConnected() && y_->isConnected() && z_->isConnected()))
		return;

	// Repairing the sample stage is a two stage process.
	/*
		First: Stop all the motors in the sample stage.  This is required because turning on the PID loop will have no effect if they are still moving.
		Second: Turn on all the PID loops.
	 */

	AMBeamlineParallelActionsList *repairActionsList = new AMBeamlineParallelActionsList;
	AMBeamlineListAction *repairAction = new AMBeamlineListAction(repairActionsList);

	repairActionsList->appendStage(new QList<AMBeamlineActionItem *>());
	repairActionsList->appendAction(0, VESPERSBeamline::vespers()->pseudoSampleStage()->createStopAllAction());

	repairActionsList->appendStage(new QList<AMBeamlineActionItem *>());
	repairActionsList->appendAction(1, createPIDChangeStateAction(true));

	connect(repairAction, SIGNAL(failed(int)), this, SLOT(onRepairFailed()));

	repairAction->start();
}
