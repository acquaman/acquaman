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


#include "VESPERSSampleStageControl.h"

#include "actions/AMBeamlineControlMoveAction.h"
#include "actions/AMBeamlineControlStopAction.h"
#include "actions/AMBeamlineParallelActionsList.h"
#include "actions/AMBeamlineListAction.h"

VESPERSSampleStageControl::VESPERSSampleStageControl(AMControl *horiz, AMControl *vert, AMControl *norm, QObject *parent)
	: QObject(parent)
{
	connected_ = false;

	// The limits.
	xRange_ = qMakePair(0, 0);
	yRange_ = qMakePair(0, 0);
	zRange_ = qMakePair(0, 0);

	// The motor controls
	horiz_ = horiz;
	vert_ = vert;
	norm_ = norm;

	connect(horiz_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()));
	connect(vert_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()));
	connect(norm_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()));

	connect(horiz_, SIGNAL(movingChanged(bool)), this, SIGNAL(movingChanged(bool)));
	connect(vert_, SIGNAL(movingChanged(bool)), this, SIGNAL(movingChanged(bool)));
	connect(norm_, SIGNAL(movingChanged(bool)), this, SIGNAL(movingChanged(bool)));

	connect(horiz_, SIGNAL(setpointChanged(double)), this, SIGNAL(horizontalSetpointChanged(double)));
	connect(vert_, SIGNAL(setpointChanged(double)), this, SIGNAL(verticalSetpointChanged(double)));
	connect(norm_, SIGNAL(setpointChanged(double)), this, SIGNAL(normalSetpointChanged(double)));

	// If the controls are CLS VME motors, then use their connections to their steps.  Otherwise, it is a pseudomotor and we'll need to create those connections ourselves.
	CLSMAXvMotor *x = qobject_cast<CLSMAXvMotor *>(horiz_);
	CLSMAXvMotor *y = qobject_cast<CLSMAXvMotor *>(vert_);
	CLSMAXvMotor *z = qobject_cast<CLSMAXvMotor *>(norm_);

	if (x != 0 && y != 0 && z != 0){

		connect(x, SIGNAL(stepChanged(double)), this, SLOT(onXStepChanged(double)));
		connect(y, SIGNAL(stepChanged(double)), this, SLOT(onYStepChanged(double)));
		connect(z, SIGNAL(stepChanged(double)), this, SLOT(onZStepChanged(double)));
	}
	else{

		AMProcessVariable *xStep = new AMProcessVariable("SVM1607-2-B21-02:step:sp", true, this);
		AMProcessVariable *yStep = new AMProcessVariable("SVM1607-2-B21-03:step:sp", true, this);
		AMProcessVariable *zStep = new AMProcessVariable("SVM1607-2-B21-01:step:sp", true, this);

		connect(xStep, SIGNAL(valueChanged(double)), this, SLOT(onXStepChanged(double)));
		connect(yStep, SIGNAL(valueChanged(double)), this, SLOT(onYStepChanged(double)));
		connect(zStep, SIGNAL(valueChanged(double)), this, SLOT(onZStepChanged(double)));
	}
}

void VESPERSSampleStageControl::onXStepChanged(double step)
{
	if (step < xRange_.first){

		stopAll();
		emit horizontalMoveError(false);
	}
	else if (step > xRange_.second){

		stopAll();
		emit horizontalMoveError(true);
	}
}

void VESPERSSampleStageControl::onYStepChanged(double step)
{
	if (step < yRange_.first){

		stopAll();
		emit verticalMoveError(false);
		emit normalMoveError(false);
	}
	else if (step > yRange_.second){

		stopAll();
		emit verticalMoveError(true);
		emit normalMoveError(true);
	}
}

void VESPERSSampleStageControl::onZStepChanged(double step)
{
	if (step < zRange_.first){

		stopAll();
		emit verticalMoveError(false);
		emit normalMoveError(false);
	}
	else if (step > zRange_.second){

		stopAll();
		emit verticalMoveError(true);
		emit normalMoveError(true);
	}
}

AMBeamlineActionItem *VESPERSSampleStageControl::createHorizontalMoveAction(double position)
{
	if (!horiz_->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(horiz_);
	action->setSetpoint(position);

	return action;
}

AMBeamlineActionItem *VESPERSSampleStageControl::createHorizontalStopAction()
{
	if (!horiz_->isConnected())
		return 0;

	return new AMBeamlineControlStopAction(horiz_);
}

AMBeamlineActionItem *VESPERSSampleStageControl::createVerticalMoveAction(double position)
{
	if (!vert_->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(vert_);
	action->setSetpoint(position);

	return action;
}

AMBeamlineActionItem *VESPERSSampleStageControl::createVerticalStopAction()
{
	if (!vert_->isConnected())
		return 0;

	return new AMBeamlineControlStopAction(vert_);
}

AMBeamlineActionItem *VESPERSSampleStageControl::createNormalMoveAction(double position)
{
	if (!norm_->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(norm_);
	action->setSetpoint(position);

	return action;

}

AMBeamlineActionItem *VESPERSSampleStageControl::createNormalStopAction()
{
	if (!norm_->isConnected())
		return 0;

	return new AMBeamlineControlStopAction(norm_);
}

AMBeamlineActionItem *VESPERSSampleStageControl::createStopAllAction()
{
	if (!isConnected())
		return 0;

	AMBeamlineParallelActionsList *stopAllActionsList = new AMBeamlineParallelActionsList;
	AMBeamlineListAction *stopAllAction = new AMBeamlineListAction(stopAllActionsList);

	stopAllActionsList->appendStage(new QList<AMBeamlineActionItem *>());
	stopAllActionsList->appendAction(0, new AMBeamlineControlStopAction(horiz_));
	stopAllActionsList->appendAction(0, new AMBeamlineControlStopAction(vert_));
	stopAllActionsList->appendAction(0, new AMBeamlineControlStopAction(norm_));

	return stopAllAction;
}
