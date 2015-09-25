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
	AMAction3(info, parent)
{
	// Initialize local variables.

	scanAction_ = 0;
	controlMoveAction_ = 0;
}

AMOptimizeControlAction::AMOptimizeControlAction(const AMOptimizeControlAction &original) :
	AMAction3(original)
{
	scanAction_ = 0;
	controlMoveAction_ = 0;
}

AMOptimizeControlAction::~AMOptimizeControlAction()
{

}

void AMOptimizeControlAction::onScanActionStarted()
{
	const AMOptimizeControlActionInfo *info = optimizeControlActionInfo();

	if (info && scanAction_) {

		// Set up the mechanism for finding the optimal control value.

		AMScan *scan = scanAction_->controller()->scan();
		scan->addAnalyzedDataSource(info->optimizationAB(), true, false);
	}
}

void AMOptimizeControlAction::onScanActionSucceeded()
{
	double optimalPosition = 0;
	bool optimalPositionFound = false;
	const AMOptimizeControlActionInfo *info = optimizeControlActionInfo();
	AMScanConfiguration *configuration = 0;

	// Once the scan is complete, we can find the optimal position.

	if (info) {
		configuration = info->configuration();

		AMAnalysisBlock *optimizationAB = info->optimizationAB();

		if (optimizationAB) {
			optimalPosition = double(optimizationAB->axisValue(0, 0));
			optimalPositionFound = true;
			qDebug() << "Optimal position =" << optimalPosition;
		}
	}

	// Create new control move action to the optimal position, the second of two components to this action.

	if (configuration && optimalPositionFound)
		controlMoveAction_ = createControlMoveAction(configuration, optimalPosition);

	// Start the move action.

	if (controlMoveAction_)
		controlMoveAction_->start();
	else
		setFailed();
}

void AMOptimizeControlAction::onControlMoveActionSucceeded()
{
	// Delete scan action.

	if (scanAction_) {
		scanAction_->disconnect();
		scanAction_->deleteLater();
		scanAction_ = 0;
	}

	// Delete control move action.

	if (controlMoveAction_) {
		controlMoveAction_->disconnect();
		controlMoveAction_->deleteLater();
		controlMoveAction_ = 0;
	}

	// Notify listeners that this action has succeeded.

	setSucceeded();
}

void AMOptimizeControlAction::onChildActionCancelled()
{
	// Forward cancelled status.

	if (scanAction_ && !scanAction_->inFinalState())
		scanAction_->cancel();

	if (controlMoveAction_ && !controlMoveAction_->inFinalState())
		controlMoveAction_->cancel();

	// Check status of both child actions.

	bool scanActionOK = false;
	bool controlMoveActionOK = false;

	if (!scanAction_ || scanAction_ && scanAction_->inFinalState())
		scanActionOK = true;

	if (!controlMoveAction_ || controlMoveAction_ && controlMoveAction_->inFinalState())
		controlMoveActionOK = true;

	// Notify listeners that this action has been cancelled.

	if (scanActionOK && controlMoveActionOK)
		setCancelled();
}

void AMOptimizeControlAction::onChildActionFailed()
{
	// Forward failed status.

	if (scanAction_ && !scanAction_->inFinalState())
		scanAction_->cancel();

	if (controlMoveAction_ && !controlMoveAction_->inFinalState())
		controlMoveAction_->cancel();

	// Check status of both child actions.

	bool scanActionOK = false;
	bool controlMoveActionOK = false;

	if (!scanAction_ || scanAction_ && scanAction_->inFinalState())
		scanActionOK = true;

	if (!controlMoveAction_ || controlMoveAction_ && controlMoveAction_->inFinalState())
		controlMoveActionOK = true;

	// Notify listeners that this action has failed.

	if (scanActionOK && controlMoveActionOK)
		setFailed();
}

void AMOptimizeControlAction::onChildActionPaused()
{
	// Forward paused status.

	if (scanAction_ && !scanAction_->inFinalState())
		scanAction_->pause();

	if (controlMoveAction_ && !controlMoveAction_->inFinalState())
		controlMoveAction_->pause();

	// Check status of both child actions.

	bool scanActionOK = false;
	bool controlMoveActionOK = false;

	if (!scanAction_ || scanAction_ && scanAction_->state() == Paused || scanAction_ && scanAction_->inFinalState())
		scanActionOK = true;

	if (!controlMoveAction_ || controlMoveAction_ && controlMoveAction_->state() == Paused || controlMoveAction_ && controlMoveAction_->inFinalState())
		controlMoveActionOK = true;

	// Notify listeners that this action has been paused.

	if (scanActionOK && controlMoveActionOK)
		setPaused();
}

