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


#include "CLSCAEN2527HVChannel.h"

#include "beamline/AMPVControl.h"

CLSCAEN2527HVChannel::CLSCAEN2527HVChannel(const QString &name, const QString &pvBaseName, AMHighVoltageChannel::highVoltageChannelPolarity polarity, QObject *parent) :
		AMHighVoltageChannel(name, parent)
{
	wasConnected_ = false;
	poweringDown_ = false;
	setNoPolarity(false);
	setPolarity(polarity);
	description_ = new AMProcessVariable(pvBaseName+":name", true, this);
	descriptionFbk_ = new AMProcessVariable(pvBaseName+":name:fbk", true, this);
	demand_ = new AMPVControl(name+"Demand", pvBaseName+":v0set:fbk", pvBaseName+":v0set", QString(), this, 5.0);
	voltage_ = new AMReadOnlyPVControl(name+"Voltage", pvBaseName+":vmon", this);
	toggle_ = new AMPVControl(name+"Toggle", pvBaseName+":pwonoff", pvBaseName+":pwonoff", QString(), this, 0.5);
	status_ = new AMReadOnlyPVControl(name+"Status", pvBaseName+":status", this);
	current_ = new AMReadOnlyPVControl(name+"Current", pvBaseName+":imon", this);

	connect(descriptionFbk_, SIGNAL(valueChanged(QString)), this, SLOT(onDescriptionFbkChanged(QString)));
	connect(demand_, SIGNAL(valueChanged(double)), this, SLOT(onDemandChanged(double)));
	connect(voltage_, SIGNAL(valueChanged(double)), this, SLOT(onVoltageChanged(double)));
	connect(toggle_, SIGNAL(valueChanged(double)), this, SLOT(onToggleChanged(double)));
	connect(status_, SIGNAL(valueChanged(double)), this, SLOT(onStatusChanged(double)));
	connect(current_, SIGNAL(valueChanged(double)), this, SLOT(onCurrentChanged(double)));

	connect(description_, SIGNAL(writeReadyChanged(bool)), this, SLOT(onChildConnected(bool)));
	connect(descriptionFbk_, SIGNAL(readReadyChanged(bool)), this, SLOT(onChildConnected(bool)));
	connect(demand_, SIGNAL(connected(bool)), this, SLOT(onChildConnected(bool)));
	connect(voltage_, SIGNAL(connected(bool)), this, SLOT(onChildConnected(bool)));
	connect(toggle_, SIGNAL(connected(bool)), this, SLOT(onChildConnected(bool)));
	connect(status_, SIGNAL(connected(bool)), this, SLOT(onChildConnected(bool)));
	connect(current_, SIGNAL(connected(bool)), this, SLOT(onChildConnected(bool)));
}

QString CLSCAEN2527HVChannel::description() const{
	if(isConnected())
		return descriptionFbk_->getString();
	else
		return "UNCONNECTED";
}

bool CLSCAEN2527HVChannel::isConnected() const{
	return description_->writeReady()
			&& descriptionFbk_->readReady()
			&& demand_->isConnected()
			&& voltage_->isConnected()
			&& toggle_->isConnected()
			&& status_->isConnected()
			&& current_->isConnected();
}

double CLSCAEN2527HVChannel::demand() const{
	if(isConnected())
		return demand_->value();
	else
		return -1;
}

double CLSCAEN2527HVChannel::voltage() const{
	if(isConnected())
		return voltage_->value();
	else
		return -1;
}

bool CLSCAEN2527HVChannel::isFullyPowered() const{
	return demand_->withinTolerance(voltage_->value());
}

QString CLSCAEN2527HVChannel::status() const{
	if(isConnected()){
		return status_->enumNameAt(status_->value());
	}
	else
		return "UNCONNECTED";
}

double CLSCAEN2527HVChannel::current() const{
	if(isConnected())
		return current_->value();
	else
		return -1;
}

AMProcessVariable* CLSCAEN2527HVChannel::descriptionControl(){
	return description_;
}

AMProcessVariable* CLSCAEN2527HVChannel::descriptionFbkControl(){
	return descriptionFbk_;
}

AMControl* CLSCAEN2527HVChannel::demandControl(){
	return demand_;
}

AMControl* CLSCAEN2527HVChannel::voltageControl(){
	return voltage_;
}

AMControl* CLSCAEN2527HVChannel::toggleControl(){
	return toggle_;
}

AMControl* CLSCAEN2527HVChannel::statusControl(){
	return status_;
}

AMControl* CLSCAEN2527HVChannel::currentControl(){
	return current_;
}

void CLSCAEN2527HVChannel::setDescription(const QString &description){
	if(isConnected())
		description_->setValue(description);
}

void CLSCAEN2527HVChannel::setDemand(double demand){
	if(isConnected())
		demand_->move(demand);
}

void CLSCAEN2527HVChannel::setPowerState(highVoltageChannelPowerState powerState){
	if(powerState == AMHighVoltageChannel::isPowerOn)
		toggle_->move(1);
	else if(powerState == AMHighVoltageChannel::isPowerOff)
		toggle_->move(0);
}

bool CLSCAEN2527HVChannel::setPolarity(highVoltageChannelPolarity polarity){
	Q_UNUSED(polarity);
	return false;
}

void CLSCAEN2527HVChannel::onDescriptionFbkChanged(const QString &description){
	emit descriptionChanged(description);
}

void CLSCAEN2527HVChannel::onChildConnected(bool connect){
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

void CLSCAEN2527HVChannel::onDemandChanged(double demand){
	emit demandChanged(demand);
}

void CLSCAEN2527HVChannel::onVoltageChanged(double voltage){
	emit voltageChanged(voltage);
	if(isOn() && demand_->withinTolerance(voltage))
		emit fullyPowered();
	if(isConnected() && poweringDown_ && ( voltage_->value() < 1.0 ) ){
		poweringDown_ = false;
		powerState_ = AMHighVoltageChannel::isPowerOff;
		emit powerStateChanged(powerState_);
	}
}

void CLSCAEN2527HVChannel::onToggleChanged(double toggle){
	Q_UNUSED(toggle)
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

void CLSCAEN2527HVChannel::onStatusChanged(double status){
	if(!isConnected())
		return;
	emit statusChanged(status_->enumNameAt(status));
}

void CLSCAEN2527HVChannel::onCurrentChanged(double current){
	emit currentChanged(current);
}
