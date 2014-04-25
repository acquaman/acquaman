#ifndef AMEXAFSSCANACTIONCONTROLLERASSEMBLER_H
#define AMEXAFSSCANACTIONCONTROLLERASSEMBLER_H

#include "acquaman/AMGenericScanActionControllerAssembler.h"
#include "dataman/AMScanAxisEXAFSRegion.h"

#define AMEXAFSSCANACTIONCONTROLLERASSEMBLER_AXES_INVALID 238000

/// Specific assembler for building an EXAFS scan controller.  Uses the specific AMScanAxisEXAFSRegion to flesh out the scan axis when switching to k-space.
class AMEXAFSScanActionControllerAssembler : public AMGenericScanActionControllerAssembler
{
	Q_OBJECT

public:
	/// Constructor.
	AMEXAFSScanActionControllerAssembler(QObject *parent = 0);
	/// Destructor.
	virtual ~AMEXAFSScanActionControllerAssembler() {}

protected:
	/// Re-implemented for EXAFS scans.  Builds the action tree required to make an EXAFS scan.
	virtual bool generateActionTreeImplmentation();

	/// Method that builds an EXAFS region.
	AMAction3 *generateActionTreeForEXAFSStepAxisRegion(AMControl *axisControl, AMScanAxisEXAFSRegion *exafsRegion, bool isFinalRegion);
};

#endif // AMEXAFSSCANACTIONCONTROLLERASSEMBLER_H
