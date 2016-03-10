#include "CLSSIS3820ScalerDarkCurrentMeasurementAction.h"
#include "beamline/CLS/CLSBeamline.h"
#include "util/AMErrorMonitor.h"

CLSSIS3820ScalerDarkCurrentMeasurementAction::CLSSIS3820ScalerDarkCurrentMeasurementAction(CLSSIS3820ScalerDarkCurrentMeasurementActionInfo *info, QObject *parent) :
	AMAction3(info, parent)
{
	oldTime_ = 0;

	startedMapper_ = new QSignalMapper(this);
	connect( startedMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onStarted()) );

	failedMapper_ = new QSignalMapper(this);
	connect( failedMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onFailed(QObject*)) );

	succeededMapper_ = new QSignalMapper(this);
	connect( succeededMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onSucceeded(QObject*)) );
}

CLSSIS3820ScalerDarkCurrentMeasurementAction::CLSSIS3820ScalerDarkCurrentMeasurementAction(const CLSSIS3820ScalerDarkCurrentMeasurementAction &other) :
	AMAction3(other)
{
	oldTime_ = 0;

	startedMapper_ = new QSignalMapper(this);
	connect( startedMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onStarted()) );

	failedMapper_ = new QSignalMapper(this);
	connect( failedMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onFailed(QObject*)) );

	succeededMapper_ = new QSignalMapper(this);
	connect( succeededMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onSucceeded(QObject*)) );
}

CLSSIS3820ScalerDarkCurrentMeasurementAction::~CLSSIS3820ScalerDarkCurrentMeasurementAction()
{

}

void CLSSIS3820ScalerDarkCurrentMeasurementAction::onStarted()
{
	// Set the action as started.

	setStarted();
}

void CLSSIS3820ScalerDarkCurrentMeasurementAction::onFailed(QObject *action)
{
	// Cleanup after the measurement action and restore pre-measurement settings.

	onMeasurementFinished(action);

	// Notify each detector that the dark current measurement failed.

	CLSSIS3820Scaler *scaler = CLSBeamline::clsBeamline()->scaler();

	if (scaler && scaler->isConnected()) {

		for (int i = 0; i < scaler->channels().count(); i++) {
			CLSSIS3820ScalerChannel *channel = scaler->channelAt(i);

			if (channel && channel->isEnabled() && channel->detector() && channel->detector()->canDoDarkCurrentCorrection()) {
				channel->detector()->setDarkCurrentValue(0);
				channel->detector()->setDarkCurrentValidState(false);
			}
		}
	}

	// Issue error.

	QString message = "Failed to complete scaler dark current measurement.";
	AMErrorMon::alert(this, CLSSIS3820SCALERDARKCURRENTMEASUREMENTACTION_ACTION_FAILED, message);
	setFailed(message);
}

void CLSSIS3820ScalerDarkCurrentMeasurementAction::onSucceeded(QObject *action)
{
	// Cleanup after the measurement action and restore pre-measurement settings.

	onMeasurementFinished(action);

	// Set the action as succeeded.

	setSucceeded();
}

void CLSSIS3820ScalerDarkCurrentMeasurementAction::onMeasurementFinished(QObject *action)
{
	// Disconnect from and delete the measurement action.

	actionCleanup(action);

	// Restore pre-measurement settings.

	measurementCleanup();
}

void CLSSIS3820ScalerDarkCurrentMeasurementAction::measurementInitialization()
{
	CLSSIS3820Scaler *scaler = CLSBeamline::clsBeamline()->scaler();

	if (scaler)
		oldTime_ = scaler->dwellTime();
}

void CLSSIS3820ScalerDarkCurrentMeasurementAction::measurementCleanup()
{
	CLSSIS3820Scaler *scaler = CLSBeamline::clsBeamline()->scaler();

	if (scaler)
		scaler->setDwellTime(oldTime_);
}

