#include "CLSSynchronizedDwellTimeConfigurationInfo.h"

 CLSSynchronizedDwellTimeConfigurationInfo::~CLSSynchronizedDwellTimeConfigurationInfo(){}
CLSSynchronizedDwellTimeConfigurationInfo::CLSSynchronizedDwellTimeConfigurationInfo(QObject *parent)
	: QObject(parent)
{
	name_ = "";
	dwellTimePV_ = "";
	scale_ = "";
	offset_ = "";
	units_ = "";
	modePV_ = "";
	singleShot_ = "";
	continuous_ = "";
	triggerPV_ = "";
	trigger_ = Normal;
	preTrigger_ = 0.0;
	dwellHold_ = 0.0;
	statusPV_ = "";
	waitFor_ = Nothing;
	delay_ = 0.0;
	waitPV_ = "";
	waitValue_ = "";
}


void CLSSynchronizedDwellTimeConfigurationInfo::setName(const QString &value)
{
	name_ = value;
}

void CLSSynchronizedDwellTimeConfigurationInfo::setDwellTimePV(const QString &value)
{
	dwellTimePV_ = value;
}

void CLSSynchronizedDwellTimeConfigurationInfo::setScale(const QString &value)
{
	scale_ = value;
}

void CLSSynchronizedDwellTimeConfigurationInfo::setOffset(const QString &value)
{
	offset_ = value;
}

void CLSSynchronizedDwellTimeConfigurationInfo::setUnits(const QString &value)
{
	units_ = value;
}

void CLSSynchronizedDwellTimeConfigurationInfo::setModePV(const QString &value)
{
	modePV_ = value;
}

void CLSSynchronizedDwellTimeConfigurationInfo::setSingleShot(const QString &value)
{
	singleShot_ = value;
}

void CLSSynchronizedDwellTimeConfigurationInfo::setContinuous(const QString &value)
{
	continuous_ = value;
}

void CLSSynchronizedDwellTimeConfigurationInfo::setTriggerPV(const QString &value)
{
	triggerPV_ = value;
}

void CLSSynchronizedDwellTimeConfigurationInfo::setTrigger(Trigger value)
{
	trigger_ = value;
}

void CLSSynchronizedDwellTimeConfigurationInfo::setPreTrigger(double value)
{
	preTrigger_ = value;
}

void CLSSynchronizedDwellTimeConfigurationInfo::setDwellHold(double value)
{
	dwellHold_ = value;
}

void CLSSynchronizedDwellTimeConfigurationInfo::setStatusPV(const QString &value)
{
	statusPV_ = value;
}

void CLSSynchronizedDwellTimeConfigurationInfo::setWaitFor(WaitFor value)
{
	waitFor_ = value;
}

void CLSSynchronizedDwellTimeConfigurationInfo::setDelay(double value)
{
	delay_ = value;
}

void CLSSynchronizedDwellTimeConfigurationInfo::setWaitPV(const QString &value)
{
	waitPV_ = value;
}

void CLSSynchronizedDwellTimeConfigurationInfo::setWaitValue(const QString &value)
{
	waitValue_ = value;
}
