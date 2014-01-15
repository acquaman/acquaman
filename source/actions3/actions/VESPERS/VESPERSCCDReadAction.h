#ifndef VESPERSCCDREADACTION_H
#define VESPERSCCDREADACTION_H

#include "actions3/actions/AMDetectorReadAction.h"
#include "beamline/VESPERS/VESPERSCCDDetector.h"

/// This is actually a stripped down version of the normal AMDetectorReadAction because the area detectors on VESPERS only need the file number since the images are saved separately.
class VESPERSCCDReadAction : public AMDetectorReadAction
{
	Q_OBJECT

public:
	/// Constructor.  Requries and takes ownership of an existing AMDetectorReadActionInfo \c info.
	Q_INVOKABLE VESPERSCCDReadAction(AMDetectorReadActionInfo *info, AMDetector *detector = 0, QObject *parent = 0);
	/// Copy constructor.
	VESPERSCCDReadAction(const VESPERSCCDReadAction &other);
	/// Virtual copy constructor.
	virtual AMAction3 *createCopy() const { return new VESPERSCCDReadAction(*this); }

	/// Destructor.
	virtual ~VESPERSCCDReadAction();

protected:
	/// Re-implemented for the simpler CCD detector that isn't treated like a normal detector - data-wise.
	virtual void internalSetSucceeded();
};

#endif // VESPERSCCDREADACTION_H