void CLSSIS3820ScalerDarkCurrentMeasurementAction::actionCleanup(QObject *action)
{
	if (action) {
		startedMapper_->removeMappings(action);
		failedMapper_->removeMappings(action);
		succeededMapper_->removeMappings(action);

		action->disconnect();

		action->deleteLater();
	}
}

bool CLSSIS3820ScalerDarkCurrentMeasurementAction::validDwellTime(double time) const
{
	return ( time > 0 );
}

void CLSSIS3820ScalerDarkCurrentMeasurementAction::startImplementation()
{
	// Must have a valid, connected scaler.

	CLSSIS3820Scaler *scaler = CLSBeamline::clsBeamline()->scaler();

	if (! (scaler && scaler->isConnected()) ) {
		QString message = QString("There was an error measuring scaler dark current. The scaler is invalid or not connected.");
		AMErrorMon::alert(this, CLSSIS3820SCALERDARKCURRENTMEASUREMENTACTION_INVALID_SCALER, message);
		setFailed(message);
	}

	// Must have a valid dwell time.

	double secondsDwell = scalerDarkCurrentMeasurementActionInfo()->dwellTime();

	if (!validDwellTime(secondsDwell)) {
		QString message = QString("There was an error measuring scaler dark current. The dwell time provided (%1 s) is invalid.").arg(secondsDwell);
		AMErrorMon::alert(this, CLSSIS3820SCALERDARKCURRENTMEASUREMENTACTION_INVALID_DWELL_TIME, message);
		setFailed(message);
	}

	// Update pre-measurement settings, to be restored once measurement is complete.

	measurementInitialization();

	// Create measurement action.

	AMAction3 *measurementAction = createMeasurementAction(secondsDwell);

	// Make connections and start action.

	if (measurementAction) {

		startedMapper_->setMapping(measurementAction, measurementAction);
		failedMapper_->setMapping(measurementAction, measurementAction);
		succeededMapper_->setMapping(measurementAction, measurementAction);

		connect( measurementAction, SIGNAL(started()), startedMapper_, SLOT(map()) );
		connect( measurementAction, SIGNAL(failed()), failedMapper_, SLOT(map()) );
		connect( measurementAction, SIGNAL(succeeded()), succeededMapper_, SLOT(map()) );

		measurementAction->start();

	} else {

		QString message = QString("There was an error measuring scaler dark current. An invalid measurement action was generated.");
		AMErrorMon::alert(this, CLSSIS3820SCALERDARKCURRENTMEASUREMENTACTION_INVALID_ACTION, message);
		setFailed(message);
	}
}

AMAction3* CLSSIS3820ScalerDarkCurrentMeasurementAction::createMeasurementAction(double secondsDwell)
{
	AMAction3 *result = 0;

	CLSSIS3820Scaler *scaler = CLSBeamline::clsBeamline()->scaler();

	if (scaler) {
		AMListAction3 *measurementAction = new AMListAction3(new AMListActionInfo3("Taking dark current measurement.", "Taking dark current measurement."), AMListAction3::Sequential);

		measurementAction->addSubAction(scaler->createDwellTimeAction3(secondsDwell));
		measurementAction->addSubAction(scaler->createTriggerAction(AMDetectorDefinitions::SingleRead));

		AMListAction3 *notifyChannelDetectors = new AMListAction3(new AMListActionInfo3("Set last measurement as dark current measurement", "Set last measurement as dark current measurement"));

		for (int i = 0; i < scaler->channels().count(); i++) {
			CLSSIS3820ScalerChannel *channel = scaler->channelAt(i);

			if (channel && channel->isEnabled() && channel->detector() && channel->detector()->canDoDarkCurrentCorrection()) {
				notifyChannelDetectors->addSubAction(channel->detector()->createSetLastMeasurementAsDarkCurrentAction());
			}
		}

		measurementAction->addSubAction(notifyChannelDetectors);

		result = measurementAction;
	}

	return result;
}
