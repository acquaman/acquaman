#include "BioXASMAXvMotorMoveToLimitAction.h"
#include "beamline/AMBeamline.h"
#include "util/AMErrorMonitor.h"

BioXASMAXvMotorMoveToLimitAction::BioXASMAXvMotorMoveToLimitAction(BioXASMAXvMotorMoveToLimitActionInfo *info, BioXASMAXvMotor *motor, QObject *parent) :
	AMAction3(info, parent)
{
	control_ = motor;
	initialValue_ = 0;
}

BioXASMAXvMotorMoveToLimitAction::BioXASMAXvMotorMoveToLimitAction(const BioXASMAXvMotorMoveToLimitAction &original) :
	AMAction3(original)
{
	control_ = original.control();
	initialValue_ = 0;
}

BioXASMAXvMotorMoveToLimitAction::~BioXASMAXvMotorMoveToLimitAction()
{

}
#include <QDebug>
void BioXASMAXvMotorMoveToLimitAction::onMotorMoveStarted()
{
	qDebug() << "\n\nMove to limit started.";

	// Set the action as started.

	setStarted();

	// Start progress timer updates.

	connect( &progressTimer_, SIGNAL(timeout()), this, SLOT(onProgressTick()) );
	progressTimer_.start(250);
}

void BioXASMAXvMotorMoveToLimitAction::onMotorMoveFailed()
{
	qDebug() << "Move to limit failed.";

	// Disconnect from control.

	disconnect( control_, 0, this, 0 );

	// End progress timer updates.

	progressTimer_.stop();
	disconnect( &progressTimer_, 0, this, 0 );

	// Check whether the control made it to the appropriate limit.
	// If so, the action has succeeded. If not, the action has failed.

	if (atLimit()) {
		setProgress(100, 100);
		setSucceeded();

	} else {
		setFailed(QString("Failed to move motor '%1' to '%2' limit. The move failed without reaching the limit.").arg(control_->name()).arg(moveToLimitInfo()->limitSetpointToString(moveToLimitInfo()->limitSetpoint())));
	}
}

void BioXASMAXvMotorMoveToLimitAction::onMotorMoveSucceeded()
{
	qDebug() << "Move to limit succeeded.";

	// Disconnect from control.

	disconnect( control_, 0, this, 0 );

	// End progress timer updates.

	progressTimer_.stop();
	disconnect( &progressTimer_, 0, this, 0 );

	// Check whether the control made it to the appropriate limit.
	// If so, the action has succeeded. If not, the action has failed.

	if (atLimit()) {
		setProgress(100, 100);
		setSucceeded();

	} else {
		setFailed(QString("Failed to move motor '%1' to '%2' limit. The move finished without reaching the limit.").arg(control_->name()).arg(moveToLimitInfo()->limitSetpointToString(moveToLimitInfo()->limitSetpoint())));
	}
}

void BioXASMAXvMotorMoveToLimitAction::onProgressTick()
{
	double destination = control_->setpoint();
	double fractionComplete = (control_->value() - initialValue_) / (destination - initialValue_);
	double runningSeconds = runningTime();
	double expectedTotalSeconds = runningSeconds / fractionComplete;

	if(fractionComplete != 0) {
		setExpectedDuration(expectedTotalSeconds);
		setProgress(runningSeconds, expectedTotalSeconds);

	} else {
		setProgress(0, 100);
	}
}

bool BioXASMAXvMotorMoveToLimitAction::atLimit() const
{
	return (control_ && control_->atLimit() == moveToLimitInfo()->limitSetpoint());
}

void BioXASMAXvMotorMoveToLimitAction::startImplementation()
{
	// Must have a control.

	if(!control_) {
		AMErrorMon::alert(this, BIOXASMAXVMOTORMOVETOLIMITACTION_INVALID_CONTROL, QString("Failed to move control '%1' to '%2' limit. The control was not found.").arg(moveToLimitInfo()->controlInfo().name()).arg(moveToLimitInfo()->limitSetpointToString(moveToLimitInfo()->limitSetpoint())));
		setFailed();
	}

	// Make connections.

	connect( control_, SIGNAL(moveStarted()), this, SLOT(onMotorMoveStarted()) );
	connect( control_, SIGNAL(moveFailed(int)), this, SLOT(onMotorMoveFailed()) );
	connect( control_, SIGNAL(moveSucceeded()), this, SLOT(onMotorMoveSucceeded()) );

	// Identify initial value.

	initialValue_ = control_->value();

	// Start the move to the limit.

	AMControl::FailureExplanation failureExplanation = control_->moveToLimit(moveToLimitInfo()->limitSetpoint());
	if (failureExplanation != AMControl::NoFailure) {
		qDebug() << "Motor move failed before it started.";
		onMotorMoveFailed();
	}
}

