#include "BioXASSIS3820Scaler.h"
#include "beamline/AMPVControl.h"
#include "actions3/AMListAction3.h"
#include "actions3/actions/AMWaitAction.h"
#include "actions3/AMActionSupport.h"
#include "beamline/AMDetectorTriggerSource.h"

BioXASSIS3820Scaler::BioXASSIS3820Scaler(const QString &baseName, BioXASZebraSoftInputControl *softInput, QObject *parent) :
	CLSSIS3820Scaler(baseName, parent)
{
	inputsMode_ = new AMSinglePVControl("InputMode", baseName+":inputMode", this);
	triggerSourceMode_ = new AMSinglePVControl("TriggerSource", baseName+":triggerSource", this);
	clockSourceMode_ = new AMSinglePVControl("ClockSource", baseName+":source", this);
	softInput_ = softInput;

	isArming_ = false;
	isTriggered_ = false;
}

BioXASSIS3820Scaler::~BioXASSIS3820Scaler()
{

}

bool BioXASSIS3820Scaler::isConnected() const
{
	bool connected = (CLSSIS3820Scaler::isConnected() &&
			  inputsMode_ && inputsMode_->isConnected() &&
			  triggerSourceMode_ && triggerSourceMode_->isConnected() &&
			  clockSourceMode_ && clockSourceMode_->isConnected() &&
			  softInput_ && softInput_->isConnected());

	return connected;
}

void BioXASSIS3820Scaler::arm()
{
	if(!isArming_){

		isArming_ = true;
		disconnect(startToggle_, SIGNAL(valueChanged(double)), this, SLOT(onScanningToggleChanged()));
		connect(startToggle_, SIGNAL(valueChanged(double)), this, SLOT(onStartToggleArmed()));

		startToggle_->move(1);
	}
}

void BioXASSIS3820Scaler::setTriggerSource(AMDetectorTriggerSource *triggerSource)
{
	if(triggerSource_)
		disconnect(triggerSource_, SIGNAL(triggered(AMDetectorDefinitions::ReadMode)), this, SLOT(onTriggerSourceTriggered(AMDetectorDefinitions::ReadMode)));

	triggerSource_ = 0;

	if(triggerSource){
		triggerSource_ = triggerSource;
		connect(triggerSource_, SIGNAL(triggered(AMDetectorDefinitions::ReadMode)), this, SLOT(onTriggerSourceTriggered(AMDetectorDefinitions::ReadMode)));
	}
}

void BioXASSIS3820Scaler::onStartToggleArmed()
{
	if(isArming_ && startToggle_->withinTolerance(1.0)){

		isArming_ = false;
		disconnect(startToggle_, SIGNAL(valueChanged(double)), this, SLOT(onStartToggleArmed()));
		connect(startToggle_, SIGNAL(valueChanged(double)), this, SLOT(onScanningToggleChanged()));

		emit armed();
	}
}

void BioXASSIS3820Scaler::onTriggerSourceTriggered(AMDetectorDefinitions::ReadMode readMode)
{
	Q_UNUSED(readMode)
	isTriggered_ = true;

	for(int x = 0, size = scalerChannels_.count(); x < size; x++){
		if(scalerChannels_.at(x)->isEnabled()){

			waitingChannels_.append(x);
			triggerChannelMapper_->setMapping(scalerChannels_.at(x), x);
		}
	}

	connect(triggerChannelMapper_, SIGNAL(mapped(int)), this, SLOT(onChannelReadingChanged(int)));
	connect(startToggle_, SIGNAL(valueChanged(double)), this, SLOT(triggerAcquisitionFinished()));
}

void BioXASSIS3820Scaler::onChannelReadingChanged(int channelIndex)
{
	if(waitingChannels_.contains(channelIndex)){

		waitingChannels_.removeAll(channelIndex);
		triggerChannelMapper_->removeMappings(channelAt(channelIndex));
	}

	triggerAcquisitionFinished();
}

void BioXASSIS3820Scaler::triggerAcquisitionFinished()
{
	if(isTriggered_ && waitingChannels_.count() == 0 && !isScanning()){

		isTriggered_ = false;
		disconnect(triggerChannelMapper_, SIGNAL(mapped(int)), this, SLOT(onChannelReadingChanged(int)));
		disconnect(startToggle_, SIGNAL(valueChanged(double)), this, SLOT(triggerAcquisitionFinished()));

		AMZebraDetectorTriggerSource *trigger = qobject_cast<AMZebraDetectorTriggerSource *>(triggerSource_);

		if (trigger)
			trigger->setSucceeded(this);
	}
}

AMAction3* BioXASSIS3820Scaler::createMoveToSingleShotAction()
{
	AMListAction3 *toSingleShot = new AMListAction3(new AMListActionInfo3("Moving BioXAS scaler to 'Single shot' mode.", "Moving BioXAS scaler to 'Single shot' mode."), AMListAction3::Sequential);
	toSingleShot->addSubAction(CLSSIS3820Scaler::createMoveToSingleShotAction());
	toSingleShot->addSubAction(new AMWaitAction(new AMWaitActionInfo(1.0)));

	return toSingleShot;
}

AMAction3* BioXASSIS3820Scaler::createMoveToContinuousAction()
{
	AMListAction3 *toContinuous = new AMListAction3(new AMListActionInfo3("Moving BioXAS scaler to 'Continuous' mode.", "Moving BioXAS scaler to 'Continuous' mode."), AMListAction3::Sequential);
	toContinuous->addSubAction(CLSSIS3820Scaler::createMoveToContinuousAction());
	toContinuous->addSubAction(new AMWaitAction(new AMWaitActionInfo(1.0)));
	toContinuous->addSubAction(AMActionSupport::buildControlMoveAction(softInput_, 1.0));

	return toContinuous;
}
