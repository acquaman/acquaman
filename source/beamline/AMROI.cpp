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

		connect(namePV, SIGNAL(hasValuesChanged(bool)), this, SLOT(onHasValuesChanged()));
		connect(lowPV, SIGNAL(hasValuesChanged(bool)), this, SLOT(onHasValuesChanged()));
		connect(highPV, SIGNAL(hasValuesChanged(bool)), this, SLOT(onHasValuesChanged()));
		connect(valPV, SIGNAL(hasValuesChanged(bool)), this, SLOT(onHasValuesChanged()));

		pvNames_ << namePV;
		pvLowerBounds_ << lowPV;
		pvHigherBounds_ << highPV;
		pvValues_ << valPV;
	}

	connect(pvNames_.first(), SIGNAL(valueChanged(QString)), this, SLOT(onNamePVChanged(QString)));
	connect(pvLowerBounds_.first(), SIGNAL(valueChanged(int)), this, SLOT(onLowPVChanged(int)));
	connect(pvHigherBounds_.first(), SIGNAL(valueChanged(int)), this, SLOT(onHighPVChanged(int)));
	connect(pvValues_.first(), SIGNAL(valueChanged()), this, SLOT(updateValue()));

	if (elements > 1){

		for (int i = 1; i < elements; i++){

			connect(pvNames_.first(), SIGNAL(valueChanged(QString)), pvNames_.at(i), SLOT(setValue(QString)));
			connect(pvLowerBounds_.first(), SIGNAL(valueChanged(int)), pvLowerBounds_.at(i), SLOT(setValue(int)));
			connect(pvHigherBounds_.first(), SIGNAL(valueChanged(int)), pvHigherBounds_.at(i), SLOT(setValue(int)));
		}
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

	if (pvNames_.first()->hasValues())
		pvNames_.first()->setValue(name);
}

void AMROI::setLow(int low)
{
	low_ = low;

	if (pvLowerBounds_.first()->hasValues())
		pvLowerBounds_.first()->setValue(low);
}

void AMROI::setLow(double low)
{
	int newLow = low/scale_;
	setLow(newLow);
}

void AMROI::setHigh(int high)
{
	high_ = high;

	if (pvHigherBounds_.first()->hasValues())
		pvHigherBounds_.first()->setValue(high);
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
