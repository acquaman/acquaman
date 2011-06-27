#include "AMROI.h"
#include "util/VESPERS/GeneralUtilities.h"

AMROI::AMROI(QString baseName, int elements, int number, QObject *parent)
	: QObject(parent)
{
	connected_ = false;
	setScale(1);
	buildAllPVs(baseName, elements, number);
	setRegion(QString(), -1, -1);
}

void AMROI::buildAllPVs(QString baseName, int elements, int number)
{
	AMProcessVariable *namePV;
	AMProcessVariable *lowPV;
	AMProcessVariable *highPV;
	AMProcessVariable *valPV;

	for (int i = 0; i < elements; i++){

		namePV = new AMProcessVariable(baseName+":mca"+QString::number(i+1)+".R"+QString::number(number)+"NM", true, this);
		lowPV = new AMProcessVariable(baseName+":mca"+QString::number(i+1)+".R"+QString::number(number)+"LO", true, this);
		highPV = new AMProcessVariable(baseName+":mca"+QString::number(i+1)+".R"+QString::number(number)+"HI", true, this);
		valPV = new AMProcessVariable(baseName+":mca"+QString::number(i+1)+".R"+QString::number(number), true, this);

		namePV->disablePutCallbackMode(true);
		lowPV->disablePutCallbackMode(true);
		highPV->disablePutCallbackMode(true);
		valPV->disablePutCallbackMode(true);

		connect(namePV, SIGNAL(valueChanged(QString)), this, SLOT(onNamePVChanged(QString)));
		connect(namePV, SIGNAL(connected(bool)), this, SLOT(connected()));
		connect(namePV, SIGNAL(hasValuesChanged(bool)), this, SLOT(onHasValuesChanged()));
		connect(lowPV, SIGNAL(valueChanged(int)), this, SLOT(onLowPVChanged(int)));
		connect(lowPV, SIGNAL(connected(bool)), this, SLOT(connected()));
		connect(lowPV, SIGNAL(hasValuesChanged(bool)), this, SLOT(onHasValuesChanged()));
		connect(highPV, SIGNAL(valueChanged(int)), this, SLOT(onHighPVChanged(int)));
		connect(highPV, SIGNAL(connected(bool)), this, SLOT(connected()));
		connect(highPV, SIGNAL(hasValuesChanged(bool)), this, SLOT(onHasValuesChanged()));
		connect(valPV, SIGNAL(valueChanged()), this, SLOT(updateValue()));
		connect(valPV, SIGNAL(connected(bool)), this, SLOT(connected()));
		connect(valPV, SIGNAL(hasValuesChanged(bool)), this, SLOT(onHasValuesChanged()));

		pvNames_ << namePV;
		pvLowerBounds_ << lowPV;
		pvHigherBounds_ << highPV;
		pvValues_ << valPV;
	}
}

AMROIInfo AMROI::toInfo()
{
	return AMROIInfo(name_, energy_, low_, high_, scale_);
}

void AMROI::fromInfo(const AMROIInfo &info)
{
	setName(info.name());
	energy_ = info.energy();
	scale_ = info.scale();
	setLow(info.low());
	setHigh(info.high());
}

void AMROI::computeLimits(double width)
{
	setLow(energy_*(1-width/2));
	setHigh(energy_*(1+width/2));
}

void AMROI::setName(QString name)
{
	name_ = name;

	for (int i = 0; i < pvNames_.size(); i++)
		if (pvNames_.at(i)->hasValues())
			pvNames_.at(i)->setValue(name);
}

void AMROI::setLow(int low)
{
	low_ = low;

	for (int i = 0; i < pvLowerBounds_.size(); i++)
		if (pvLowerBounds_.at(i)->hasValues())
			pvLowerBounds_.at(i)->setValue(low);
}

void AMROI::setLow(double low)
{
	int newLow = low/scale_;
	setLow(newLow);
}

void AMROI::setHigh(int high)
{
	high_ = high;

	for (int i = 0; i < pvHigherBounds_.size(); i++)
		if (pvHigherBounds_.at(i)->hasValues())
			pvHigherBounds_.at(i)->setValue(high);
}

void AMROI::setHigh(double high)
{
	int newHigh = high/scale_;
	setHigh(newHigh);
}

void AMROI::setRegion(QString name, double energy, double width)
{
	setName(name);
	energy_ = energy;
	computeLimits(width);
}

void AMROI::setRegion(QString name, int low, int high)
{
	setName(name);
	energy_ = (high+low)*scale_/2;
	setLow(low);
	setHigh(high);
}

void AMROI::setRegion(const AMROIInfo &info)
{
	setRegion(info.name(), info.low(), info.high());
}

void AMROI::updateValue()
{
	if (pvValues_.size() == 1){

		value_ = pvValues_.first()->getDouble();
		emit valueUpdate(value_);
	}

	else{

		double value = 0;

		for (int i = 0; i < pvValues_.size(); i++)
			value += pvValues_.at(i)->getDouble();

		if (value != value_){

			value_ = value;
			emit valueUpdate(value_);
		}
	}
}

void AMROI::connected()
{
	bool connected = true;

	for (int i = 0; i < pvNames_.size(); i++)
		connected = connected && pvNames_.at(i)->isConnected();
	for (int i = 0; i < pvLowerBounds_.size(); i++)
		connected = connected && pvLowerBounds_.at(i)->isConnected();
	for (int i = 0; i < pvHigherBounds_.size(); i++)
		connected = connected && pvHigherBounds_.at(i)->isConnected();
	for (int i = 0; i < pvValues_.size(); i++)
		connected = connected && pvValues_.at(i)->isConnected();

	if (connected != connected_){

		connected_ = connected;
		emit roiConnected(connected_);
	}
}

void AMROI::onHasValuesChanged()
{
	hasValues_ = true;

	for (int i = 0; i < pvNames_.size(); i++)
		hasValues_ = hasValues_ && pvNames_.at(i)->hasValues();
	for (int i = 0; i < pvLowerBounds_.size(); i++)
		hasValues_ = hasValues_ && pvLowerBounds_.at(i)->hasValues();
	for (int i = 0; i < pvHigherBounds_.size(); i++)
		hasValues_ = hasValues_ && pvHigherBounds_.at(i)->hasValues();
	for (int i = 0; i < pvValues_.size(); i++)
		hasValues_ = hasValues_ && pvValues_.at(i)->hasValues();

	if (hasValues_)
		emit roiHasValues();
}
