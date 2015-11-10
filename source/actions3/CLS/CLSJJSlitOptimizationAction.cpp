#include "CLSJJSlitOptimizationAction.h"

CLSJJSlitOptimizationAction::CLSJJSlitOptimizationAction(CLSJJSlitOptimizationActionInfo *info, QObject *parent) :
	AMAction3(info, parent)
{
	AMGenericStepScanConfiguration *configuration = info->configuration();
	AMDetectorInfo detectorInfo = info->detectorInfo();

	if (configuration && detectorInfo != AMDetectorInfo() && configuration->detectorConfigurations().contains(detectorInfo)) {

		scanAction_ = new AMScanAction(new AMScanActionInfo(configuration->createCopy()));

		connect( scanAction_, SIGNAL(started()), this, SLOT(onScanActionStarted()) );
		connect( scanAction_, SIGNAL(cancelled()), this, SLOT(onScanActionCancelled()) );
		connect( scanAction_, SIGNAL(failed()), this, SLOT(onScanActionFailed()) );
		connect( scanAction_, SIGNAL(succeeded()), this, SLOT(onScanActionSucceeded()) );


	}
}
