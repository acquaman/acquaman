#ifndef AMACTIONRUNNER_H
#define AMACTIONRUNNER_H

#include <QObject>
#include <Qlist>
#include <QStandardItemModel>
#include <QStandardItem>

#include <QDebug>

class AMAction;

/// This subclass of QStandardItem is used to represent actions in the AMActionRunner queue.  You should never need to use this class directly.
/*! We use a possibly hierarchical model so that we can support nesting of "For-loop" actions at some point.*/
class AMActionModelItem : public QStandardItem {
public:
	enum Type { AMActionModelItemType = QStandardItem::UserType + 1002 };

	AMActionModelItem(AMAction* action);

	AMAction* action() { return action_; }

	virtual QVariant data(int role) const;

	virtual int type() const { return AMActionModelItemType; }

	virtual QStandardItem* clone() const {
		qDebug() << "AMActionModelItem: Is clone() ever used? Apparently.";
		return new AMActionModelItem(action_);
	}
protected:
	AMAction* action_;
};

/// This singleton class takes the role of a workflow manager. You can queue up actions for it to run, manage the queue of upcoming actions, and receive notifications when actions are completed. After an action is completed, it will automatically log the actions with AMActionHistoryLogger.
/*! Note that the AMActionRunner's queue is initially paused when it is first created. To have it start executing actions as soon as they become available, call AMActionRunner::s()->setPaused(false).*/
class AMActionRunner : public QObject
{
    Q_OBJECT
public:
	/// This is a singleton class. You access the only instance of it using AMActionRunner::s().
	static AMActionRunner* s();
	/// Release and delete the singleton instance
	static void releaseActionRunner();

	// The Queue of Upcoming Actions:
	///////////////////////////////////

	/// The number of actions that are in the queue. (Does not include the possible current action.)
	int queuedActionCount() const { return queueModel_.rowCount(); }
	/// Returns a pointer to an action in the queue, or 0 if the index is out of range.
	const AMAction* queuedActionAt(int index) const { AMActionModelItem* item = static_cast<AMActionModelItem*>(queueModel_.item(index)); if(item) return item->action(); return 0; }
	/// Returns a pointer to an action in the queue, or 0 if the index is out of range.
	AMAction* queuedActionAt(int index) { AMActionModelItem* item = static_cast<AMActionModelItem*>(queueModel_.item(index)); if(item) return item->action(); return 0; }

	/// Add an action to the end of the queue. This class takes ownership of the \c action and will log and delete it after running it.
	void addActionToQueue(AMAction* action) { insertActionInQueue(action, -1); }
	/// Insert an action at a position \c index in the queue.  If \c index is -1 or > queuedActionCount(), will append to the end of the queue. If \c index is 0, will insert at the beginning of the queue. This class takes ownership of the \c action and will log and delete it after running it.
	void insertActionInQueue(AMAction* action, int index);
	/// Delete the action at \c index in the queue. This will remove it from the queue and delete the action object. Returns false if \c index out of range.
	bool deleteActionInQueue(int index);
	/// Duplicate an action in the queue. The new copy will be inserted immediately after \c index. Returns false if \c index out of range.
	bool duplicateActionInQueue(int index);
	/// Duplicate a set of actions in the queue. The new copies will be inserted, in their original order, immediately after the last index in \c indexesToCopy. Returns false if any in \c indexesToCopy are out of range.
	bool duplicateActionsInQueue(const QList<int>& indexesToCopy);
	/// Move an action up or down the queue. \c currentIndex is the action to move, and \c finalIndex is the index you want it to end up in when everything is done.  (\c finalIndex can be 0 for the beginning, or -1 to go to the end.) Returns false if \c currentIndex is out of range, or if \c currentIndex == \c finalIndex.
	bool moveActionInQueue(int currentIndex, int finalIndex);



	/// Whether the queue is paused or running. If the queue is running, it will advance automatically to the next action whenever there are actions in the queue.  \see setQueuePaused().
	bool queuePaused() const { return isPaused_; }

	/// This QStandardItemModel is full of AMActionModelItem items, and a useful model for views of the AMActionRunner's queue.
	QStandardItemModel* queueModel() { return &queueModel_; }


	// The Current Action that we (might be)/are executing
	////////////////////////////

	/// Whether there is an action currently running.
	bool actionRunning() const { return currentAction_ != 0; }
	/// A pointer to the current action. Will be 0 if actionRunning() is false, when no action is running.
	AMAction* currentAction() { return currentAction_; }
	/// A pointer to the current action. Will be 0 if actionRunning() is false, when no action is running.
	const AMAction* currentAction() const { return currentAction_; }


	// Immediate-mode interface
	////////////////////////////////

	/// Instead of going through the queue, it's possible to ask the Action Runner to take an action and run it immediately.  Immediate actions will be run in parallel/independently of the currentAction() if there is one, (although they might wait for their prerequisites to be satisfied, just like any other action.)  When the action finishes, the Action Runner will take care of logging and deleting it.  If it fails, the action's failureResponseInActionRunner() will be respected.
	void runActionImmediately(AMAction* action);


signals:

	// Signals regarding the currently-running item
	////////////////////////////

	/// Emitted whenever the currently-running action changes.  If the queue reaches the end and is empty, this will be emitted with 0 (for no current action).
	void currentActionChanged(AMAction* action);

	// These signals are useful to views of the current action. This saves them from having to connect and disconnect
	/// Forwards the progressChanged() signal from the current action, for convenience
	void currentActionProgressChanged(double numerator, double denominator);
	/// Forward the statusTextChanged() signal from the current action, for convenience.
	void currentActionStatusTextChanged(const QString& status);
	/// Forward the expectedDurationChanged() signal from the current action, for convenience
	void currentActionExpectedDurationChanged(double expectedSecondsTotal);

	// Signals regarding the state of the queue.
	////////////////////////////

	/// This signal is emitted whenever the paused/running state of the queue changes. ie: queuePaused() changes.
	void queuePausedChanged(bool isPaused);

public slots:

	/// Set whether the queue is paused or running.  If the queue is running, it will advance automatically to the next action whenever there are actions in the queue, or when an action is added to an empty queue. Note that setting the queue to paused does not pause the current action... It only pauses the workflow from moving on to the <i>next</i> action after the current one is completed.
	void setQueuePaused(bool isPaused);


protected slots:
	/// Respond internally whenever the state of the currently-running action changes.
	void onCurrentActionStateChanged(int newState, int previousState);

	/// Respond internally whenever the state of any immediate-run action changes.
	void onImmediateActionStateChanged(int newState, int previousState);


protected:
	QStandardItemModel queueModel_;
	AMAction* currentAction_;
	bool isPaused_;

	/// Helper function called when the previous action finishes. If the queue is running and there are more actions, it starts the next one. If the queue is paused or if we're out of actions, it sets the currentAction() to 0. Either way, it emits the currentActionChanged() signal.
	void internalDoNextAction();
	/// Helper function to prompt the user about what to do given that the current action failed, and it specified a "prompt user" failure response. Do they want to retry or move on?
	int internalAskUserWhatToDoAboutFailedAction(AMAction* action);

private:
	/// This is a singleton class, so the constructor is private. Access the only instance of it via s().
	explicit AMActionRunner(QObject *parent = 0);
	/// Destructor. Deletes any actions in the queue. For implementation reasons, does not delete the currently running action, because it might take this action a while to stop and clean-up.
	~AMActionRunner();

	/// The singleton instance
	static AMActionRunner* instance_;

};

#endif // AMACTIONRUNNER_H
