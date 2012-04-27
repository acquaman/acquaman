/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


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
	if(hasPolarity()){
		polarity_ = polarity;
		return true;
	}
	return false;
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
