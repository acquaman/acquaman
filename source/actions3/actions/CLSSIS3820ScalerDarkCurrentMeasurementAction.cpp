#include "CLSSIS3820ScalerDarkCurrentMeasurementAction.h"
#include "actions3/actions/AMWaitAction.h"
#include "dataman/info/AMDetectorInfo.h"
#include "beamline/CLS/CLSBeamline.h"
#include "util/AMErrorMonitor.h"

CLSSIS3820ScalerDarkCurrentMeasurementAction::CLSSIS3820ScalerDarkCurrentMeasurementAction(CLSSIS3820ScalerDarkCurrentMeasurementActionInfo *info, QObject *parent) :
    AMListAction3(info, AMListAction3::Sequential, parent)
{
	CLSSIS3820Scaler *scaler = CLSBeamline::clsBeamline()->scaler();
	double secondsDwell = scalerDarkCurrentMeasurementActionInfo()->dwellTime();

	connect( this, SIGNAL(failed()), this, SLOT(onActionFailed()) );

	if (scaler && scaler->isConnected() && secondsDwell > 0) {

		// pre-measurement settings.
		double oldDwell = scaler->dwellTime();

		// first turn off beam.
//		addSubAction(AMBeamline::bl()->createTurnOffBeamActions());

		// set the scaler's dwell time to new time.
		addSubAction(scaler->createDwellTimeAction3(secondsDwell));

		// initiate a scaler measurement and wait until it is complete.
//		addSubAction(scaler->createStartAction3(true));
//		addSubAction(scaler->createWaitForDwellFinishedAction(secondsDwell + 5.0));
		addSubAction(scaler->createTriggerAction(AMDetectorDefinitions::SingleRead));

		// notify attached and able scaler channel detectors that the latest measurement was a dark current measurement.
		AMListAction3 *notifyChannelDetectors = new AMListAction3(new AMListActionInfo3("Set last measurement as dark current measurement", "Set last measurement as dark current measurement"));

		for (int i = 0; i < scaler->channels().count(); i++) {
			CLSSIS3820ScalerChannel *channel = scaler->channelAt(i);

			if (channel && channel->isEnabled() && channel->detector() && channel->detector()->canDoDarkCurrentCorrection()) {
				notifyChannelDetectors->addSubAction(channel->detector()->createSetLastMeasurementAsDarkCurrentAction());
			}
		}

		addSubAction(notifyChannelDetectors);

		// reset settings to pre-measurement conditions.
		addSubAction(scaler->createDwellTimeAction3(oldDwell));

	} else {
		AMErrorMon::alert(this, CLSSIS3820SCALERDARKCURRENTMEASUREMENTACTION_SCALER_NOT_VALID, "Failed to complete dark current measurement--scaler not valid.");
		setFailed();
	}

}

CLSSIS3820ScalerDarkCurrentMeasurementAction::CLSSIS3820ScalerDarkCurrentMeasurementAction(const CLSSIS3820ScalerDarkCurrentMeasurementAction &other) :
	AMListAction3(other)
{

}

CLSSIS3820ScalerDarkCurrentMeasurementAction::~CLSSIS3820ScalerDarkCurrentMeasurementAction()
{

}

void CLSSIS3820ScalerDarkCurrentMeasurementAction::onActionFailed()
{
	AMErrorMon::alert(this, CLSSIS3820SCALERDARKCURRENTMEASUREMENTACTION_ACTION_FAILED, "Failed to complete dark current measurement.");

	CLSSIS3820Scaler *scaler = CLSBeamline::clsBeamline()->scaler();

	if (scaler && scaler->isConnected()) {

		// Notify each able channel detector that the dark current measurement has failed.

		for (int i = 0; i < scaler->channels().count(); i++) {
			CLSSIS3820ScalerChannel *channel = scaler->channelAt(i);

			if (channel && channel->isEnabled() && channel->detector() && channel->detector()->canDoDarkCurrentCorrection()) {
				channel->detector()->setDarkCurrentValue(0);
				channel->detector()->setDarkCurrentValidState(false);
			}
		}
	}
}
