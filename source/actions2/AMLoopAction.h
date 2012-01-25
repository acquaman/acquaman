#ifndef AMLOOPACTION_H
#define AMLOOPACTION_H

#include "actions2/AMNestedAction.h"

/// An AMLoopAction contains a list of sub-actions that, when the loop action is run, will be executed a fixed number of times.  It implements the AMNestedAction interface so that the sub-actions are visible inside the AMActionRunner views, so that users can drag-and-drop existing actions into/out-of the loop.
/*! The sub-actions are effectively used as templates, because a new copy of each sub-action will be made every time it is executed. You can configure whether you want the entire loop action logged as one, or every sub-action to be logged individually, by calling setShouldLogSubActionsSeparately(). */
class AMLoopAction : public AMNestedAction
{
    Q_OBJECT
public:
	/// Constructor.
	AMLoopAction(AMActionInfo* info, QObject *parent = 0);

	/// Copy constructor, to make a new runnable action just like \c other. Here we make a copy of all the sub-actions.
	AMLoopAction(const AMLoopAction& other);

	/// This virtual function takes the role of a virtual copy constructor.
	virtual AMAction* createCopy() const { return new AMLoopAction(*this); }

	/// Destructor: Deletes all the subActions_
	virtual ~AMLoopAction();



	// Public virtual functions to re-implement:
	//////////////////////

	/// Specifies whether, when logging actions that are run with AMActionRunner, to log the entire action, or log the individual sub-actions as they complete.
	virtual bool shouldLogSubActionsSeparately() { return logSubActionsSeparately_; }

	/// Returns the number of sub-actions inside the loop
	virtual int subActionCount() const { return subActions_.count(); }
	/// Returns the subAction at \c index within the loop.
	virtual const AMAction* subActionAt(int index) const { if(index<0 || index>=subActions_.count()) return 0; return subActions_.at(index); }
	/// Returns the subAction at \c index within the loop.
	virtual AMAction* subActionAt(int index) { if(index<0 || index>=subActions_.count()) return 0; return subActions_.at(index); }

	/// Returns the index of the currently-active subaction. Return -1 if it doesn't make sense to consider any as current, and -2 if all should be considered current.
	virtual int currentSubActionIndex() const { return currentSubAction_; }


	// new public functions:
	/////////////////////////////

	/// Set whether each individual sub-action should be logged separately as soon as it completes, or if the whole loop action should be logged as one long single action (in the user's action history log).
	void setShouldLogSubActionsSeparately(bool logSeparately) { logSubActionsSeparately_ = logSeparately; }

signals:

public slots:

protected:
	int loopCount_, currentLoop_, currentSubAction_;
	QList<AMAction*> subActions_;
	bool logSubActionsSeparately_;


	// Protected virtual functions to re-implement
	/////////////////////////////////////

	/// Implement to insert a sub- \c action at \c index. Can be assured by the base class that \c index is valid and that we're not running yet.
	virtual void insertSubActionImplementation(AMAction *action, int index);
	/// Implement to remove and return the sub-action at \c index. Can be assured by the base class that \c index is valid and that we're not running yet.
	virtual AMAction* takeSubActionImplementation(int index);
};

#endif // AMLOOPACTION_H
