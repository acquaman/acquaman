#ifndef CLSJJSLITOPTIMIZATIONACTION_H
#define CLSJJSLITOPTIMIZATIONACTION_H

#include "actions3/AMListAction3.h"
#include "actions3/CLS/CLSJJSlitOptimizationActionInfo.h"

class CLSJJSlitOptimizationAction : public AMListAction3
{
    Q_OBJECT

public:
	/// Constructor.
	explicit CLSJJSlitOptimizationAction(CLSJJSlitOptimizationActionInfo *info, QObject *parent = 0);
	/// Copy constructor.
	CLSJJSlitOptimizationAction(const CLSJJSlitOptimizationAction &original);
	/// Destructor.
	virtual ~CLSJJSlitOptimizationAction();

	/// Returns a new AMAction3 that's a copy of this one.
	virtual AMAction3* createCopy() const { return new CLSJJSlitOptimizationAction(*this); }

	/// Returns whether this action can be paused, false for this action.
	virtual bool canPause() const { return false; }
	/// Returns whether this action can be skipped, false for this action.
	virtual bool canSkip() const { return false; }

protected:
	/// Returns the specific action info for this class.
	const CLSJJSlitOptimizationActionInfo* jjSlitOptimizationActionInfo() const { return qobject_cast<const CLSJJSlitOptimizationActionInfo*>(info()); }
	/// Returns the specific action info for this class.
	const CLSJJSlitOptimizationActionInfo* jjSlitOptimizationActionInfo() { return qobject_cast<CLSJJSlitOptimizationActionInfo*>(info()); }
};

#endif // CLSJJSLITOPTIMIZATIONACTION_H
