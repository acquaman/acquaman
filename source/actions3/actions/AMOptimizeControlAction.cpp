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

	configuration_ = 0;
	control_ = 0;

	optimalPositionAB_ = 0;

	scanAction_ = 0;
	controlMoveAction_ = 0;

	// Current settings.

	setConfiguration(info->configuration());
}

AMOptimizeControlAction::AMOptimizeControlAction(const AMOptimizeControlAction &original) :
	AMAction3(original)
{

}

AMOptimizeControlAction::~AMOptimizeControlAction()
{

}

void AMOptimizeControlAction::setConfiguration(AMGenericStepScanConfiguration *newConfiguration)
{
	if (configuration_ != newConfiguration && canChangeConfiguration()) {

		if (configuration_) {
			disconnect( configuration_, 0, this, 0 );
		}

		configuration_ = newConfiguration;

		if (configuration_) {
			connect( configuration_, SIGNAL(axisControlInfosChanged()), this, SLOT(onConfigurationControlChanged()) );
		}

		onConfigurationControlChanged();
	}
}

void AMOptimizeControlAction::setControl(AMControl *newControl)
{
	if (control_ != newControl && canChangeControl()) {

		if (control_) {
			disconnect( control_, 0, this, 0 );
		}

		control_ = newControl;

		if (control_) {
			connect( control_, SIGNAL(moveFailed(int)), this, SLOT(onControlMoveFailed(int)) );
			connect( control_, SIGNAL(moveSucceeded()), this, SLOT(onControlMoveSucceeded()) );
		}
	}
}

void AMOptimizeControlAction::onConfigurationControlChanged()
{
	// Identify the configuration's new control, and update our control.

	if (configuration_) {

		QList<AMControlInfo> controlInfos = configuration_->axisControlInfos().toList();

		if (controlInfos.count() > 0) {
			AMControlInfo controlInfo = configuration_->axisControlInfos().at(0);
			AMControl *control = AMBeamline::bl()->exposedControlByInfo(controlInfo);

			setControl(control);
		}
	}
}

void AMOptimizeControlAction::onScanActionStarted()
{
	// Set up the mechanism for finding the optimal control value.

	AMScan *scan = scanAction_->controller()->scan();
	AMDataSource *optimizationSource = scan->dataSourceAt(0); // find a way for this to be not hardcoded.

	if (optimizeControlActionInfo()->option() == AMOptimizeControlActionInfo::Maximum)
		optimalPositionAB_ = new AM1DMaximumAB("Maximum");

	if (optimalPositionAB_) {
		optimalPositionAB_->setInputDataSources(QList<AMDataSource*>() << optimizationSource);

		scan->addAnalyzedDataSource(optimalPositionAB_, true, false);

		setStarted();

	} else {
		setFailed();
	}
}

void AMOptimizeControlAction::onScanActionCancelled()
{
	setCancelled();
}

void AMOptimizeControlAction::onScanActionFailed()
{
	setFailed();
}

void AMOptimizeControlAction::onScanActionSucceeded()
{
	// Once the scan is complete, we know the optimal position.

	double optimalPosition = double(optimalPositionAB_->axisValue(0, 0));
	qDebug() << "Optimal position =" << optimalPosition;

	// Create and execute control move action.

	controlMoveAction_ = AMActionSupport::buildControlMoveAction(control_, optimalPosition);

	connect( controlMoveAction_, SIGNAL(cancelled()), this, SLOT(onControlMoveActionCancelled()) );
	connect( controlMoveAction_, SIGNAL(failed()), this, SLOT(onControlMoveActionFailed()) );
	connect( controlMoveAction_, SIGNAL(succeeded()), this, SLOT(onControlMoveActionSucceeded()) );

	controlMoveAction_->start();
}

void AMOptimizeControlAction::onControlMoveActionCancelled()
{
	setCancelled();
}

void AMOptimizeControlAction::onControlMoveActionFailed()
{
	setFailed();
}

void AMOptimizeControlAction::onControlMoveActionSucceeded()
{
	setSucceeded();
}

bool AMOptimizeControlAction::canChangeConfiguration() const
{
	bool result = false;

	if (state() == Constructed)
		result = true;

	return result;
}

bool AMOptimizeControlAction::canChangeControl() const
{
	bool result = false;

	if (state() == Constructed)
		result = true;

	return result;
}

void AMOptimizeControlAction::startImplementation()
{
	if (configuration_ && control_) {

		// Create and execute scan action.

		scanAction_ = new AMScanAction(new AMScanActionInfo(configuration_));

		connect( scanAction_, SIGNAL(started()), this, SLOT(onScanActionStarted()) );
		connect( scanAction_, SIGNAL(cancelled()), this, SLOT(onScanActionCancelled()) );
		connect( scanAction_, SIGNAL(failed()), this, SLOT(onScanActionFailed()) );

		scanAction_->start();

	} else {
		setFailed();
	}
}

void AMOptimizeControlAction::pauseImplementation()
{
	if (scanAction_ && !scanAction_->inFinalState())
		scanAction_->pause();

	if (controlMoveAction_ && !controlMoveAction_->inFinalState())
		controlMoveAction_->pause();
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
	if (scanAction_)
		scanAction_->cancel();

	if (controlMoveAction_ && !controlMoveAction_->inFinalState())
		controlMoveAction_->cancel();
}

void AMOptimizeControlAction::skipImplementation(const QString &command)
{
	if (scanAction_ && !scanAction_->inFinalState())
		scanAction_->skip(command);

	if (controlMoveAction_ && !controlMoveAction_->inFinalState())
		controlMoveAction_->skip(command);
}
