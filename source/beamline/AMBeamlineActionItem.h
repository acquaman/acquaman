/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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


#ifndef AMBEAMLINEACTIONITEM_H
#define AMBEAMLINEACTIONITEM_H

#include "util/AMOrderedSet.h"

#include <QObject>
#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDebug>
#include <QMouseEvent>
#include <QStyle>

/// Using this for debuggging purposes in all ActionItem descendents and views
#define VERBOSE_ACTION_ITEMS 0

/// Defines a default height for the workflow, all ActionItemViews can use it
#define NATURAL_ACTION_VIEW_HEIGHT 62


/// Class for managing boolean flags with associated signals. Using the state flag means you can pass in a state and be assured that the correct signals are emitted.
class AMBeamlineActionItemStateFlag : public QObject
{
Q_OBJECT
public:
	/// Constructor for StateFlag. Take an initial state for the flag (defaults to false)
	AMBeamlineActionItemStateFlag(bool initialState = false, QObject *parent = 0);

	/// Returns the current state (true or false)
	virtual bool state() const;

signals:
	/// Emitted when the StateFlag changes states (relays the new state)
	void stateChanged(bool changedTo);

public slots:
	/// Sets the StateFlag to the value state. Emits the stateChanged() signal if and only if the state actually changed (from true to false or false to true), nothing happens if the current state is true and true is passed in.
	void setState(bool state);

protected:
	/// Internal state flag, don't bother touching this
	bool state_;
};

class AMBeamlineActionItemView;

/// Base class for all actionItems.
class AMBeamlineActionItem : public QObject
{
Q_OBJECT
public:
	/// Default Constructor, calls initialize() to set all state flags to false
	explicit AMBeamlineActionItem(QObject *parent = 0);

	/// Returns state flag for initialized, true if and only if all other flags are false (nothing has happened yet)
	bool isInitialized() const;
	/* NTBA March 14, 2011 David Chevrier
	   Is this still necessary?
	*/
	bool isReinitialized() const;
	/// Returns state flag for ready, should be used to convey whether or not the action is ready to start. Can be used as a trigger for the circumstance when start() is called but we need to wait until the action is ready.
	bool isReady() const;
	/// Returns state flag for start, should be true when the action has started and should remain true even after the action has finished
	bool hasStarted() const;
	/// Returns state flag for start, should be true when the action has started but not yet finished (failed or succeeded)
	virtual bool isRunning() const;
	/// Returns state flag for succeeded, should be used to convey that the action is done and has achieved its goal
	bool hasSucceeded() const;
	/// Returns state flag for failed, should be used to convey that the action is done and has NOT achieved its goal
	bool hasFailed() const;
	/// Returns state flag for finished, should be used to convey that the action is done (whether succeeded or failed)
	bool hasFinished() const;
	/// Returns a pointer to the previous action (if the action is not in a parallel list)
	AMBeamlineActionItem* previous() const;
	/// Returns a pointer to the next action (if the action is not in a parallel list)
	AMBeamlineActionItem* next() const;

	virtual AMBeamlineActionItemView* createView(int index = 0) = 0;

	virtual QString message() const;

	bool hasHelp() const;
	AMOrderedSet<QString, QPixmap> helpImages() const;

signals:
	/// All signal emitting is taken care of if the setReady/setStarted/setSucceeded/etc functions are used. These functions will emit the signals as necessary
	/// Emitted at the end of the initial() function, conveys that all other flags are false (actionItem constructed, but nothing has happened yet)
	void initialized();
	/// Emitted when the ready state changes. Can change due to changes in the environment (ready might mean that a control is connected and not moving, at the control moves and stops the ready flag toggles to reflect this fact)
	void ready(bool ready);
	/* NTBA March 14, 2011 David Chevrier
	   Can we change this to a start() and startImplementation() structure for sub-classes?
	*/
	/// Emitted when the action is started (make sure sub-classes call setStart(true) in their start() implementation)
	void started();
	/// Emitted when the action has finished and achieved its goal
	void succeeded();
	/// Emitted when the action has finished and has not achieved its goal (the explanation parameter can be used to define error causes, using define statements in headers is a good way to do this)
	void failed(int explanation);
	/// Emitted when the action is done (whether it succeeded or failed)
	void finished();
	/// Should be emitted periodically to relay how much of the action has completed. Format: first argument is how much is done, second argument is the total to be done (could be % completed and 100%, for example)
	void progress(double, double);

public slots:
	/// Pure virtual. Sub-classes need to implement and they better call setStart(true) at some point causing started() to be emitted
	virtual void start() = 0;//
	//Calling reset always set reintialized to true
	// NTBA March 14, 2011 David Chevrier Is this still needed?
	virtual void reset(bool delayInitialize = false);//NOT Pure virtual. Sub-classes better call this at the end of their reset (AMBeamlineActionItem::reset) or call initialize themselves
	/* NTBA March 14, 2011 David Chevrier
	   This should be called somewhere or removed
	*/
	/// Pure virtual. Sub-classes need to implement in case there tasks to complete at the end of ana action.
	virtual void cleanup() = 0;
	/// Sets the previous action but does not connect signals and slots
	bool setPrevious(AMBeamlineActionItem* previous);
	/// Sets the next action but does not connect signals and slots
	bool setNext(AMBeamlineActionItem* next);

