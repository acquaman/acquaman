#include "SGMMonochromator.h"

#include "util/AMErrorMonitor.h"
#include "beamline/AMPVControl.h"

#include "actions3/AMListAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "actions3/AMActionSupport.h"
#include "beamline/SGM/monochromator/SGMEnergyTrajectory.h"
SGMMonochromator::SGMMonochromator(QObject *parent) :
    AMPseudoMotorControl("Energy", "eV", parent, "Monochromator Energy")
{
    // Todo: Get names of, and initialize all the controls provided by the add on
    // coordinator.

    energyPv_ = new AMPVwStatusControl("Energy", "READ_PV", "WRITE_PV", "STATUS_PV", "STOP_PV", this, "Monochromator Energy");

    gratingAngleVelocityPv_ = new AMSinglePVControl("Grating Angle Velocity", "PV_NAME", this, "Grating Angle Velocity");
    undulatorVelocityPv_ = new AMSinglePVControl("Undulator Velocity", "PV_NAME", this, "Undulator Velocity");
    exitSlitVelocityPv_ = new AMSinglePVControl("Exit Slit Velocity", "PV_NAME", this, "Exit Slit Velocity");

    undulatorTrackingPv_ = new AMSinglePVControl("Undulator Tracking", "PV_NAME", this, 0.5, "Undulator Tracking");
    exitSlitTrackingPv_ = new AMSinglePVControl("Exit Slit Tracking", "PV_NAME", this, 0.5, "Exit Slit Tracking");

    requiredControls_ = new AMControlSet(this);

    requiredControls_->addControl(energyPv_);
    requiredControls_->addControl(gratingAngleVelocityPv_);
    requiredControls_->addControl(exitSlitVelocityPv_);
    requiredControls_->addControl(undulatorTrackingPv_);
    requiredControls_->addControl(exitSlitTrackingPv_);

    addChildControl(energyPv_);

    setConnected(requiredControls_->isConnected());
    connect(requiredControls_, SIGNAL(connected(bool)), this, SLOT(onRequiredControlsConnectionChanged(bool)));
    connect(undulatorTrackingPv_, SIGNAL(valueChanged(double)), this, SLOT(onUndulatorTrackingPvValueChanged(double)));
    connect(exitSlitTrackingPv_, SIGNAL(valueChanged(double)), this, SLOT(onExitSlitTrackingPvValeuChanged(double)));

}

bool SGMMonochromator::shouldMeasure() const
{
    return true;
}

bool SGMMonochromator::shouldMove() const
{
    return true;
}

bool SGMMonochromator::shouldStop() const
{
    return true;
}

bool SGMMonochromator::canMeasure() const
{
    return shouldMeasure() && isConnected();
}

bool SGMMonochromator::canMove() const
{
    return shouldMove() && isConnected();
}

bool SGMMonochromator::canStop() const
{
    return shouldStop() && isConnected();
}

bool SGMMonochromator::isUndulatorTracking() const
{
    if(!undulatorTrackingPv_->isConnected()) {
        return false;
    }

    return undulatorTrackingPv_->withinTolerance(1);
}

void SGMMonochromator::setUndulatorTracking(bool isTracking)
{
    double trackingValue;

    isTracking ? trackingValue = 1 : trackingValue = 0;

    if(undulatorTrackingPv_->isConnected() &&
            !undulatorTrackingPv_->withinTolerance(trackingValue)) {

        undulatorTrackingPv_->move(trackingValue);
    }

    // Note, emitting the tracking changed signal is handled by connecting up to
    // the PV's value changed signal
}

bool SGMMonochromator::isExitSlitTracking() const
{
    if(!exitSlitTrackingPv_->isConnected()) {
        return false;
    }

    return exitSlitTrackingPv_->withinTolerance(1);
}

void SGMMonochromator::setExitSlitTracking(bool isTracking)
{
    double trackingValue;

    isTracking ? trackingValue = 1 : trackingValue = 0;

    if(exitSlitTrackingPv_->isConnected() &&
            !exitSlitTrackingPv_->withinTolerance(trackingValue)) {

        exitSlitTrackingPv_->move(trackingValue);
    }

    // Note, emitting the tracking changed signal is handled by connecting up to
    // the PV's value changed signal
}

