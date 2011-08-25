/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "CLSPGT8000HVChannel.h"

#include "beamline/AMControl.h"

CLSPGT8000HVChannel::CLSPGT8000HVChannel(const QString &name, const QString &pvBaseName, QObject *parent) :
		AMHighVoltageChannel(name, parent)
{
	wasConnected_ = false;
	poweringDown_ = false;
	setNoPolarity(false);
	setPolarity(AMHighVoltageChannel::negative);
	setNoCurrent(true);
	description_ = "";
	demand_ = new AMPVControl(name+"Demand", pvBaseName+":Bias:Volt:fbk", pvBaseName+":Bias:Volt", QString(), this, 1.0);
	voltage_ = new AMReadOnlyPVControl(name+"Voltage", pvBaseName+":Bias:VoltActual:fbk", this);
	toggle_ = new AMPVControl(name+"Toggle", pvBaseName+":Power:fbk", pvBaseName+":Power", QString(), this, 0.5);

	connect(demand_, SIGNAL(valueChanged(double)), this, SLOT(onDemandChanged(double)));
	connect(voltage_, SIGNAL(valueChanged(double)), this, SLOT(onVoltageChanged(double)));
	connect(toggle_, SIGNAL(valueChanged(double)), this, SLOT(onToggleChanged(double)));

	connect(demand_, SIGNAL(connected(bool)), this, SLOT(onChildConnected(bool)));
	connect(voltage_, SIGNAL(connected(bool)), this, SLOT(onChildConnected(bool)));
	connect(toggle_, SIGNAL(connected(bool)), this, SLOT(onChildConnected(bool)));
}

QString CLSPGT8000HVChannel::description() const{
	return description_;
}

bool CLSPGT8000HVChannel::isConnected() const{
	return demand_->isConnected()
			&& voltage_->isConnected()
			&& toggle_->isConnected();
}

double CLSPGT8000HVChannel::demand() const{
	if(isConnected())
		return demand_->value();
	else
		return -1;
}

double CLSPGT8000HVChannel::voltage() const{
	if(isConnected())
		return voltage_->value();
	else
		return -1;
}

QString CLSPGT8000HVChannel::status() const{
	if(isConnected())
		return toggle_->enumNameAt(toggle_->value());
	else
		return "UNCONNECTED";
}

double CLSPGT8000HVChannel::current() const{
	return -1;
}

void CLSPGT8000HVChannel::setDescription(const QString &description){
	description_ = description;
}

void CLSPGT8000HVChannel::setDemand(double demand){
	if(isConnected())
		demand_->move(demand);
}

void CLSPGT8000HVChannel::setPowerState(highVoltageChannelPowerState powerState){
	if(powerState == AMHighVoltageChannel::isPowerOn)
		toggle_->move(1);
	else if(powerState == AMHighVoltageChannel::isPowerOff)
		toggle_->move(0);
}

void CLSPGT8000HVChannel::onChildConnected(bool connect){
	Q_UNUSED(connect);
	if(isConnected() && !wasConnected_){
		wasConnected_ = true;
		onToggleChanged(toggle_->value());
		emit connected(true);
	}
	else if(wasConnected_){
		wasConnected_ = false;
		emit connected(false);
	}
}

void CLSPGT8000HVChannel::onDemandChanged(double demand){
	emit demandChanged(demand);
}

void CLSPGT8000HVChannel::onVoltageChanged(double voltage){
	emit voltageChanged(voltage);
	if(isOn() && demand_->withinTolerance(voltage))
		emit fullyPowered();
	if(isConnected() && poweringDown_ && (voltage_->value()) < 1){
		poweringDown_ = false;
		powerState_ = AMHighVoltageChannel::isPowerOff;
		emit powerStateChanged(powerState_);
	}
}

void CLSPGT8000HVChannel::onToggleChanged(double toggle){
	if(!isConnected())
		return;
	if(toggle_->withinTolerance(1)){
		powerState_ = AMHighVoltageChannel::isPowerOn;
		emit powerStateChanged(powerState_);
	}
	else if(toggle_->withinTolerance(0)){
		poweringDown_ = true;
		onVoltageChanged(voltage_->value());
	}
}
