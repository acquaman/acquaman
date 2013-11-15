#ifndef REIXSXASSCANACTIONCONTROLLER_H
#define REIXSXASSCANACTIONCONTROLLER_H

#include "acquaman/AMRegionScanActionController.h"
#include "acquaman/REIXS/REIXSXASScanConfiguration.h"
#include "dataman/AMUser.h"
#include "actions3/AMAction3.h"

class REIXSXASScanActionController : public AMRegionScanActionController
{
Q_OBJECT
public:
	REIXSXASScanActionController(REIXSXASScanConfiguration *cfg, QObject *parent = 0);

protected:
	/// Adds anything extra (eg: analysis blocks) to the scan before it's started.
	virtual void buildScanControllerImplementation() {}

	AMAction3* createInitializationActions();
	AMAction3* createCleanupActions();

protected:
	REIXSXASScanConfiguration *configuration_;
};

#endif // REIXSXASSCANACTIONCONTROLLER_H
