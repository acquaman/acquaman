/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef AMCONTROLMOVEACTION_H
#define AMCONTROLMOVEACTION_H

#include "actions2/AMAction.h"
#include "actions2/actions/AMControlMoveActionInfo.h"

#include <QTimer>

class AMControl;

/// This implementation of AMAction takes care of moving an AMControl into position.  Essentially, it wraps AMControl for use with the Action/Workflow system.
class AMControlMoveAction : public AMAction
{
	Q_OBJECT
public:
	// Constructors and copying
	//////////////////////////////

	/// Constructor. Requires and takes ownership of an existing AMControlMoveActionInfo \c info.
	Q_INVOKABLE AMControlMoveAction(AMControlMoveActionInfo* info, QObject *parent = 0);
	/// Copy constructor: must re-implement, but can simply use the AMAction copy constructor to make copies of the info and prereqs. We need to reset any internal state variables to make the copy a "like new" action - ie, not run yet.
	AMControlMoveAction(const AMControlMoveAction& other) : AMAction(other) { control_ = 0; }
	/// Virtual copy constructor
	virtual AMAction* createCopy() const { return new AMControlMoveAction(*this); }

	// Re-implemented public functions
	///////////////////////////////

	/// Specify that we cannot pause (since AMControl cannot pause).  If we wanted to get fancy, we might check if the control could stop, (and stop it for pause, and then start it again to resume). But this is much simpler for now.
	virtual bool canPause() const { return false; }

	// New public functions
	//////////////////////////////////
	/// [Must re-implement] Returns a pointer to the beamline-specific AMControl we should use to run this action. \c controlInfo contains the name and setpoint that were specified in our info().
	/*! Due to the separation of implementation and parameters into AMAction and AMActionInfo, our AMControlMoveActionInfo cannot hold an AMControl directly, since it needs to be OK to exist on any beamline/non-beamline/Acquaman release. Instead, it holds an AMControlInfo.  Therefore, AMControlMoveAction needs to be subclassed for beamline-specific use by implementing this function, which is responsible for returning a pointer to the control we must use to run the action.

The default implementation returns 0, which will cause the action to fail.*/
	virtual AMControl* getControlForAction(const AMControlInfo& controlInfo) const { Q_UNUSED(controlInfo) return 0; }

signals:

public slots:

protected:

	// The following functions are used to define the unique behaviour of the action.  We set them up in this way so that subclasses don't need to worry about (and cannot) break the state machine logic; they only need to fill in their pieces.

	// These virtual functions allow us to implement our unique action behaviour.  They are called at the appropriate time by the base class, when base-class-initiated state changes happen: ->Starting, ->Cancelling, ->Pausing, ->Resuming
	/////////////////////////
	/// This function is called from the Starting state when the implementation should initiate the action. Once the action is started, you should call notifyStarted().
	virtual void startImplementation();

	/// All implementations must support cancelling. This function will be called from the Cancelling state. Implementations will probably want to examine the previousState(), which could be any of Starting, Running, Pausing, Paused, or Resuming. Once the action is cancelled and can be deleted, you should call notifyCancelled().
	/*! \note If startImplementation() was never called, you won't receive this when a user tries to cancel(); the base class will handle it for you. */
	virtual void cancelImplementation();

protected slots:
	/// Every second, we emit a progress update with setProgress()
	void onProgressTick();

	/// Handle signals from our control:
	void onMoveStarted();
	void onMoveReTargetted();	///< Only connected after a control starts moving.  If it is re-directed in mid-move to a different setpoint, we can be sure our intended move failed.  Calls onMoveFailed(6);
	void onMoveFailed(int reason);
	void onMoveSucceeded();

protected:

	/// We can always access our info object via info_ or info(), but it will come back as a AMActionInfo* pointer that we would need to cast to AMControlMoveActionInfo. This makes it easier to access.
	const AMControlMoveActionInfo* controlMoveInfo() const { return qobject_cast<const AMControlMoveActionInfo*>(info()); }
	/// We can always access our info object via info_ or info(), but it will come back as a AMActionInfo* pointer that we would need to cast to AMControlMoveActionInfo. This makes it easier to access.
	AMControlMoveActionInfo* controlMoveInfo() { return qobject_cast<AMControlMoveActionInfo*>(info()); }

	// Internal variables:

	/// Timer used to issue progress updates on a per-second basis
	QTimer progressTick_;

	/// A pointer to the AMControl we use to implement the action
	AMControl* control_;
	/// Stores the start position, which we use for calculating progress
	AMControlInfo startPosition_;

};

#endif // AMCONTROLMOVEACTION_H
