#ifndef BIOXASMAXVMOTORMOVETOLIMITACTION_H
#define BIOXASMAXVMOTORMOVETOLIMITACTION_H

#include "actions3/AMAction3.h"
#include "actions3/BioXAS/BioXASMAXvMotorMoveToLimitActionInfo.h"
#include "beamline/BioXAS/BioXASMAXvMotor.h"

// Timeouts.

#define BIOXASMAXVMOTORMOVETOLIMITACTION_PROGRESS_INTERVAL 250
#define BIOXASMAXVMOTORMOVETOLIMITACTION_MOVE_FINISHED_TIMEOUT 500

// Error codes.

#define BIOXASMAXVMOTORMOVETOLIMITACTION_INVALID_CONTROL 31097

class BioXASMAXvMotorMoveToLimitAction : public AMAction3
{
	Q_OBJECT

public:
	/// Constructor.
	Q_INVOKABLE BioXASMAXvMotorMoveToLimitAction(BioXASMAXvMotorMoveToLimitActionInfo *info, BioXASMAXvMotor *control, QObject *parent = 0);
	/// Copy constructor.
	BioXASMAXvMotorMoveToLimitAction(const BioXASMAXvMotorMoveToLimitAction &original);
	/// Destructor.
	virtual ~BioXASMAXvMotorMoveToLimitAction();

	/// Virtual copy constructor.
	virtual AMAction3* createCopy() const { return new BioXASMAXvMotorMoveToLimitAction(*this); }

	/// Returns true if this action can be paused, false otherwise.
	virtual bool canPause() const { return false; }
	/// Returns true if this action can be skipped, false otherwise.
	virtual bool canSkip() const { return false; }

	/// Returns true if this action has child actions, false otherwise.
	virtual bool hasChildren() const { return false; }
	/// Return the number of children that belong to this action.
	virtual int numberOfChildren() const { return 0; }

	/// Returns the control this action will move.
	BioXASMAXvMotor* control() const { return control_; }

	/// Returns the action info specific for this action.
	const BioXASMAXvMotorMoveToLimitActionInfo* moveToLimitInfo() const { return qobject_cast<const BioXASMAXvMotorMoveToLimitActionInfo*>(info()); }
	/// Returns the action info specific for this action.
	BioXASMAXvMotorMoveToLimitActionInfo* moveToLimitInfo() { return qobject_cast<BioXASMAXvMotorMoveToLimitActionInfo*>(info()); }

public slots:
	/// Sets the control used by this action.
	void setControl(BioXASMAXvMotor *motor) { control_ = motor; }

protected slots:
	/// Handles updating the action's state when the motor has started.
	void onMotorStarted();
	/// Handles updating the action's state when the motor has finished.
	void onMotorFinished();
	/// Handles updating the action's state when whether the motor is at a limit changes.
	void onMotorLimitStatusChanged();
	/// Handles updating the action's state when the move has finished.
	void onMoveFinished();

	/// Handles updating the action progress.
	void onProgressTimerTimeout();

protected:
	/// Returns true if the motor has finished moving.
	bool motorFinished() const { return (control_ && !control_->isMoving()); }
	/// Returns true if the limit has been reached.
	bool limitReached() const { return (control_ && control_->atLimit() == moveToLimitInfo()->limitSetpoint()); }
	/// Returns true if the move is finished.
	bool moveFinished() const { return motorFinished() && limitReached(); }

	/// This function is called from the Starting state when the implementation should initiate the action.
	virtual void startImplementation();
	/// For actions which support pausing, this function is called from the Pausing state when the implementation should pause the action.
	virtual void pauseImplementation() { return; }
	/// For actions that support resuming, this function is called from the Paused state when the implementation should resume the action.
	virtual void resumeImplementation() { setResumed(); }
	/// All implementations must support cancelling. This function will be called from the Cancelling state when the implementation should cancel the action.
	virtual void cancelImplementation() { setCancelled(); }
	/// The function is called from the Skipping state when the implementation should skip the action. This implementation does not support skipping.
	virtual void skipImplementation(const QString &command) { Q_UNUSED(command); }

protected:
	/// The control being moved.
	BioXASMAXvMotor *control_;
	/// The control's initial position, used for progress updates.
	double initialValue_;

	/// The timer used to issue progress updates.
	QTimer progressTimer_;
	/// The timer used to monitor time between move finished and limit reached conditions.
	QTimer moveFinishedTimer_;
};

#endif // BIOXASMAXVMOTORMOVETOLIMITACTION_H
