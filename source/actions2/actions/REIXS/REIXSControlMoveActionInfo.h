#ifndef REIXSCONTROLMOVEACTIONINFO_H
#define REIXSCONTROLMOVEACTIONINFO_H

#include "actions2/actions/AMControlMoveActionInfo.h"

/// This trivial subclass of AMControlMoveActionInfo is used to verify that (when looking at a stored move action in the database) we know that it came from and can be executed on the REIXS beamline. It provides no new functionality; it simply distinguishes instances from the general AMControlMoveActionInfo.
class REIXSControlMoveActionInfo : public AMControlMoveActionInfo
{
    Q_OBJECT
public:
	/// Constructor. You should always specify a valid \c setpoint, but we provide the default argument because we need a default constructor for database loading.
	Q_INVOKABLE REIXSControlMoveActionInfo(const AMControlInfo& setpoint = AMControlInfo(), QObject *parent = 0) : AMControlMoveActionInfo(setpoint, parent) {}

	/// Copy Constructor
	REIXSControlMoveActionInfo(const REIXSControlMoveActionInfo& other) : AMControlMoveActionInfo(other) {}

	/// This function is used as a virtual copy constructor
	virtual REIXSControlMoveActionInfo* createCopy() const { return new REIXSControlMoveActionInfo(*this); }

	// Re-implemented public functions
	/////////////////////////////////

	/// This should describe the type of the action
	virtual QString typeDescription() const { return "REIXS Control Move"; }

};

#endif // REIXSCONTROLMOVEACTIONINFO_H
