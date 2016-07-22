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


#include "IDEASMonochromatorControl.h"
#include "IDEASBeamline.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"



IDEASMonochromatorControl::IDEASMonochromatorControl(QObject *parent)
//: AMPVwStatusControl("Energy", "BL08B2-1:Energy:EV:fbk", "BL08B2-1:Energy:EV", "SVM1608-9-B20-01:brag.DMOV", "BL08B2-1:Energy:stop", parent, AMCONTROL_TOLERANCE_DONT_CARE, 2.0, new AMControlStatusCheckerDefault(0), 1, "Mono Energy")
	: AMPVwStatusControl("Energy", "BL08B2-1:Energy:EV:fbk", "BL08B2-1:Energy:EV", "BL08B2-1:Energy:status", "BL08B2-1:Energy:stop", parent, AMCONTROL_TOLERANCE_DONT_CARE, 2.0, new AMControlStatusCheckerDefault(1), 1, "Mono Energy")
{
	connect(this, SIGNAL(connected(bool)), this, SLOT(onAllControlsConnected()));
}

void IDEASMonochromatorControl::onAllControlsConnected()
{
	setUnits("eV");
	lastPositiveMove_ = 30000; //IDEASBeamline::ideas()->monoEnergyControl()->value()
}
#include <QDebug>
AMControl::FailureExplanation IDEASMonochromatorControl::move(double setpoint)
{

	if (setpoint > IDEASBeamline::ideas()->monoHighEV()->value())
		setpoint = IDEASBeamline::ideas()->monoHighEV()->value();

	if (setpoint < IDEASBeamline::ideas()->monoLowEV()->value())
		setpoint = IDEASBeamline::ideas()->monoLowEV()->value();

	if (qAbs(setpoint-setpoint_) < 0.1)
	{

		emit moveSucceeded();
		return AMControl::NoFailure;
	}

	if (setpoint > lastPositiveMove_)    //Just do Positive moves, resetting lastPositiveMove
	{
		lastPositiveMove_ = setpoint;
		return AMPVwStatusControl::move(setpoint);

	}

	if ( setpoint > (lastPositiveMove_ - 100))    //Allow small backward moves
		return AMPVwStatusControl::move(setpoint);


	lastPositiveMove_ = setpoint;  //Else begin backlash corrected move action

	double mono2d = IDEASBeamline::ideas()->mono2d()->value();
	double braggAngle = asin(12398.4193 / mono2d / setpoint);
	double backlashDegrees = 4;

	double dE = (backlashDegrees / 180 * M_PI) * (mono2d * setpoint * setpoint * cos(braggAngle * M_PI / 180)) / (-12398.4193);
	double backlashE = setpoint + dE;
	if(backlashE < IDEASBeamline::ideas()->monoLowEV()->value()) backlashE = IDEASBeamline::ideas()->monoLowEV()->value();


	moveAction_ = new AMListAction3(new AMListActionInfo3("IDEAS Mono Move", "IDEAS Backlash Corrected Mono Move"));

	moveAction_->addSubAction(AMActionSupport::buildControlMoveAction(IDEASBeamline::ideas()->monoDirectEnergyControl(), backlashE));
	moveAction_->addSubAction(AMActionSupport::buildControlMoveAction(IDEASBeamline::ideas()->monoDirectEnergyControl(), setpoint));

	connect(moveAction_, SIGNAL(failed()), this, SLOT(onMoveActionFailed()));
	connect(moveAction_, SIGNAL(cancelled()), this, SLOT(onMoveActionFailed()));
	connect(moveAction_, SIGNAL(succeeded()), this, SLOT(onMoveActionSucceeded()));


	moveAction_->start();

	return AMControl::NoFailure;
}


void IDEASMonochromatorControl::onMoveActionFailed()
{
	disconnect(moveAction_, 0, this, 0);
	moveAction_->deleteLater();
	moveAction_ = 0;

	bool nowMoving = isMoving();
	if(nowMoving != wasMoving_)
		emit movingChanged(wasMoving_ = nowMoving);

	// was cancelled or failed.  If we were supposed to be stopping:
	if(stopInProgress_) {
		emit moveFailed(AMControl::WasStoppedFailure);
	}
	else {
		emit moveFailed(AMControl::OtherFailure);
	}

	stopInProgress_ = false;
}

void IDEASMonochromatorControl::onMoveActionSucceeded()
{
	disconnect(moveAction_, 0, this, 0);
	moveAction_->deleteLater();
	moveAction_ = 0;

	bool nowMoving = isMoving();
	if(nowMoving != wasMoving_)
		emit movingChanged(wasMoving_ = nowMoving);

	// check tolerance:
	if(inPosition()) {
		emit moveSucceeded();
	}
	else {
		emit moveFailed(AMControl::ToleranceFailure);
	}
}

IDEASDirectMonochromatorControl::IDEASDirectMonochromatorControl(QObject *parent)
	: AMPVwStatusControl("DirectEnergy", "BL08B2-1:Energy:EV:fbk", "BL08B2-1:Energy:EV", "SVM1608-9-B20-01:brag.DMOV", "BL08B2-1:Energy:stop", parent, AMCONTROL_TOLERANCE_DONT_CARE, 2.0, new AMControlStatusCheckerDefault(0), 1, "Mono Energy")
{
	connect(this, SIGNAL(connected(bool)), this, SLOT(onAllControlsConnected()));
}

void IDEASDirectMonochromatorControl::onAllControlsConnected()
{
	setUnits("eV");
}


AMControl::FailureExplanation IDEASDirectMonochromatorControl::move(double setpoint)
{
	if (qAbs(setpoint-setpoint_) < 0.1){

		emit moveSucceeded();
		return AMControl::NoFailure;
	}
	return AMPVwStatusControl::move(setpoint);
}
