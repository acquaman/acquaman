#include "CLSJJSlitOptimizationAction.h"
#include "beamline/CLS/CLSBeamline.h"
#include "util/AMUtility.h"
#include "dataman/AMScan.h"
#include "acquaman/AMScanActionController.h"
#include "actions3/AMActionSupport.h"
#include "util/AMErrorMonitor.h"

CLSJJSlitOptimizationAction::CLSJJSlitOptimizationAction(CLSJJSlitOptimizationActionInfo *info, QObject *parent) :
	AMAction3(info, parent)
{	
	jjSlits_ = 0;
	configuration_ = 0;
	control_ = 0;

	scanAction_ = 0;
	moveAction_ = 0;
}

CLSJJSlitOptimizationAction::CLSJJSlitOptimizationAction(const CLSJJSlitOptimizationAction &original) :
	AMAction3(original)
{
	jjSlits_ = CLSBeamline::clsBeamline()->jjSlits();
	configuration_ = qobject_cast<CLSJJSlitOptimizationActionInfo*>(original.info()).
}

CLSJJSlitOptimizationAction::~CLSJJSlitOptimizationAction()
{

}

void CLSJJSlitOptimizationAction::onScanActionStarted()
{
	setStarted();
}

void CLSJJSlitOptimizationAction::onScanActionCancelled()
{
	setCancelled();
	onScanActionFinished();
}

void CLSJJSlitOptimizationAction::onScanActionFailed()
{
	setFailed();
	onScanActionFinished();
}

void CLSJJSlitOptimizationAction::onScanActionSucceeded()
{
	// Find optimal control position.

	AMNumber optimalPosition = findOptimalPosition(0);

	if (optimalPosition == AMNumber::InvalidError) {
		AMErrorMon::alert(this, CLSJJSLITOPTIMIZATIONACTION_INVALID_OPTIMAL_POSITION, "Unable to complete optimization procedure: the optimal position found was invalid.");
		onMoveActionFailed();

	} else {

		// Note the value and axis value results in the scan notes.

		AMScan *scan = scanAction_->controller()->scan();

		QString notes;
		QString maxNotes = "No max info.";

		if (maxValue.isValid() && maxAxisValue.isValid()) {
			maxNotes = control_->name() + " max: " + QString::number(double(maxValue));
			maxNotes += "\nAxis value: " + QString::number(double(maxAxisValue));
		}

		if (scan->notes().isEmpty())
			notes = maxNotes;
		else
			notes = scan->notes() + "\n" + notes;

		scan->setNotes(notes);

		// Create control move to the optimal position.

		if (maxAxisValue.isValid()){
			moveAction_ = AMActionSupport::buildControlMoveAction(control_, double(maxAxisValue));

			connect( moveAction_, SIGNAL(cancelled()), this, SLOT(onMoveActionCancelled()) );
			connect( moveAction_, SIGNAL(failed()), this, SLOT(onMoveActionFailed()) );
			connect( moveAction, SIGNAL(succeeded()), this, SLOT(onMoveActionSucceeded()) );

			moveAction_->start();
		}
	}

	// Handle cleaning up after the scan action.

	onScanActionFinished();
}

void CLSJJSlitOptimizationAction::onScanActionFinished()
{
	if (scanAction_) {
		scanAction_->disconnect();
		scanAction_->deleteLater();
		scanAction_ = 0;
	}
}

void CLSJJSlitOptimizationAction::onMoveActionCancelled()
{
	setCancelled();
	onMoveActionFinished();
}

void CLSJJSlitOptimizationAction::onMoveActionFailed()
{
	setFailed();
	onMoveActionFinished();
}

void CLSJJSlitOptimizationAction::onMoveActionSucceeded()
{
	setSucceeded();
	onMoveActionFinished();
}

void CLSJJSlitOptimizationAction::onMoveActionFinished()
{
	if (moveAction_) {
		moveAction_->disconnect();
		moveAction_->deleteLater();
		moveAction_ = 0;
	}
}

AMNumber CLSJJSlitOptimizationAction::findOptimalPosition(AMDataSource *detectorSource) const
{
	AMNumber result = AMNumber(AMNumber::InvalidError);

	if (detectorSource) {

		// Find the maximum value achieved by the detector in the scan.

		AMnDIndex start = AMnDIndex(0);
		AMnDIndex end = AMnDIndex(detectorSource->size() - 1);

		QVector<double> data = QVector<double>(start.totalPointsTo(end));
		detectorSource->values(start, end, data.data());

		int maxIndex = AMUtility::flatIndexOfMaximum(data);

		// Identify corresponding control value.

		result = detectorSource->axisValue(0, maxIndex);
	}

	return maxAxisValue;
}

void CLSJJSlitOptimizationAction::startImplementation()
{
	// Initialize class variables.

	CLSJJSlitOptimizationActionInfo *info = qobject_cast<CLSJJSlitOptimizationActionInfo*>(info());

	AMControl *jjSlits = CLSBeamline::clsBeamline()->jjSlits();
	AMGenericStepScanConfiguration *configuration = info->configuration();
	AMControlInfo controlInfo = info->controlInfo();
	AMDetectorInfo detectorInfo = info->detectorInfo();

	if (configuration && !configuration->axisControlInfos().isEmpty() && detectorInfo != AMDetectorInfo() && configuration->detectorConfigurations().contains(detectorInfo) && jjSlits && jjSlits->hasChildControl(scanControl)) {
		control_ = CLSBeamline::clsBeamline()->exposedControlByInfo(configuration->axisControlInfos().at(0));

		scanAction_ = new AMScanAction(new AMScanActionInfo(configuration->createCopy()));

		connect( scanAction_, SIGNAL(started()), this, SLOT(onScanActionStarted()) );
		connect( scanAction_, SIGNAL(cancelled()), this, SLOT(onScanActionCancelled()) );
		connect( scanAction_, SIGNAL(failed()), this, SLOT(onScanActionFailed()) );
		connect( scanAction_, SIGNAL(succeeded()), this, SLOT(onScanActionSucceeded()) );

		scanAction_->start();

	} else {
		AMErrorMon::alert(this, CLSJJSLITOPTIMIZATIONACTION_)
	}
}

void CLSJJSlitOptimizationAction::pauseImplementation()
{

}

void CLSJJSlitOptimizationAction::resumeImplementation()
{

}

void CLSJJSlitOptimizationAction::skipImplementation(const QString &command)
{

}
