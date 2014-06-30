/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#include "CLSSynchronizedDwellTimeConfiguration.h"

#include <QStringBuilder>

 CLSSynchronizedDwellTimeConfiguration::~CLSSynchronizedDwellTimeConfiguration(){}
CLSSynchronizedDwellTimeConfiguration::CLSSynchronizedDwellTimeConfiguration(const QString &name, int number, QObject *parent)
	: QObject(parent)
{
	connected_ = false;

	name_ = new AMProcessVariable(name % ":device" % QChar(65+number), true, this);
	dwellTimePV_ = new AMProcessVariable(name % ":set" % QChar(65+number) % ".OUT", true, this);
	scale_ = new AMProcessVariable(name % ":set" % QChar(65+number) % ".INPA", true, this);
	offset_ = new AMProcessVariable(name % ":set" % QChar(65+number) % ".INPB", true, this);
	units_ = new AMProcessVariable(name % ":set" % QChar(65+number) % ".EGU", true, this);
	modePV_ = new AMProcessVariable(name % ":mode" % QChar(65+number) % ".OUT", true, this);
	singleShot_ = new AMProcessVariable(name % ":mode" % QChar(65+number) % ".INPA", true, this);
	continuous_ = new AMProcessVariable(name % ":mode" % QChar(65+number) % ".INPB", true, this);
	triggerPV_ = new AMProcessVariable(name % ":start" % QChar(65+number) % ".LNK2", true, this);
	trigger_ = new AMProcessVariable(name % ":invert" % QChar(65+number), true, this);
	preTrigger_ = new AMProcessVariable(name % ":preTrig" % QChar(65+number), true, this);
	dwellHold_ = new AMProcessVariable(name % ":hold" % QChar(65+number), true, this);
	statusPV_ = new AMProcessVariable(name % ":stat" % QChar(65+number) % ":clc.INPA", true, this);
	waitFor_ = new AMProcessVariable(name % ":busy" % QChar(65+number) % ":type", true, this);
	delay_ = new AMProcessVariable(name % ":busy" % QChar(65+number) % ":timer.DLY2", true, this);
	waitPV_ = new AMProcessVariable(name % ":busyVal" % QChar(65+number) % ".INPA", true, this);
	waitValue_ = new AMProcessVariable(name % ":busyVal" % QChar(65+number) % ".INPB", true, this);

	connect(name_, SIGNAL(connected()), this, SLOT(onConnectedChanged()));
	connect(dwellTimePV_, SIGNAL(connected()), this, SLOT(onConnectedChanged()));
	connect(scale_, SIGNAL(connected()), this, SLOT(onConnectedChanged()));
	connect(offset_, SIGNAL(connected()), this, SLOT(onConnectedChanged()));
	connect(units_, SIGNAL(connected()), this, SLOT(onConnectedChanged()));
	connect(modePV_, SIGNAL(connected()), this, SLOT(onConnectedChanged()));
	connect(singleShot_, SIGNAL(connected()), this, SLOT(onConnectedChanged()));
	connect(continuous_, SIGNAL(connected()), this, SLOT(onConnectedChanged()));
	connect(triggerPV_, SIGNAL(connected()), this, SLOT(onConnectedChanged()));
	connect(trigger_, SIGNAL(connected()), this, SLOT(onConnectedChanged()));
	connect(preTrigger_, SIGNAL(connected()), this, SLOT(onConnectedChanged()));
	connect(dwellHold_, SIGNAL(connected()), this, SLOT(onConnectedChanged()));
	connect(statusPV_, SIGNAL(connected()), this, SLOT(onConnectedChanged()));
	connect(waitFor_, SIGNAL(connected()), this, SLOT(onConnectedChanged()));
	connect(delay_, SIGNAL(connected()), this, SLOT(onConnectedChanged()));
	connect(waitPV_, SIGNAL(connected()), this, SLOT(onConnectedChanged()));
	connect(waitValue_, SIGNAL(connected()), this, SLOT(onConnectedChanged()));

	connect(name_, SIGNAL(valueChanged()), this, SIGNAL(nameChanged()));
	connect(dwellTimePV_, SIGNAL(valueChanged()), this, SIGNAL(dwellTimePVChanged()));
	connect(scale_, SIGNAL(valueChanged()), this, SIGNAL(scaleChanged()));
	connect(offset_, SIGNAL(valueChanged()), this, SIGNAL(offsetChanged()));
	connect(units_, SIGNAL(valueChanged()), this, SIGNAL(unitsChanged()));
	connect(modePV_, SIGNAL(valueChanged()), this, SIGNAL(modePVChanged()));
	connect(singleShot_, SIGNAL(valueChanged()), this, SIGNAL(singleShotChanged()));
	connect(continuous_, SIGNAL(valueChanged()), this, SIGNAL(continuousChanged()));
	connect(triggerPV_, SIGNAL(valueChanged()), this, SIGNAL(triggerPVChanged()));
	connect(trigger_, SIGNAL(valueChanged()), this, SIGNAL(triggerChanged()));
	connect(preTrigger_, SIGNAL(valueChanged()), this, SIGNAL(preTriggerChanged()));
	connect(dwellHold_, SIGNAL(valueChanged()), this, SIGNAL(dwellHoldChanged()));
	connect(statusPV_, SIGNAL(valueChanged()), this, SIGNAL(statusPVChanged()));
	connect(waitFor_, SIGNAL(valueChanged()), this, SIGNAL(waitForChanged()));
	connect(delay_, SIGNAL(valueChanged()), this, SIGNAL(delayChanged()));
	connect(waitPV_, SIGNAL(valueChanged()), this, SIGNAL(waitPVChanged()));
	connect(waitValue_, SIGNAL(valueChanged()), this, SIGNAL(waitValueChanged()));
}

