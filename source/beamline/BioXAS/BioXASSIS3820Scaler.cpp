#include "BioXASSIS3820Scaler.h"
#include "beamline/AMPVControl.h"

#include "beamline/AMDetectorTriggerSource.h"

BioXASSIS3820Scaler::BioXASSIS3820Scaler(const QString &baseName, QObject *parent) :
	CLSSIS3820Scaler(baseName, parent)
{
	inputsMode_ = new AMSinglePVControl("InputMode", baseName+":inputMode", this);
	triggerSourceMode_ = new AMSinglePVControl("TriggerSource", baseName+":triggerSource", this);
	clockSourceMode_ = new AMSinglePVControl("ClockSource", baseName+":source", this);

	isArming_ = false;
	isTriggered_ = false;
}

BioXASSIS3820Scaler::~BioXASSIS3820Scaler()
{

}

bool BioXASSIS3820Scaler::isConnected() const
{
	bool connected = (
				CLSSIS3820Scaler::isConnected() &&
				inputsMode_ && inputsMode_->isConnected() &&
				triggerSourceMode_ && triggerSourceMode_->isConnected() &&
				clockSourceMode_ && clockSourceMode_->isConnected()
				);

	return connected;
}

void BioXASSIS3820Scaler::arm()
{
	if(isArming_)
		return;

	isArming_ = true;
	disconnect(startToggle_, SIGNAL(valueChanged(double)), this, SLOT(onScanningToggleChanged()));
	connect(startToggle_, SIGNAL(valueChanged(double)), this, SLOT(onStartToggleArmed()));

	startToggle_->move(1);
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
	if(!isArming_)
		return;

	if(startToggle_->withinTolerance(1.0)){
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
}

void BioXASSIS3820Scaler::onScanningToggleChanged()
{
	if(!isConnected())
		return;

	if(isTriggered_ && startToggle_->withinTolerance(0.0)){
		isTriggered_ = false;
		triggerSource_->setSucceeded();
	}

	emit scanningChanged(startToggle_->withinTolerance(1));
}