AMControl::FailureExplanation SGMMonochromator::move(double setpoint, double trajectoryTime)
{
    // Check that this control is connected and able to move before proceeding.

    if (!isConnected()) {
        AMErrorMon::alert(this, AMPSEUDOMOTORCONTROL_NOT_CONNECTED, QString("Failed to move %1: control is not connected.").arg(name()));
        return AMControl::NotConnectedFailure;
    }

    if (!canMove()) {
        AMErrorMon::alert(this, AMPSEUDOMOTORCONTROL_CANNOT_MOVE, QString("Failed to move %1: control cannot move.").arg(name()));
        return AMControl::OtherFailure;
    }

    if (isMoving() && !allowsMovesWhileMoving()) {
        AMErrorMon::alert(this, AMPSEUDOMOTORCONTROL_ALREADY_MOVING, QString("Failed to move %1: control is already moving.").arg(name()));
        return AMControl::AlreadyMovingFailure;
    }

    if (!validSetpoint(setpoint)) {
        AMErrorMon::alert(this, AMPSEUDOMOTORCONTROL_INVALID_SETPOINT, QString("Failed to move %1: provided setpoint is invalid.").arg(name()));
        return AMControl::LimitFailure;
    }

    // Update the setpoint.
    setSetpoint(setpoint);

    // If the new setpoint is within tolerance, no need to proceed with move.
    // Instead report a successful move to setpoint.

    if (withinTolerance(setpoint)) {
        onMoveStarted(0);
        onMoveSucceeded(0);
        return AMControl::NoFailure;
    }

    // Otherwise, an actual move is needed.
    // Create new move action.

    AMAction3 *moveAction = createMoveAction(setpoint_, trajectoryTime);

    // Check that a valid move action was generated.
    // If an invalid move action was created, abort the move.

    if (!moveAction) {
        AMErrorMon::alert(this, AMPSEUDOMOTORCONTROL_INVALID_MOVE_ACTION, QString("Did not move %1: invalid move action generated.").arg(name()));
        onMoveStarted(0);
        onMoveFailed(0);
        return AMControl::LimitFailure;
    }

    // Otherwise, proceed with initializing and running the move action.
    // Create move action signal mappings.

    startedMapper_->setMapping(moveAction, moveAction);
    connect( moveAction, SIGNAL(started()), startedMapper_, SLOT(map()) );

    cancelledMapper_->setMapping(moveAction, moveAction);
    connect( moveAction, SIGNAL(cancelled()), cancelledMapper_, SLOT(map()) );

    failedMapper_->setMapping(moveAction, moveAction);
    connect( moveAction, SIGNAL(failed()), failedMapper_, SLOT(map()) );

    succeededMapper_->setMapping(moveAction, moveAction);
    connect( moveAction, SIGNAL(succeeded()), succeededMapper_, SLOT(map()) );

    // Run action.

    moveAction->start();

    return AMControl::NoFailure;
}

void SGMMonochromator::onUndulatorTrackingPvValueChanged(double /*value*/)
{
    bool isTracking = undulatorTrackingPv_->withinTolerance(1);

    emit undulatorTrackingChanged(isTracking);
}

void SGMMonochromator::onExitSlitTrackingPvValeuChanged(double /*value*/)
{
    bool isTracking = exitSlitTrackingPv_->withinTolerance(1);

    emit exitSlitTrackingChanged(isTracking);
}

void SGMMonochromator::onRequiredControlsConnectionChanged(bool isConnected)
{
    setConnected(isConnected);
}

void SGMMonochromator::updateConnected()
{
    // Our required controls set already takes care of this.
}

void SGMMonochromator::updateValue()
{
    setValue(energyPv_->value());
}

void SGMMonochromator::updateMoving()
{
    setIsMoving(energyPv_->isMoving());
}

AMAction3 *SGMMonochromator::createMoveAction(double setpoint)
{
    return AMActionSupport::buildControlMoveAction(energyPv_, setpoint, false);
}

AMAction3 *SGMMonochromator::createMoveAction(double setpoint, double trajectoryTime)
{

    AMListAction3* parentAction = new AMListAction3(new AMListActionInfo3("Energy Move",
                                                                          QString("Energy Trajectory Move (%1 eV to %2 eV)").arg(value()).arg(setpoint())),
                                                    AMListAction3::Sequential);


    // Calculate the velocities required for the trajectory move.
    SGMEnergyTrajectory energyTrajectory(value(), setpoint, trajectoryTime, SGMGratingSupport::LowGrating);

    if(energyTrajectory.hasErrors()) {
        return 0;
    }

    // Create a parallel action list to set these velocities first
    AMListAction3* velocityActions = new AMListAction3(new AMListActionInfo3(QString("Moving %1").arg(name()),
                                                                             QString("Moving %1").arg(name())),
                                                       AMListAction3::Parallel);
    velocityActions->addSubAction(AMActionSupport::buildControlMoveAction(gratingAngleVelocityPv_, energyTrajectory.gratingAngleVelocityProfile().targetVelocity(), false));
    velocityActions->addSubAction(AMActionSupport::buildControlMoveAction(undulatorVelocityPv_, energyTrajectory.undulatorVelocityProfile().targetVelocity(), false));
    velocityActions->addSubAction(AMActionSupport::buildControlMoveAction(exitSlitVelocityPv_, energyTrajectory.exitSlitVelocityProfile().targetVelocity(), false));

    parentAction->addSubAction(velocityActions);

    // Create an action to move the energy, after all the velocities have been set
    parentAction->addSubAction(AMActionSupport::buildControlMoveAction(energyPv_, setpoint, false));

    return parentAction;
}

