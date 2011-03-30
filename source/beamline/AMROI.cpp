#include "AMROI.h"

AMROI::AMROI(QObject *parent) :
	QObject(parent)
{
}

const AMROIInfo &AMROI::toInfo()
{
	return AMROIInfo(name_, energy_, width_, scale_);
}

void AMROI::fromInfo(const AMROIInfo &info)
{
	name_ = info.name();
	energy_ = info.energy();
	width_ = info.width();
	scale_ = info.scale();
	computeLimits();
}

void AMROI::computeLimits()
{
	setLow(energy_*(1-width_/2));
	setHigh(energy_*(1+width_/2));
}

void AMROI::setName(QString name)
{
	name_ = name;

	for (int i = 0; i < pvNames_.size(); i++)
		pvNames_.at(i)->setValue(name);
}

void AMROI::setEnergy(double energy)
{
	energy_ = energy;
	computeLimits();
}

void AMROI::setWidth(double width)
{
	width_ = width;
	computeLimits();
}

void AMROI::setLow(int low)
{
	low_ = low;

	for (int i = 0; i < pvLowerBounds_.size(); i++)
		pvLowerBounds_.at(i)->setValue(low);
}

void AMROI::setLow(double low)
{
	setLow((int)low*scale_);
}

void AMROI::setHigh(int high)
{
	high_ = high;

	for (int i = 0; i < pvHigherBounds_.size(); i++)
		pvHigherBounds_.at(i)->setValue(high);
}

void AMROI::setHigh(double high)
{
	setHigh((int)high*scale_);
}

void AMROI::setRegion(QString name, double energy, double width)
{
	name_ = name;
	energy_ = energy;
	width_ = width;
	computeLimits();
}

void AMROI::setRegion(QString name, int low, int high)
{
	setName(name);
	energy_ = (high+low)*scale_/2;
	width_ = (high-low)*scale_;
	setLow(low);
	setHigh(high);
}

void AMROI::setNamePVs(QList<AMProcessVariable *> namePVs)
{
	// Disconnecting all the old PVs from the slots here and then connecting the new PVs to those slots.
	for (int i = 0; i < pvNames_.size(); i++)
		disconnect(pvNames_.at(i), SIGNAL(valueChanged(QString)), this, SLOT(setName(QString)));

	pvNames_ = namePVs;

	for (int i = 0; i < pvNames_.size(); i++)
		connect(pvNames_.at(i), SIGNAL(valueChanged(QString)), this, SLOT(setName(QString)));
}

void AMROI::setNamePV(AMProcessVariable *namePV)
{
	// This is meant as a convenience function for single element detectors.  If the previous list did not have one element then there was an error somewhere, either coding or human.
	if (pvNames_.size() != 1)
		return;

	disconnect(pvNames_.first(), SIGNAL(valueChanged(QString)), this, SLOT(setName(QString)));
	pvNames_.replace(0, namePV);
	connect(pvNames_.first(), SIGNAL(valueChanged(QString)), this, SLOT(setName(QString)));
}

void AMROI::setLowerBoundPVs(QList<AMProcessVariable *> lowPVs)
{
	// Disconnecting all the old PVs from the slots here and then connecting the new PVs to those slots.
	for (int i = 0; i < pvLowerBounds_.size(); i++)
		disconnect(pvLowerBounds_.at(i), SIGNAL(valueChanged(int)), this, SLOT(setLow(int)));

	pvLowerBounds_ = lowPVs;

	for (int i = 0; i < pvLowerBounds_.size(); i++)
		connect(pvLowerBounds_.at(i), SIGNAL(valueChanged(int)), this, SLOT(setLow(int)));
}

void AMROI::setLowerBoundPV(AMProcessVariable *lowPV)
{
	// This is meant as a convenience function for single element detectors.  If the previous list did not have one element then there was an error somewhere, either coding or human.
	if (pvNames_.size() != 1)
		return;

	disconnect(pvLowerBounds_.first(), SIGNAL(valueChanged(int)), this, SLOT(setLow(int)));
	pvLowerBounds_.replace(0, lowPV);
	connect(pvLowerBounds_.first(), SIGNAL(valueChanged(int)), this, SLOT(setLow(int)));
}

void AMROI::setHigherBoundPVs(QList<AMProcessVariable *> highPVs)
{
	// Disconnecting all the old PVs from the slots here and then connecting the new PVs to those slots.
	for (int i = 0; i < pvHigherBounds_.size(); i++)
		disconnect(pvHigherBounds_.at(i), SIGNAL(valueChanged(int)), this, SLOT(setHigh(int)));

	pvHigherBounds_ = highPVs;

	for (int i = 0; i < pvHigherBounds_.size(); i++)
		connect(pvHigherBounds_.at(i), SIGNAL(valueChanged(int)), this, SLOT(setHigh(int)));
}

void AMROI::setHigherBoundPV(AMProcessVariable *highPV)
{
	// This is meant as a convenience function for single element detectors.  If the previous list did not have one element then there was an error somewhere, either coding or human.
	if (pvNames_.size() != 1)
		return;

	disconnect(pvHigherBounds_.first(), SIGNAL(valueChanged(int)), this, SLOT(setHigh(int)));
	pvHigherBounds_.replace(0, highPV);
	connect(pvHigherBounds_.first(), SIGNAL(valueChanged(int)), this, SLOT(setHigh(int)));
}

void AMROI::setValuePVs(QList<AMProcessVariable *> valuePVs)
{
	// Disconnecting all the old PVs from the slots here and then connecting the new PVs to those slots.
	for (int i = 0; i < pvValues_.size(); i++)
		disconnect(pvValues_.at(i), SIGNAL(valueChanged()), this, SLOT(updateValue()));

	pvValues_ = valuePVs;

	for (int i = 0; i < pvValues_.size(); i++)
		connect(pvValues_.at(i), SIGNAL(valueChanged()), this, SLOT(updateValue()));
}

void AMROI::setValuePV(AMProcessVariable *valuePV)
{
	// This is meant as a convenience function for single element detectors.  If the previous list did not have one element then there was an error somewhere, either coding or human.
	if (pvNames_.size() != 1)
		return;

	disconnect(pvValues_.first(), SIGNAL(valueChanged()), this, SLOT(updateValue()));
	pvValues_.replace(0, valuePV);
	connect(pvValues_.first(), SIGNAL(valueChanged()), this, SLOT(updateValue()));
}

void AMROI::setAllPVs(QList<AMProcessVariable *> namePVs, QList<AMProcessVariable *> lowPVs, QList<AMProcessVariable *> highPVs, QList<AMProcessVariable *> valuePVs)
{
	setNamePVs(namePVs);
	setLowerBoundPVs(lowPVs);
	setHigherBoundPVs(highPVs);
	setValuePVs(valuePVs);
}

void AMROI::setAllPVs(AMProcessVariable *namePV, AMProcessVariable *lowPV, AMProcessVariable *highPV, AMProcessVariable *valuePV)
{
	setNamePV(namePV);
	setLowerBoundPV(lowPV);
	setHigherBoundPV(highPV);
	setValuePV(valuePV);
}

void AMROI::updateValue()
{
	if (pvValues_.size() == 1)
		value_ = pvValues_.first()->getDouble();

	else{

		value_ = 0;

		for (int i = 0; i < pvValues_.size(); i++)
			value_ += pvValues_.at(i)->getDouble();
	}
}
