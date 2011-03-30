#include "AMROI.h"

AMROI::AMROI(QObject *parent) :
	QObject(parent)
{
}

AMROIInfo *AMROI::toInfo()
{
	return new AMROIInfo(name_, energy_, width_, scale_);
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
	setLow(energy_*(1-width_));
	setHigh(energy_*(1+width_));
}
#error "Need to set the values to the PVs."
void AMROI::setName(QString name)
{
	name_ = name;
}

void AMROI::setEnergy(double energy)
{
	energy_ = energy;
}

void AMROI::setWidth(double width)
{
	width_ = width;
}

void AMROI::setLow(int low)
{
	low_ = low;
}

void AMROI::setLow(double low)
{
	setLow((int)low*scale_);
}

void AMROI::setHigh(int high)
{
	high_ = high;
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
	name_ = name;
	low_ = low;
	high_ = high;
}

void AMROI::setNamePVs(QList<AMProcessVariable *> namePVs)
{
	pvNames_ = namePVs;
}

void AMROI::setNamePV(AMProcessVariable *namePV)
{
	pvNames_.replace(0, namePV);
}

void AMROI::setLowerBoundPVs(QList<AMProcessVariable *> lowPVs)
{
	pvLowerBounds_ = lowPVs;
}

void AMROI::setLowerBoundPV(AMProcessVariable *lowPV)
{
	pvLowerBounds_.replace(0, lowPV);
}

void AMROI::setHigherBoundPVs(QList<AMProcessVariable *> highPVs)
{
	pvHigherBounds_ = highPVs;
}

void AMROI::setHigherBoundPV(AMProcessVariable *highPV)
{
	pvHigherBounds_.replace(0, highPV);
}

void AMROI::setValuePVs(QList<AMProcessVariable *> valuePVs)
{
	pvValues_ = valuePVs;
}

void AMROI::setValuePV(AMProcessVariable *valuePV)
{
	pvValues_.replace(0, valuePV);
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
