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


#ifndef AMLOOPACTION3_H
#define AMLOOPACTION3_H

#include "actions3/AMListAction3.h"

#include "actions3/AMLoopActionInfo3.h"

/// An AMLoopAction contains a list of sub-actions that, when the loop action is run, will be executed a fixed number of times.  It implements the AMNestedAction interface so that the sub-actions are visible inside the AMActionRunner views, so that users can drag-and-drop existing actions into/out-of the loop.
/*! The sub-actions are effectively used as templates, because a new copy of each sub-action will be made every time it is executed. You can configure whether you want the entire loop action logged as one, or every sub-action to be logged individually, by calling setShouldLogSubActionsSeparately(). */
class AMLoopAction3 : public AMSequentialListAction3
{
	Q_OBJECT
public:
	/// Constructor. Requires an AMLoopActionInfo for the \c info.
	Q_INVOKABLE AMLoopAction3(AMLoopActionInfo3* info, QObject *parent = 0);
	/// This convenience constructor is a synonym for AMLoopAction(new AMLoopActionInfo(iterations)).
	AMLoopAction3(int iterations, QObject* parent = 0);

	/// Copy constructor, to make a new runnable action just like \c other. Here we make a copy of all the sub-actions.
	AMLoopAction3(const AMLoopAction3& other);

	/// This virtual function takes the role of a virtual copy constructor.
	virtual AMAction3* createCopy() const { return new AMLoopAction3(*this); }

	/// Destructor: Deletes all the subActions_
	virtual ~AMLoopAction3();

	// Public virtual functions: re-implemented:
	//////////////////////

	/// Returns the current loop iteration (ie: loop counter).  Will be 0 before and while running the first loop, 1 during the second loop, and finally loopCount() after the last loop is done.
	int currentIteration() const { return currentIteration_; }

	/// Returns the currently-running sub-action, or 0 if none is running.
	virtual const AMAction3* currentSubAction() const { return currentSubAction_; }
	/// Returns the currently-running sub-action, or 0 if none is running.
	virtual AMAction3* currentSubAction() { return currentSubAction_; }

	virtual bool canPause() const;
	/// Returns whether the action can skip.  Loop actions can skip.
	virtual bool canSkip() const { return true; }

	// new public functions:
	/////////////////////////////

	/// Returns the number of iterations to loop for:
	int loopCount() const { return loopInfo()->loopCount(); }

	/// Public function to duplicate a set of sub-actions at \c indexesToCopy. The new sub-actions will all be inserted at the position after the last existing sub-action in \c indexesToCopy.  It uses insertSubAction() to add copies of the existing ones, and therefore will fail if the action is not in the AMAction::Constructed state.
	bool duplicateSubActions(const QList<int>& indexesToCopy);

signals:
	// see AMNestedAction::currentSubActionChanged(int) for when the current sub-action changes.

	/// Emitted whenever a loop completes / the next loop starts. Contains the current loop index
	void currentIterationChanged(int currentIteration);

public slots:
	/// Set the number of iterations to loop for. If the action is running and this is smaller than the number already completed, it will finish at the end of the current loop.
	void setLoopCount(int newLoopCount) { loopInfo()->setLoopCount(newLoopCount); }

protected slots:
	/// Called when the current action's progress is updated. We use it to update our own progress
	virtual void internalOnCurrentActionProgressChanged(double numerator, double denominator);
	/// Called when any of the sub-actions emits statusTextChanged()
	virtual void internalOnSubActionStatusTextChanged(const QString &statusText);

protected:
	// Protected virtual functions to re-implement
	/////////////////////////////////////

	// These virtual functions allow subclasses to implement their unique action behaviour.  They are called at the appropriate time by the base class, when base-class-initiated state changes happen: ->Starting, ->Cancelling, ->Pausing, ->Resuming
	/////////////////////////
	/// This function is called from the Starting state when the implementation should initiate the action. Once the action is started, you should call notifyStarted().
	virtual void startImplementation();
	/// This function is called when the action should be skipped in some way.
	virtual void skipImplementation(const QString &command);

	/// Helper function to manage action and loop iterations. Does everything we need to do to move onto the next action (either at the beginning, or after the last one completes).
	virtual void internalDoNextAction();
	/// Re-implemented.  Helper function to handle all clean up responsibilities for an action.  Disconnects, logs, and deletes the action.
	virtual void internalCleanupAction(AMAction3 *action = 0);

	/// Our info() will always be an AMLoopActionInfo, but info() returns it as an AMActionInfo*.  This makes it easier to access.
	AMLoopActionInfo3* loopInfo() { return qobject_cast<AMLoopActionInfo3*>(info()); }
	/// Our info() will always be an AMLoopActionInfo, but info() returns it as an AMActionInfo*.  This makes it easier to access.
	const AMLoopActionInfo3* loopInfo() const { return qobject_cast<const AMLoopActionInfo3*>(info()); }


	/// The iterator that keeps track of how far along the loop is.
	int currentIteration_;
	/// The current action that is being run in the loop.
	AMAction3 *currentSubAction_;
	/// The flag for whether the action should be skipped after the current iteration.
	bool skipAfterCurrentIteration_;
};

#endif // AMLOOPACTION_H
