#include "CLSSynchronizedDwellTimeConfiguration.h"

#include <QStringBuilder>

CLSSynchronizedDwellTimeConfiguration::CLSSynchronizedDwellTimeConfiguration(const QString &name, int number, QObject *parent)
	: QObject(parent)
{
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
	statusPV_ = new AMProcessVariable(name % ":stat" % QChar(65+number) % ".clc.INPA", true, this);
	waitFor_ = new AMProcessVariable(name % ":busy" % QChar(65+number) % ":type", true, this);
	delay_ = new AMProcessVariable(name % ":busy" % QChar(65+number) % ":timer.DLY2", true, this);
	waitPV_ = new AMProcessVariable(name % ":busyVal" % QChar(65+number) % ".INPA", true, this);
	waitValue_ = new AMProcessVariable(name % ":busyVal" % QChar(65+number) % ".INPB", true, this);

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
