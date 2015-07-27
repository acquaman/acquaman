#ifndef CLSJJSLITSCENTEROPTIMIZATIONSCANCONTROLLER_H
#define CLSJJSLITSCENTEROPTIMIZATIONSCANCONTROLLER_H

#include "acquaman/AMGenericStepScanController.h"
#include "acquaman/CLS/CLSJJSlitsCenterOptimizationScanConfiguration.h"

class CLSJJSlitsCenterOptimizationScanController : public AMGenericStepScanController
{
    Q_OBJECT

public:
	/// Constructor.
	explicit CLSJJSlitsCenterOptimizationScanController(CLSJJSlitsCenterOptimizationScanConfiguration *configuration, QObject *parent = 0);
	/// Destructor.
	virtual ~CLSJJSlitsCenterOptimizationScanController();

protected:
	/// Reimplemented to provide initialization actions to perform before the scan starts.
	AMAction3* createInitializationActions();
	/// Reimplemented to provide cleanup actions to perform after the scan is complete.
	AMAction3* createCleanupActions();

protected:
	/// The scan configuration.
	CLSJJSlitsCenterOptimizationScanConfiguration *jjSlitsScanConfiguration_;
};

#endif // CLSJJSLITSCENTEROPTIMIZATIONSCANCONTROLLER_H