bool CLSSynchronizedDwellTimeConfiguration::isConnected() const
{
	return connected_;
}

void CLSSynchronizedDwellTimeConfiguration::onConnectedChanged()
{
	bool connectedNow = name_->isConnected()
				&& dwellTimePV_->isConnected()
				&& scale_->isConnected()
				&& offset_->isConnected()
				&& units_->isConnected()
				&& modePV_->isConnected()
				&& singleShot_->isConnected()
				&& continuous_->isConnected()
				&& triggerPV_->isConnected()
				&& trigger_->isConnected()
				&& preTrigger_->isConnected()
				&& dwellHold_->isConnected()
				&& statusPV_->isConnected()
				&& waitFor_->isConnected()
				&& delay_->isConnected()
				&& waitPV_->isConnected()
				&& waitValue_->isConnected();

	if (connected_ != connectedNow){

		connected_ = connectedNow;
		emit connected(connected_);
	}
}

void CLSSynchronizedDwellTimeConfiguration::setName(const QString &value)
{
	name_->setValue(value);
}

void CLSSynchronizedDwellTimeConfiguration::setDwellTimePV(const QString &value)
{
	dwellTimePV_->setValue(value);
}

void CLSSynchronizedDwellTimeConfiguration::setScale(const QString &value)
{
	scale_->setValue(value);
}

void CLSSynchronizedDwellTimeConfiguration::setOffset(const QString &value)
{
	offset_->setValue(value);
}

void CLSSynchronizedDwellTimeConfiguration::setUnits(const QString &value)
{
	units_->setValue(value);
}

void CLSSynchronizedDwellTimeConfiguration::setModePV(const QString &value)
{
	modePV_->setValue(value);
}

void CLSSynchronizedDwellTimeConfiguration::setSingleShot(const QString &value)
{
	singleShot_->setValue(value);
}

void CLSSynchronizedDwellTimeConfiguration::setContinuous(const QString &value)
{
	continuous_->setValue(value);
}

void CLSSynchronizedDwellTimeConfiguration::setTriggerPV(const QString &value)
{
	triggerPV_->setValue(value);
}

void CLSSynchronizedDwellTimeConfiguration::setTrigger(CLSSynchronizedDwellTimeConfigurationInfo::Trigger value)
{
	trigger_->setValue(int(value));
}

void CLSSynchronizedDwellTimeConfiguration::setPreTrigger(double value)
{
	preTrigger_->setValue(value);
}

void CLSSynchronizedDwellTimeConfiguration::setDwellHold(double value)
{
	dwellHold_->setValue(value);
}

void CLSSynchronizedDwellTimeConfiguration::setStatusPV(const QString &value)
{
	statusPV_->setValue(value);
}

void CLSSynchronizedDwellTimeConfiguration::setWaitFor(CLSSynchronizedDwellTimeConfigurationInfo::WaitFor value)
{
	waitFor_->setValue(int(value));
}

void CLSSynchronizedDwellTimeConfiguration::setDelay(double value)
{
	delay_->setValue(value);
}

void CLSSynchronizedDwellTimeConfiguration::setWaitPV(const QString &value)
{
	waitPV_->setValue(value);
}

void CLSSynchronizedDwellTimeConfiguration::setWaitValue(const QString &value)
{
	waitValue_->setValue(value);
}

void CLSSynchronizedDwellTimeConfiguration::configure(const CLSSynchronizedDwellTimeConfigurationInfo &info)
{
	setName(info.name());
	setDwellTimePV(info.dwellTimePV());
	setScale(info.scale());
	setOffset(info.offset());
	setUnits(info.units());
	setModePV(info.modePV());
	setSingleShot(info.singleShot());
	setContinuous(info.continuous());
	setTriggerPV(info.triggerPV());
	setTrigger(info.trigger());
	setPreTrigger(info.preTrigger());
	setDwellHold(info.dwellHold());
	setStatusPV(info.statusPV());
	setWaitFor(info.waitFor());
	setDelay(info.delay());
	setWaitPV(info.waitPV());
	setWaitValue(info.waitValue());
}