void AMOptimizeControlAction::onChildActionResumed()
{
	// Forward resumed status.

	if (scanAction_ && !scanAction_->inFinalState())
		scanAction_->resume();

	if (controlMoveAction_ && !controlMoveAction_->inFinalState())
		controlMoveAction_->resume();

	// Check status of both child actions.

	bool scanActionOK = false;
	bool controlMoveActionOK = false;

	if (!scanAction_ || scanAction_ && scanAction_->state() == Running || scanAction_ && scanAction_->inFinalState())
		scanActionOK = true;

	if (!controlMoveAction_ || controlMoveAction_ && controlMoveAction_->state() == Running || controlMoveAction_ && controlMoveAction_->inFinalState())
		controlMoveActionOK = true;

	// Notify listeners that this action has resumed.

	if (scanActionOK && controlMoveActionOK)
		setResumed();
}

void AMOptimizeControlAction::startImplementation()
{
	// If there is an existing scanAction_, delete it.

	if (scanAction_) {
		scanAction_->disconnect();
		scanAction_->deleteLater();
		scanAction_ = 0;
	}

	// If there is an existing control move action, delete it.

	if (controlMoveAction_) {
		controlMoveAction_->disconnect();
		controlMoveAction_->deleteLater();
		controlMoveAction_ = 0;
	}

	// Create new scan action, the first of two components to this action.

	const AMOptimizeControlActionInfo *info = optimizeControlActionInfo();

	if (info)
		scanAction_ = createScanAction(info->configuration());

	// Start the scan action.
	// If either the info or the configuration provided are invalid, this action fails.

	if (scanAction_)
		scanAction_->start();
	else
		setFailed();
}

void AMOptimizeControlAction::resumeImplementation()
{
	if (scanAction_ && !scanAction_->inFinalState())
		scanAction_->resume();

	if (controlMoveAction_ && !controlMoveAction_->inFinalState())
		controlMoveAction_->resume();
}

void AMOptimizeControlAction::cancelImplementation()
{
	if (scanAction_ && !scanAction_->inFinalState())
		scanAction_->cancel();

	if (controlMoveAction_ && !controlMoveAction_->inFinalState())
		controlMoveAction_->cancel();
}

AMScanAction* AMOptimizeControlAction::createScanAction(AMScanConfiguration *configuration)
{
	AMScanAction *scanAction = 0;

	if (configuration) {
		scanAction = new AMScanAction(new AMScanActionInfo(configuration), this);

		connect( scanAction, SIGNAL(started()), this, SLOT(onScanActionStarted()) );
		connect( scanAction, SIGNAL(cancelled()), this, SLOT(onChildActionCancelled()) );
		connect( scanAction, SIGNAL(failed()), this, SLOT(onChildActionFailed()) );
		connect( scanAction, SIGNAL(paused()), this, SLOT(onChildActionPaused()) );
		connect( scanAction, SIGNAL(resumed()), this, SLOT(onChildActionResumed()) );
		connect( scanAction, SIGNAL(succeeded()), this, SLOT(onScanActionSucceeded()) );
	}

	return scanAction;
}

AMAction3* AMOptimizeControlAction::createControlMoveAction(AMScanConfiguration *configuration, double optimalPosition)
{
	AMAction3 *controlMoveAction = 0;

	if (configuration && !configuration->axisControlInfos().isEmpty()) {
		AMControl *control = AMBeamline::bl()->exposedControlByInfo(configuration->axisControlInfos().at(0));
		controlMoveAction = AMActionSupport::buildControlMoveAction(control, optimalPosition);

		connect( controlMoveAction, SIGNAL(cancelled()), this, SLOT(onChildActionCancelled()) );
		connect( controlMoveAction, SIGNAL(failed()), this, SLOT(onChildActionFailed()) );
		connect( controlMoveAction, SIGNAL(paused()), this, SLOT(onChildActionPaused()) );
		connect( controlMoveAction, SIGNAL(resumed()), this, SLOT(onChildActionResumed()) );
		connect( controlMoveAction, SIGNAL(succeeded()), this, SLOT(onControlMoveActionSucceeded()) );
	}

	return controlMoveAction;
}
