#ifndef CLSJJSLITSOPTIMIZATIONACTION_H
#define CLSJJSLITSOPTIMIZATIONACTION_H

#include "actions3/AMAction3.h"
#include "actions3/CLS/CLSJJSlitsOptimizationActionInfo.h"

class CLSJJSlitsOptimizationAction : public AMAction3
{
    Q_OBJECT

public:
	/// Constructor.
	explicit CLSJJSlitsOptimizationAction(CLSJJSlitsOptimizationActionInfo *info, QObject *parent = 0);
	/// Copy constructor.
	CLSJJSlitsOptimizationAction(const CLSJJSlitsOptimizationAction &original);
	/// Destructor.
	virtual ~CLSJJSlitsOptimizationAction();

	/// Returns a new AMAction3 that's a copy of this one.
	virtual AMAction3* createCopy() const { return new CLSJJSlitsOptimizationAction(*this); }

	/// Returns whether this action can be paused, false for this action.
	virtual bool canPause() const { return false; }
	/// Returns whether this action can be skipped, false for this action.
	virtual bool canSkip() const { return false; }

protected:
	/// Returns the specific action info for this class.
	const CLSJJSlitsOptimizationActionInfo* jjSlitsOptimizationActionInfo() const { return qobject_cast<const CLSJJSlitsOptimizationActionInfo*>(info()); }
	/// Returns the specific action info for this class.
	const CLSJJSlitsOptimizationActionInfo* jjSlitsOptimizationActionInfo() { return qobject_cast<CLSJJSlitsOptimizationActionInfo*>(info()); }
};

#endif // CLSJJSLITSOPTIMIZATIONACTION_H
