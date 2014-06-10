#ifndef VESPERSTIMESCANACTIONCONTROLLER_H
#define VESPERSTIMESCANACTIONCONTROLLER_H

#include "acquaman/AMTimedScanActionController.h"
#include "acquaman/VESPERS/VESPERSTimeScanConfiguration.h"

class VESPERSTimeScanActionController : public AMTimedScanActionController
{
	Q_OBJECT

public:
	VESPERSTimeScanActionController(VESPERSTimeScanConfiguration *configuration, QObject *parent = 0);
	virtual ~VESPERSTimeScanActionController(){}

protected:

	/// Adds anything extra (eg: analysis blocks) to the scan before it's started.
	virtual void buildScanControllerImplementation(){}

	/// Specific scan configuration with all the VESPERS specific information inside.
	VESPERSTimeScanConfiguration *configuration_;
};

#endif // VESPERSTIMESCANACTIONCONTROLLER_H
