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


#include "AMROI.h"
#include "util/VESPERS/GeneralUtilities.h"

AMROI::AMROI(QString baseName, int elements, int number, QObject *parent)
	: QObject(parent)
{
	hasValues_ = false;
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

		if (i == 0){

			connect(namePV, SIGNAL(valueChanged(QString)), this, SIGNAL(nameUpdate(QString)));
			connect(lowPV, SIGNAL(valueChanged(int)), this, SIGNAL(lowUpdate(int)));
			connect(highPV, SIGNAL(valueChanged(int)), this, SIGNAL(highUpdate(int)));
			connect(valPV, SIGNAL(valueChanged()), this, SLOT(updateValue()));

			connect(namePV, SIGNAL(valueChanged(QString)), this, SLOT(onRoiChanged()));
			connect(lowPV, SIGNAL(valueChanged(int)), this, SLOT(onRoiChanged()));
			connect(highPV, SIGNAL(valueChanged(int)), this, SLOT(onRoiChanged()));
		}

		connect(namePV, SIGNAL(hasValuesChanged(bool)), this, SLOT(onHasValuesChanged()));
		connect(lowPV, SIGNAL(hasValuesChanged(bool)), this, SLOT(onHasValuesChanged()));
		connect(highPV, SIGNAL(hasValuesChanged(bool)), this, SLOT(onHasValuesChanged()));
		connect(valPV, SIGNAL(hasValuesChanged(bool)), this, SLOT(onHasValuesChanged()));

		pvNames_ << namePV;
		pvLowerBounds_ << lowPV;
		pvHigherBounds_ << highPV;
		pvValues_ << valPV;
	}
}

AMROIInfo AMROI::toInfo()
{
	return AMROIInfo(name(), energy_, low()*scale_, high()*scale_, scale_);
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

void AMROI::setName(QString newName)
{
	if (hasValues_ && newName.compare(name()) != 0)
		for (int i = 0; i < pvNames_.size(); i++)
			pvNames_.at(i)->setValue(newName);
}

void AMROI::setLow(int val)
{
	if (hasValues_ && low() != val)
		for (int i = 0; i < pvLowerBounds_.size(); i++)
			pvLowerBounds_.at(i)->setValue(val);
}

void AMROI::setLow(double low)
{
	setLow((int)(low/scale_));
}

void AMROI::setHigh(int val)
{
	if (hasValues_ && high() != val)
		for (int i = 0; i < pvHigherBounds_.size(); i++)
			pvHigherBounds_.at(i)->setValue(val);
}

void AMROI::setHigh(double high)
{
	setHigh((int)(high/scale_));
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
	setName(info.name());
	energy_ = info.energy();
	setLow(info.low());
	setHigh(info.high());
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
