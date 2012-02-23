#ifndef REIXSSAMPLEMANIPULATOR_H
#define REIXSSAMPLEMANIPULATOR_H

#include "beamline/AMSampleManipulator.h"

/// Implements the AMSampleManipulator interface for the REIXS beamline, so that AMSamplePlateView can measure and move to sample positions.  For measuring, we grab the sample manipulator positions from the beamline controls. For moving, we add a REIXSSampleMoveAction to the front of the workflow and run it (assuming nothing else is currently running. If something is running, we return that we cannot move right now.)
class REIXSSampleManipulator : public AMSampleManipulator {
public:
	/// Constructor
	REIXSSampleManipulator() : AMSampleManipulator() {}

	/// Returns the current position of the sample manipulator, as an AMControlInfoList.
	virtual AMControlInfoList position() const;
	/// Sends a request to move the sample manipulator into position \c newPosition. Returns true if the request is accepted; false if it cannot be started for any reason.
	virtual bool moveToPosition(const AMControlInfoList& newPosition);

	/// Returns the facility ID where this manipulator is found. It would be bad to try to move to sample positions from one facility when at another facility. The REIXS facility id is 5. \todo Make a more robust facility id registration system.
	virtual int facilityId() const { return 5; }

};

#endif // REIXSSAMPLEMANIPULATOR_H
