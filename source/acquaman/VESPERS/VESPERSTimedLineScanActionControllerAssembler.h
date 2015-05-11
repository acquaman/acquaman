#ifndef VESPERSTIMEDLINESCANACTIONCONTROLLERASSEMBLER_H
#define VESPERSTIMEDLINESCANACTIONCONTROLLERASSEMBLER_H

#include "acquaman/AMGenericScanActionControllerAssembler.h"

/// Extension of the generic scan action controller assembler that creates a timed line scan.
class VESPERSTimedLineScanActionControllerAssembler : public AMGenericScanActionControllerAssembler
{
	Q_OBJECT

public:
	/// Constructor.
	VESPERSTimedLineScanActionControllerAssembler(int iterations, double timeoutTime, QObject *parent = 0);
	/// Destructor.
	virtual ~VESPERSTimedLineScanActionControllerAssembler() { }

protected:
	/// Does all the work of building the action tree.
	virtual bool generateActionTreeImplmentation();

	/// The timeout time.
	double timeoutTime_;
	/// The number of iterations.
	int iterations_;
};

#endif // VESPERSTIMEDLINESCANACTIONCONTROLLERASSEMBLER_H