	void setMessage(const QString &message);
	void setHelp(const AMOrderedSet<QString, QPixmap> &helpImages);

protected slots:
	// Interface to internal state. If sub-classes want to change something, call these.
	// They will cause the corresponding signal to be emitted if the state changed (in the right direction)
	// They will also cause initialized to become false if they change from their initial state
	/// Sets ready state to isReady and emits ready() signal if necessary
	void setReady(bool isReady);
	/// Sets start state to isStarted and emits started() signal if necessary
	void setStarted(bool isStarted);
	/// Sets succeeded state to isSucceeded and emits succeeded() signal if necessary
	void setSucceeded(bool isSucceeded);
	/// Sets failed state to isFailed (with accompanying explanation if desired) and emit failed() signal (with explanation) if necessary
	void setFailed(bool isFailed, int explanation = -1);
	/// Sets finihsed state to isFinished and emits finished() signal if necessary
	void setFinished(bool isFinished);

	/* NTBA March 14, 2011 David Chevrier
	   Can we switch this over to initialize() and initializeImplementation() for sub-classes?
	*/
	/// NOT Pure virtual. Sub-classes better call this at the end of their initialize (AMBeamlineActionItem::initialize)
	virtual void initialize();

protected:
	/* NTBA March 14, 2011 David Chevrier
	   Is this still needed? It has to do with reinitialize and reset and such
	*/
	AMBeamlineActionItem(bool delayInitialize = false, QObject *parent = 0);

	/// Holds pointer to previous action (only meaningful if not in parallel list)
	AMBeamlineActionItem *previous_;
	/// Holds pointer to next action (only meaningful if not in parallel list)
	AMBeamlineActionItem *next_;

	QString message_;
	AMOrderedSet<QString, QPixmap> helpImages_;

private slots:
	/// Connected internally so that any other state change makes sure that initialized is set to false. Any change means we are no longer in the initialized state
	void dirtyInitialized();

private:
	/// Kept private, access with initialize(). Only true when initialized and nothing else has happened. Setting any flag below will change initialized to false
	AMBeamlineActionItemStateFlag initialized_;
	/* NTBA March 14, 2011 David Chevrier
	   Necessary?
	*/
	AMBeamlineActionItemStateFlag reinitialized_;//Stays false until reset is called
	/// Kept private, access with setReady(bool)
	AMBeamlineActionItemStateFlag ready_;
	/// Kept private, access with setStarted(bool)
	AMBeamlineActionItemStateFlag started_;
	/// Kept private, access with setSucceeded(bool)
	AMBeamlineActionItemStateFlag succeeded_;
	/// Kept private, access with setFailed(bool)
	AMBeamlineActionItemStateFlag failed_;
	/// Kept private, access with setFinished(bool)
	AMBeamlineActionItemStateFlag finished_;
};

class AMBeamlineActionItemView : public QFrame
{
	Q_OBJECT
public:
	AMBeamlineActionItemView(AMBeamlineActionItem *action, int index = 0, QWidget *parent = 0);

	int index() const { return index_;}
	virtual AMBeamlineActionItem* action();

public slots:
	virtual void setIndex(int index);
	virtual void setAction(AMBeamlineActionItem *action);
	virtual void defocusItem();

signals:
	void focusRequested(AMBeamlineActionItem *action);
	void removeRequested(AMBeamlineActionItem *action);
	void pauseRequested(AMBeamlineActionItem *action);
	void resumeRequested(AMBeamlineActionItem *action);
	void stopRequested(AMBeamlineActionItem *action);

protected slots:
	virtual void onInfoChanged() = 0;
	virtual void onStopCancelButtonClicked() = 0;
	virtual void onPlayPauseButtonClicked() = 0;

protected:
	void mousePressEvent(QMouseEvent *event);

	virtual void updateLook();

protected:
	AMBeamlineActionItem *action_;
	int index_;
	bool inFocus_;
};

class AMImageListView : public QWidget
{
Q_OBJECT
public:
	AMImageListView(const AMOrderedSet<QString, QPixmap> &images, QWidget *parent = 0);
};

#endif // AMBEAMLINEACTIONITEM_H
