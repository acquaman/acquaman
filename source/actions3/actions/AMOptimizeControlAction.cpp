#include "AMOptimizeControlAction.h"

#include "acquaman/AMGenericStepScanConfiguration.h"
#include "acquaman/AMScanActionController.h"
#include "actions3/AMActionSupport.h"
#include "actions3/actions/AMScanAction.h"
#include "analysis/AM1DMaximumAB.h"
#include "beamline/AMBeamline.h"
#include "dataman/AMScan.h"
#include "util/AMErrorMonitor.h"

AMOptimizeControlAction::AMOptimizeControlAction(AMOptimizeControlActionInfo *info, QObject *parent) :
	AMListAction3(info, AMListAction3::Sequential, parent)
{
	// Initialize local variables.

	AMGenericStepScanConfiguration *configuration = new AMGenericStepScanConfiguration();
	AMControl *control = AMBeamline::bl()->exposedControlByInfo(info->control());

	if (configuration && control) {

		// Create scan action, add to list of subactions. Add analysis block according to the optimization option.

		AMScanAction *scanAction = new AMScanAction(new AMScanActionInfo(configuration));
		AMScan *scan = scanAction->controller()->scan();
		AMDataSource *optimizationSource = scan->dataSourceAt(0); // find a way for this to be not hardcoded. Info can provide?

		AMAnalysisBlock *optionAB = 0;

		if (info->option() == AMOptimizeControlActionInfo::Maximum) {
			AM1DMaximumAB *maximumAB = new AM1DMaximumAB("Maximum", this);
			maximumAB->setInputDataSources(QList<AMDataSource*>() << optimizationSource);

			optionAB = maximumAB;
		}

		scan->addAnalyzedDataSource(optionAB, true, false);

		addSubAction(scanAction);

		// Once the scan is complete, we know the optimal position. Move control to optimal position.

		double optimalPosition = double(optionAB->axisValue(AMnDIndex()));
		qDebug() << "Optimal position =" << optimalPosition;

		addSubAction(AMActionSupport::buildControlMoveAction(control, optimalPosition));

	} else {
		setFailed();
	}
}

AMOptimizeControlAction::AMOptimizeControlAction(const AMOptimizeControlAction &original) :
	AMListAction3(original)
{

}

AMOptimizeControlAction::~AMOptimizeControlAction()
{

}
