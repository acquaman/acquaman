#ifndef AMNESTEDACTION_H
#define AMNESTEDACTION_H

#include "actions2/AMAction.h"

/// AMNestedAction extends the AMAction interface for actions that want to make a hierarchy of sub-actions visible to users, and let users add to or remove sub-actions.  TODO
class AMNestedAction : public AMAction
{
    Q_OBJECT
public:	
	/// Constructor: create an action to run the specified AMActionInfo.
	AMNestedAction(AMActionInfo* info, QObject *parent = 0) : AMAction(info, parent) {}

	/// Copy constructor, to make a new runnable action just like \c other.
	AMNestedAction(const AMNestedAction& other) : AMAction(other) {}

//	/// This virtual function takes the role of a virtual copy constructor. All actions MUST RE-IMPLEMENT to be able to create and return a an independent copy of themselves. (This allows us to get a detailed subclass copy without knowing the type of the action.) NOTE that the returned instance should be a perfect copy except for the state() -- which will be reset to Constructed -- and any other subclass-specific state information: the copy should look like a new action that hasn't been run yet.
//	/*! It's recommended to make use of the copy constructor when implementing this, to ensure that the base class is copied properly.*/
//	virtual AMAction* createCopy() const { return new AMNestedAction(*this); }

	/// IMPORANT: subclasses should ensure to delete their subActions in their own destructor
	virtual ~AMNestedAction() {}

	// Additional public functions:
	/////////////////////////////////

	/// Public interface to insert a sub-action at \c index.  Takes care of emitting signals subActionAboutToBeAdded(), and subActionAdded().  If \c index < 0 or > subActionCount(), then appends action to end.  Only allowed while the action is in the AMAction::Constructed state... Once it's running, you can no longer insert sub-actions and this will return false.
	bool insertSubAction(AMAction* action, int index);
	/// Public interface to delete the sub-action at \c index. Takes care of emitting signals subActionAboutToBeRemoved(), and subActionRemoved(). If \c index >= subActionCount(), returns false.  Only allowed while the action is in the AMAction::Constructed state... Once it's running, you can no longer remove sub-actions and this will return false.
	bool deleteSubAction(int index);

	/// Public interface to remove the sub-action at \c index, but don't delete it.  Ownership of the sub-action becomes the responsibility of the caller, and the sub-action's parentAction() is set back to 0.    Only allowed while the action is in the AMAction::Constructed state... Once it's running, you can no longer remove sub-actions and this will return 0.  Also returns 0 if \c index is out of range.
	AMAction* takeSubAction(int index);

	/// Public function to duplicate a set of sub-actions at \c indexesToCopy. The new sub-actions will all be inserted at the position after the last existing sub-action in \c indexesToCopy.  It uses insertSubAction() to add copies of the existing ones, and therefore will fail if the action is not in the AMAction::Constructed state.
	bool duplicateSubActions(const QList<int>& indexesToCopy);


	/// Find the index of a sub-action \c action, or -1 if not found.
	int indexOfSubAction(const AMAction* action) const;


	// Public virtual functions that must be re-implemented:
	//////////////////////

	/// Specifies whether, when logging actions that are run with AMActionRunner, to log the entire action, or log the individual sub-actions as they complete.
	virtual bool shouldLogSubActionsSeparately() { return true; }

	/// Returns the number of sub-actions
	virtual int subActionCount() const = 0;
	/// Returns the sub-action at \c index, or 0 if \c index is out of range.
	virtual const AMAction* subActionAt(int index) const = 0;
	/// Returns the sub-action at \c index, or 0 if \c index is out of range.
	virtual AMAction* subActionAt(int index) = 0;

	/// Returns the index of the currently-active subaction. Return -1 if it doesn't make sense to consider any as current, and -2 if all should be considered current.
	virtual int currentSubActionIndex() const = 0;

signals:

	// These signals should be emitted by implementations
	/////////////////////////////////////////

	/// Emitted when a sub-action completes.
	void subActionCompleted(AMAction* completedAction);
	/// Emitted when the current sub-action changes. \c newSubActionIndex can be from 0 to subActionCount()-1.  This can change in any order... For example, AMLoopAction might go from the last subAction back to the first subAction.  Send -1 if it doesn't make sense to consider any as current, and -2 if all should be considered current.
	void currentSubActionChanged(int newSubActionIndex);

	// Emitted by the base class
	/////////////////////////////

	/// Emitted right before a sub-action is added at \c index
	void subActionAboutToBeAdded(int index);
	/// Emitted right after a sub-action is added at \c index
	void subActionAdded(int index);
	/// Emitted right before the sub-action that is now at \c index will be removed
	void subActionAboutToBeRemoved(int index);
	/// Emitted right after the sub-action that used to be at \c index was removed.
	void subActionRemoved(int index);

public slots:


protected:
	// Protected virtual functions to re-implement
	/////////////////////////////////////

	/// Implement this to insert a sub-action \c action at \c index. You can assume \c index is valid. Don't emit subActionAboutToBeAdded(), etc. because the base class takes care of this for you.
	virtual void insertSubActionImplementation(AMAction *action, int index) = 0;
	/// Implement this to remove and return the sub-action at \c index. You can assume index is valid. Don't emit subActionAboutToBeRemoved(), etc. because the base class takes care of this for you.
	virtual AMAction* takeSubActionImplementation(int index) = 0;

};

#endif // AMNESTEDACTION_H
