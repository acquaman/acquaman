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


#ifndef AMSAMPLEPLATEPRE2013MOVEACTION_H
#define AMSAMPLEPLATEPRE2013MOVEACTION_H

#include "actions3/AMAction3.h"
#include "actions3/actions/AMSamplePlatePre2013MoveActionInfo.h"
#include "dataman/info/AMControlInfoList.h"

#include <QTimer>

#define AMSAMPLEPLATEPRE2013MOVEACTION_NO_SAMPLE_POSITIONER 327101
#define AMSAMPLEPLATEPRE2013MOVEACTION_POSITIONER_MOTOR_CANNOT_MOVE 327102
#define AMSAMPLEPLATEPRE2013MOVEACTION_POSITIONER_MOVE_OUT_OF_RANGE 327103
#define AMSAMPLEPLATEPRE2013MOVEACTION_CONTROL_MOVE_CREATION_FAILED 327104
#define AMSAMPLEPLATEPRE2013MOVEACTION_MOVELIST_FAILED 327105
#define AMSAMPLEPLATEPRE2013MOVEACTION_BAD_POSITION_SETPOINT 327106

class AMControlSet;
class AMParallelListAction3;

class AMSamplePlatePre2013MoveAction : public AMAction3
{
Q_OBJECT

public:
	/// Constructor, Requires and takes ownership of an exisiting AMSamplePlateMoveActionInfo. Does not require a control set, this will be looked up from the beamline automatically.
	Q_INVOKABLE AMSamplePlatePre2013MoveAction(AMSamplePlatePre2013MoveActionInfo *info, QObject *parent = 0);
	/// Copy constructor
	AMSamplePlatePre2013MoveAction(const AMSamplePlatePre2013MoveAction &other);

	virtual ~AMSamplePlatePre2013MoveAction();

	/// Virtual copy constructor
	virtual AMAction3* createCopy() const;

	// Re-implemented public functions
	///////////////////////////////

	/// Specify that we cannot pause (since AMControl cannot pause).  If we wanted to get fancy, we might check if the control could stop, (and stop it for pause, and then start it again to resume). But this is much simpler for now.
	virtual bool canPause() const { return false; }
	/// This method does not support the skip ability.
	virtual bool canSkip() const { return false; }

	/// Virtual function that denotes that this action has children underneath it or not.
	virtual bool hasChildren() const { return false; }
	/// Virtual function that returns the number of children for this action.
	virtual int numberOfChildren() const { return 0; }

protected slots:
	void onMoveListStarted();
	void onMoveListFailed();
	void onMoveListSucceeded();
	void onMoveListCancelled();

protected:
	// The following functions are used to define the unique behaviour of the action.  We set them up in this way so that subclasses don't need to worry about (and cannot) break the state machine logic, they only need to fill in their pieces.

	// These virtual functions allow us to implement our unique action behaviour.  They are called at the appropriate time by the base class, when base-class-initiated state changes happen: ->Starting, ->Cancelling, ->Pausing, ->Resuming
	/////////////////////////

	/// This function is called from the Starting state when the implementation should initiate the action. Once the action is started, you should call notifyStarted().
	virtual void startImplementation();

	/// For actions which support pausing, this function is called from the Pausing state when the implementation should pause the action. Once the action is paused, you should call notifyPaused().  The base class implementation does nothing and must be re-implemented.
	virtual void pauseImplementation() { setPaused(); }

	/// For actions that support resuming, this function is called from the Paused state when the implementation should resume the action. Once the action is running again, you should call notifyResumed().
	virtual void resumeImplementation() { setResumed(); }

	/// All implementations must support cancelling. This function will be called from the Cancelling state. Implementations will probably want to examine the previousState(), which could be any of Starting, Running, Pausing, Paused, or Resuming. Once the action is cancelled and can be deleted, you should call notifyCancelled().
	/*! \note If startImplementation() was never called, you won't receive this when a user tries to cancel(); the base class will handle it for you. */
	virtual void cancelImplementation();

	/// Since the action does not support skipping, this method is empty.
	virtual void skipImplementation(const QString &command) { Q_UNUSED(command); }

	/// We can always access our info object via info_ or info(), but it will come back as a AMActionInfo* pointer that we would need to cast to AMSamplePlateMoveActionInfo. This makes it easier to access.
	const AMSamplePlatePre2013MoveActionInfo* samplePlateMoveInfo() const { return qobject_cast<const AMSamplePlatePre2013MoveActionInfo*>(info()); }
	/// We can always access our info object via info_ or info(), but it will come back as a AMActionInfo* pointer that we would need to cast to AMControlMoveActionInfo. This makes it easier to access.
	AMSamplePlatePre2013MoveActionInfo* samplePlateMoveInfo() { return qobject_cast<AMSamplePlatePre2013MoveActionInfo*>(info()); }

protected:
	/// Timer used to issue progress updates on a per-second basis
	QTimer progressTick_;

	/// A pointer to the AMControlSet we use to implement this action (taken from beamline's sample manipulator)
	AMControlSet *sampleManipulator_;

	/// Store the start position, which we use for calculating progress
	AMControlInfoList startPosition_;

	/// We're going to use a parallel list action to get the job done
	AMParallelListAction3 *moveListAction_;
};

#endif // AMSAMPLEPLATEPRE2013MOVEACTION_H
