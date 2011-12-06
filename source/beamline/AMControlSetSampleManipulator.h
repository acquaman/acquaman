#ifndef AMCONTROLSETSAMPLEMANIPULATOR_H
#define AMCONTROLSETSAMPLEMANIPULATOR_H

#include "beamline/AMSampleManipulator.h"

class AMControlSet;

/// This is a simple implementation of the AMSampleManipulator interface that uses an AMControlSet to directly read and move the sample position. More advanced implementations might want to use actions in conjunction with the workflow, so that sample switches appear in the user's history log, can be copied to run again, etc.
class AMControlSetSampleManipulator : public AMSampleManipulator
{
public:
	/// Construct a sample manipulator based on this \c controlSet.  The \c controlSet should exist as long as we are around and being used; we do not take ownership of it.
	explicit AMControlSetSampleManipulator(AMControlSet* controlSet);

	/// Returns the control set
	AMControlSet* controlSet() { return controlSet_; }

	/// Reimplemented to read the position from the control set
	virtual AMControlInfoList position() const;
	/// Reimplemented to start a move to \c newPosition. Returns false if the control set is not connected.
	virtual bool moveToPosition(const AMControlInfoList &newPosition);

protected:
	/// The control set
	AMControlSet* controlSet_;
};

#endif // AMCONTROLSETSAMPLEMANIPULATOR_H
