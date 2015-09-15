#include "CLSJJSlitOptimizationAction.h"
#include "acquaman/AMScanActionController.h"
#include "actions3/AMActionSupport.h"
#include "actions3/actions/AMScanAction.h"
#include "analysis/AM1DMaximumAB.h"
#include "beamline/CLS/CLSBeamline.h"
#include "util/AMErrorMonitor.h"

CLSJJSlitOptimizationAction::CLSJJSlitOptimizationAction(CLSJJSlitOptimizationActionInfo *info, QObject *parent) :
	AMListAction3(info, AMListAction3::Sequential, parent)
{
	// Initialize local variables.

	AMGenericStepScanConfiguration *configuration = info->configuration();
	AMControl *control = CLSBeamline::clsBeamline()->exposedControlByInfo(jjSlitOptimizationActionInfo()->configuration()->axisControlInfos().at(0));

	if (configuration && control) {

		// Create scan action, add to list of subactions.

		AMScanAction *scanAction = new AMScanAction(new AMScanActionInfo(configuration));
		AMScan *scan = scanAction->controller()->scan();
		AMDataSource *optimizationSource = scan->dataSourceAt(0); // find a way for this to be not hardcoded.

		AM1DMaximumAB *maximumAB = new AM1DMaximumAB("Maximum", this);
		maximumAB->setInputDataSources(QList<AMDataSource*>() << optimizationSource);

		scan->addAnalyzedDataSource(maximumAB, true, false);

		addSubAction(scanAction);

		// Once the scan is complete, we know the optimal position. Move control to optimal position.

		double optimalPosition = double(maximumAB->axisValue(AMnDIndex()));
		qDebug() << "Optimal position =" << optimalPosition;

		addSubAction(AMActionSupport::buildControlMoveAction(control, optimalPosition));

	} else {

		setFailed();
	}
}

CLSJJSlitOptimizationAction::CLSJJSlitOptimizationAction(const CLSJJSlitOptimizationAction &original) :
	AMListAction3(original)
{

}

CLSJJSlitOptimizationAction::~CLSJJSlitOptimizationAction()
{

}
