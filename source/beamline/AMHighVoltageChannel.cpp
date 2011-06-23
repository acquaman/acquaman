#include "AMHighVoltageChannel.h"

#include <QDebug>

AMHighVoltageChannel::AMHighVoltageChannel(const QString &name, QObject *parent) :
		QObject(parent)
{
	name_ = name;
	powerState_ = AMHighVoltageChannel::isPowerInvalid;
	polarity_ = AMHighVoltageChannel::none;
	noCurrent_ = true;
	noPolarity_ = true;
	connect(this, SIGNAL(powerStateChanged(AMHighVoltageChannel::highVoltageChannelPowerState)), this, SLOT(onPowerStateChanged(AMHighVoltageChannel::highVoltageChannelPowerState)));
}

QString AMHighVoltageChannel::name() const{
	return name_;
}

bool AMHighVoltageChannel::isConnected() const{
	return false;
}

AMHighVoltageChannel::highVoltageChannelPowerState AMHighVoltageChannel::powerState() const{
	return powerState_;
}

bool AMHighVoltageChannel::isOn() const{
	return powerState() == AMHighVoltageChannel::isPowerOn;
}

bool AMHighVoltageChannel::isOff() const{
	return powerState() == AMHighVoltageChannel::isPowerOff;
}

bool AMHighVoltageChannel::hasCurrent() const{
	return !noCurrent_;
}

AMHighVoltageChannel::highVoltageChannelPolarity AMHighVoltageChannel::polarity() const{
	return polarity_;
}

bool AMHighVoltageChannel::hasPolarity() const{
	return !noPolarity_;
}

void AMHighVoltageChannel::setPowerState(highVoltageChannelPowerState powerState){
	powerState_ = powerState;
}

void AMHighVoltageChannel::setOn(bool on){
	if(on)
		setPowerState(AMHighVoltageChannel::isPowerOn);
	else
		setPowerState(AMHighVoltageChannel::isPowerOff);
}

void AMHighVoltageChannel::setOff(){
	setOn(false);
}

bool AMHighVoltageChannel::setPolarity(highVoltageChannelPolarity polarity){
	polarity_ = polarity;
}

void AMHighVoltageChannel::setNoCurrent(bool noCurrent){
	noCurrent_ = noCurrent;
}

void AMHighVoltageChannel::setNoPolarity(bool noPolarity){
	noPolarity_ = noPolarity;
}

void AMHighVoltageChannel::onPowerStateChanged(highVoltageChannelPowerState powerState){
	if(powerState == AMHighVoltageChannel::isPowerOn)
		emit turnedOn();
	else if(powerState == AMHighVoltageChannel::isPowerOff){
		emit turnedOff();
	}
}
