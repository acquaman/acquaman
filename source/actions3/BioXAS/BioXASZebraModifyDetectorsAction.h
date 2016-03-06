#ifndef BIOXASZEBRAMODIFYDETECTORSACTION_H
#define BIOXASZEBRAMODIFYDETECTORSACTION_H

#include "actions3/AMAction3.h"
#include "actions3/BioXAS/BioXASZebraModifyDetectorsActionInfo.h"

#define BIOXASZEBRAMODIFYDETECTORSACTION_INVALID_ZEBRA 98820
#define BIOXASZEBRAMODIFYDETECTORSACTION_INVALID_ACTION 98821
#define BIOXASZEBRAMODIFYDETECTORSACTION_INVALID_DETECTOR 98822
#define BIOXASZEBRAMODIFYDETECTORSACTION_ACTION_FAILED 98823

class BioXASZebraModifyDetectorsAction : public AMAction3
{
    Q_OBJECT

public:
	/// Constructor.
	Q_INVOKABLE BioXASZebraModifyDetectorsAction(BioXASZebraModifyDetectorsActionInfo *info, QObject *parent = 0);
	/// Copy constructor.
	BioXASZebraModifyDetectorsAction(const BioXASZebraModifyDetectorsAction &original);
	/// Destructor.
	virtual ~BioXASZebraModifyDetectorsAction();

	/// Virtual copy constructor.
	virtual AMAction3* createCopy() const { return new BioXASZebraModifyDetectorsAction(*this); }

	/// Specify that we cannot pause (since AMControl cannot pause).  If we wanted to get fancy, we might check if the control could stop, (and stop it for pause, and then start it again to resume). But this is much simpler for now.
	virtual bool canPause() const { return false; }
	/// This action cannot skip.
	virtual bool canSkip() const { return false; }

	/// Virtual function that denotes that this action has children underneath it or not.
	virtual bool hasChildren() const { return false; }
	/// Virtual function that returns the number of children for this action.
	virtual int numberOfChildren() const { return 0; }

	/// Returns the action info specific for this action.
	const BioXASZebraModifyDetectorsActionInfo* modifyDetectorsInfo() const { return qobject_cast<const BioXASZebraModifyDetectorsActionInfo*>(info()); }
	/// Returns the action info specific for this action.
	BioXASZebraModifyDetectorsActionInfo* modifyDetectorsInfo() { return qobject_cast<BioXASZebraModifyDetectorsActionInfo*>(info()); }

protected:
	/// Returns true if the given option is supported, false otherwise.
	bool supportedOption(int action) const;

	/// This function is called from the Starting state when the implementation should initiate the action.
	virtual void startImplementation();
	/// For actions which support pausing, this function is called from the Pausing state when the implementation should pause the action.
	virtual void pauseImplementation() { setPaused(); }
	/// For actions that support resuming, this function is called from the Paused state when the implementation should resume the action.
	virtual void resumeImplementation() { setResumed(); }
	/// All implementations must support cancelling. This function will be called from the Cancelling state when the implementation should cancel the action.
	virtual void cancelImplementation() { setCancelled(); }
	/// The function is called from the Skipping state when the implementation should skip the action. This implementation does not support skipping.
	virtual void skipImplementation(const QString &command) { Q_UNUSED(command); }
};

#endif // BIOXASZEBRAMODIFYDETECTORSACTION_H
