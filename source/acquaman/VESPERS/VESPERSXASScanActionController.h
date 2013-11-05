#ifndef VESPERSXASSCANACTIONCONTROLLER_H
#define VESPERSXASSCANACTIONCONTROLLER_H

#include "acquaman/AMRegionScanActionController.h"
#include "acquaman/VESPERS/VESPERSEXAFSScanConfiguration.h"

class VESPERSXASScanActionController : public AMRegionScanActionController
{
	Q_OBJECT

public:
	VESPERSXASScanActionController(VESPERSEXAFSScanConfiguration *configuration, QObject *parent = 0);

protected:

	AMAction3* createInitializationActions();
	AMAction3* createCleanupActions();

	virtual void buildScanControllerImplementation() {}

protected:
	VESPERSEXAFSScanConfiguration *configuration_;
};

#endif // VESPERSXASSCANACTIONCONTROLLER_H
