#include "CLSSIS3820ScalerTriggerAction.h"
#include "beamline/AMDetectorTriggerSource.h"
#include "beamline/CLS/CLSBeamline.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "util/AMErrorMonitor.h"

CLSSIS3820ScalerTriggerAction::CLSSIS3820ScalerTriggerAction(CLSSIS3820ScalerTriggerActionInfo *info, QObject *parent) :
	AMAction3(info, parent)
{

}

CLSSIS3820ScalerTriggerAction::CLSSIS3820ScalerTriggerAction(const CLSSIS3820ScalerTriggerAction &original) :
	AMAction3(original)
{

}

CLSSIS3820ScalerTriggerAction::~CLSSIS3820ScalerTriggerAction()
{

}

void CLSSIS3820ScalerTriggerAction::onStarted()
{
	// Set the action as started.

	setStarted();
}

void CLSSIS3820ScalerTriggerAction::onFailed()
{
	onFinished();

	// Create failure message and set action as failed.

	QString message = QString("There was an error triggering the scaler.");
	AMErrorMon::alert(this, CLSSIS3820SCALERTRIGGERACTION_TRIGGER_FAILED, message);
	setFailed(message);
}

void CLSSIS3820ScalerTriggerAction::onSucceeded()
{
	// Set the action as succeeded.

	onFinished();
	setSucceeded();
}

void CLSSIS3820ScalerTriggerAction::onFinished()
{
	triggerCleanup();
}

bool CLSSIS3820ScalerTriggerAction::supportedReadMode(int mode) const
{
	bool result = false;

	switch (mode) {
	case AMDetectorDefinitions::SingleRead:
		result = true;
		break;
	case AMDetectorDefinitions::ContinuousRead:
		result = true;
		break;
	default:
		break;
	}

	return result;
}

void CLSSIS3820ScalerTriggerAction::startImplementation()
{
	// Must have a valid, connected scaler.

	CLSSIS3820Scaler *scaler = CLSBeamline::clsBeamline()->scaler();

	if ( !(scaler && scaler->isConnected()) ) {
		QString message = QString("There was an error triggering the scaler. The scaler appears to be invalid or not connected.");
		AMErrorMon::alert(this, CLSSIS3820SCALERTRIGGERACTION_INVALID_SCALER, message);
		setFailed(message);
		return;
	}

	// Must have a valid scaler trigger source.

	AMDetectorTriggerSource *triggerSource = scaler->triggerSource();

	if (!triggerSource) {
		QString message = QString("There was an error triggering the scaler. The scaler doesn't appear to have a valid software trigger source.");
		AMErrorMon::alert(this, CLSSIS3820SCALERTRIGGERACTION_INVALID_TRIGGER_SOURCE, message);
		setFailed(message);
		return;
	}

	// Must have a supported read mode.

	int infoMode = scalerTriggerInfo()->readMode();

	if (!supportedReadMode(infoMode)) {
		QString message = QString("There was an error triggering the scaler. The acquisition read mode provided (%1) is either invalid or not supported.").arg(infoMode);
		AMErrorMon::alert(this, CLSSIS3820SCALERTRIGGERACTION_INVALID_READ_MODE, message);
		setFailed(message);
		return;
	}

	// Initialize.

	triggerInitialization();

	// Make connections.

	connect( triggerSource, SIGNAL(triggered(AMDetectorDefinitions::ReadMode)), this, SLOT(onStarted()) );
	connect( triggerSource, SIGNAL(failed()), this, SLOT(onFailed()) );
	connect( triggerSource, SIGNAL(succeeded()), this, SLOT(onSucceeded()) );

	// Start action.

	triggerSource->trigger(AMDetectorDefinitions::ReadMode(infoMode));
}
