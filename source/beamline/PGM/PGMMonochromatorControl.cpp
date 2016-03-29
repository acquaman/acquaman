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

#include "PGMMonochromatorControl.h"
#include "PGMBeamline.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"


PGMMonochromatorControl::PGMMonochromatorControl(QObject *parent)
  : AMPVwStatusControl( /* ToDo: PV INFO REQUIRED   */ )
{
    // When control functionacility achieved signal is emitted, send signal to this classes controls connected slot
    connect(this, SIGNAL(connected(bool)), this, SLOT(onAllControlsConnected()));
}

void PGMMonochromatorControl::onAllControlsConnected()
{
    setUnits("eV");
    lastPositiveMove_ = PGMBeamline::pgm()->monoEnergyControl()->value();
}

AMControl::FailureExplanation PGMMonochromatorControl::move(double setpoint)
{
    // ToDo: Create monoHighEV/monoLowEV in beamline class
    if(setpoint > PGMBeamline::pgm()->monoHighEV()->value())
        setpoint = PGMBeamline::pgm()->monoHighEV()->value();

    if(setpoint < PGMBeamline::pgm()->monoLowEV()->value())
        setpoint = PGMBeamline::pgm()->monoLowEV()->value();

    // Check to see if current setpoint move is within a tolerance, if so return.
    int moveTolerance = 0.1;
    if(qAbs(setpoint - setpoint_) < moveTolerance)
    {
        emit moveSucceeded();
        return AMControl::NoFailure;
    }

    // Reset lastPositiveMove value
    if(setpoint > lastPositiveMove_)
    {
        lastPositiveMove_ = setpoint;
        return AMPVwStatusControl::move(setpoint);
    }

    // Allow for small backwards moves. ToDo: What is an allowable "Small" backwards move
    int smallBackwardsMove = 100;
    if (setpoint > (lastPositiveMove_ - smallBackwardsMove))
        return AMPVwStatusControl::move(setpoint);


    // Begin backlash corrected move action  ToDo: Whats does backlash mean?
    lastPositiveMove_ = setpoint;

    // ToDo: create mono2d()
    double mono2d = PGMBeamline::pgm()->mono2d()->value();
    // For use in computing bragg angle. ToDo: Is this value beamline dependant?
    double braggAngleTheta = 12398.4193;
    double braggAngle = asin(braggAngleTheta / mono2d / setpoint);
    // ToDo: Is this value beamline dependant?
    double backlashDegrees = 4;

    // Compute backlash energy, using change in energy
    double dE = (backlashDegrees / 180 * M_PI) * (mono2d * setpoint * setpoint * cos(braggAngle * M_PI / 180)) / (-braggAngleTheta);
    double backlashEnergy = setpoint + dE;

    if(backlashEnergy < PGMBeamline::pgm()->monoLowEV()->value())
        backlashEnergy = PGMBeamline::pgm()->monoLowEV()->value();

    // Create Action list for mono move and mono backlash corrected move
    moveAction_ = new AMListAction3(new AMListActionInfo3("PGM Mono Move", "PGM Backlash Correctect Mono Move"));

    //ToDo: monoDirectEnergyControl()
    moveAction_->addSubAction(AMActionSupport::buildControlMoveAction(PGMBeamline::pgm()->monoDirectEnergyControl(), backlashEnergy));
    moveAction_->addSubAction(AMActionSupport::buildControlMoveAction(PGMBeamline::pgm()->monoDirectEnergyControl(), setpoint));

    connect(moveAction_, SIGNAL(failed()), this, SLOT(onMoveActionsFailed()));
    connect(moveAction_, SIGNAL(cancelled()), this, SLOT(onMoveActionsFailed()));
    connect(moveAction_, SIGNAL(succeeded()), this, SLOT(onMoveActionSucceeded()));

    moveAction_->start();

    return AMControl::NoFailure;
}


// If move has failed or was canceled then remove connections from moveAction object. Prepare
// it for deletion. Emit signal if it in the process of moving as well as cause of failure
void PGMMonochromatorControl::onMoveActionsFailed()
{
    disconnect(moveAction_, 0, this, 0);
    moveAction_->deleteLater();
    moveAction_ = 0;

    bool nowMoving = isMoving();
    if(nowMoving != wasMoving_)
        emit movingChanged(wasMoving_ = nowMoving);

    //emit reason for failing
    if(stopInProgress_){
        emit moveFailed(AMControl::WasStoppedFailure);
    } else {
        emit moveFailed(AMControl::OtherFailure);
    }

    stopInProgress_ = false;

}

// If move was succsesfull then remove connections from moveAction object. Prepair it
// for deletion. If moved to desired position emit move succeeded, otherwise emit a tolerance failure
void PGMMonochromatorControl::onMoveActionSucceeded()
{
    disconnect(moveAction_, 0, this, 0);
    moveAction_->deleteLater();
    moveAction_ = 0;

    bool nowMoving = isMoving();
    if(nowMoving != wasMoving_)
        emit movingChanged(wasMoving_ = nowMoving);

    // Did it move to the desired location?
    if(inPosition()){
        emit moveSucceeded();
    } else {
        emit moveFailed(AMControl::ToleranceFailure);
    }
}

PGMDirectMonochromatorControl::PGMDirectMonochromatorControl(QObject *parent)
    : AMPVwStatusControl(/* ToDo: PV INFO REQUIRED  */)
{
    connect(this, SIGNAL(connected(bool)), this, SLOT(onAllControlsConnected()));
}

void PGMDirectMonochromatorControl::onAllControlsConnected()
{
    setUnits("eV");
}

AMControl::FailureExplanation PGMDirectMonochromatorControl::move(double setpoint)
{
    // Check to see if current setpoint move is within a tolerance, if so return.
    int moveTolerance = 0.1;
    if(qAbs(setpoint - setpoint_) < moveTolerance)
    {
        emit moveSucceeded();
        return AMControl::NoFailure;
    }

    return AMPVwStatusControl::move(setpoint);
}
