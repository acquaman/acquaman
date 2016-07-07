#include "AMMockMoveAction.h"

AMMockMoveAction::AMMockMoveAction(AMActionInfo3* info,
                                   AMMockControl* control,
                                   double setpoint,
				                   QObject* parent)
    :AMAction3(info, parent)
{

	setpoint_ = setpoint;
	control_ = control;

	// Sends updates every 1/10th of a second
	moveUpdateTimer_.setInterval(100);
	connect(&moveUpdateTimer_, SIGNAL(timeout()), this, SLOT(onMoveUpdateTimerTimeout()));
}

void AMMockMoveAction::onMoveUpdateTimerTimeout()
{
	bool isMovingBackwards = setpoint_ < startPosition_;

	double timeSinceLastUpdate = moveUpdateTimer_.interval() / 1000.0;

	if(isMovingBackwards) {

		emit moveValueChanged(qMax(control_->value() - timeSinceLastUpdate, setpoint_));
	} else {

		emit moveValueChanged(qMin(control_->value() + timeSinceLastUpdate, setpoint_));
	}

	if(control_->withinTolerance(setpoint_)) {

		moveUpdateTimer_.stop();
		setSucceeded();
	}
}

void AMMockMoveAction::startImplementation()
{
	startPosition_ = control_->value();
	moveUpdateTimer_.start();
	setStarted();
}

void AMMockMoveAction::pauseImplementation()
{
	// The action cannnot be paused.
}

void AMMockMoveAction::resumeImplementation()
{
	// The action cannot be paused.
}

void AMMockMoveAction::cancelImplementation()
{
	moveUpdateTimer_.stop();
	setCancelled();
}

void AMMockMoveAction::skipImplementation(const QString&)
{
	moveUpdateTimer_.stop();
	setSkipped();
}



