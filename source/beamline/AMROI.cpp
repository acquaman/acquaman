#include "AMROI.h"

AMROI::AMROI(QString name, double energy, double width, double scale, AMProcessVariable *namePV, AMProcessVariable *lowPV, AMProcessVariable *highPV, AMProcessVariable *valuePV, QObject *parent)
	: QObject(parent)
{
	setScale(scale);
	setAllPVs(namePV, lowPV, highPV, valuePV);
	setRegion(name, energy, width);
}

AMROI::AMROI(AMROIInfo info, AMProcessVariable *namePV, AMProcessVariable *lowPV, AMProcessVariable *highPV, AMProcessVariable *valuePV, QObject *parent)
	: QObject(parent)
{
	setAllPVs(namePV, lowPV, highPV, valuePV);
	setRegion(info.name(), info.energy(), info.width());
}

AMROI::AMROI(QString name, double energy, double width, double scale, QList<AMProcessVariable *> namePVs, QList<AMProcessVariable *> lowPVs, QList<AMProcessVariable *> highPVs, QList<AMProcessVariable *> valuePVs, QObject *parent)
	: QObject(parent)
{
	setScale(scale);
	setAllPVs(namePVs, lowPVs, highPVs, valuePVs);
	setRegion(name, energy, width);
}

AMROI::AMROI(AMROIInfo info, QList<AMProcessVariable *> namePVs, QList<AMProcessVariable *> lowPVs, QList<AMProcessVariable *> highPVs, QList<AMProcessVariable *> valuePVs, QObject *parent)
	: QObject(parent)
{
	setAllPVs(namePVs, lowPVs, highPVs, valuePVs);
	setRegion(info.name(), info.energy(), info.width());
}

AMROIInfo AMROI::toInfo()
{
	return AMROIInfo(name_, energy_, width_, scale_);
}

void AMROI::fromInfo(const AMROIInfo &info)
{
	setName(info.name());
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
		if (pvNames_.at(i)->isConnected())
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
		if (pvLowerBounds_.at(i)->isConnected())
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
		if (pvHigherBounds_.at(i)->isConnected())
			pvHigherBounds_.at(i)->setValue(high);
}

void AMROI::setHigh(double high)
{
	int newHigh = high/scale_;
	setHigh(newHigh);
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

void AMROI::setRegion(const AMROIInfo &info)
{
	setRegion(info.name(), info.energy(), info.energy());
}

void AMROI::setNamePVs(QList<AMProcessVariable *> namePVs)
{
	// Disconnecting all the old PVs from the slots here and then connecting the new PVs to those slots.
	for (int i = 0; i < pvNames_.size(); i++){

		disconnect(pvNames_.at(i), SIGNAL(valueChanged(QString)), this, SLOT(onNamePVChanged(QString)));
		disconnect(pvNames_.at(i), SIGNAL(connected(bool)), this, SLOT(connected()));
		disconnect(pvNames_.at(i), SIGNAL(hasValuesChanged(bool)), this, SLOT(onHasValuesChanged()));
	}

	pvNames_ = namePVs;

	for (int i = 0; i < pvNames_.size(); i++){

		connect(pvNames_.at(i), SIGNAL(valueChanged(QString)), this, SLOT(onNamePVChanged(QString)));
		connect(pvNames_.at(i), SIGNAL(connected(bool)), this, SLOT(connected()));
		connect(pvNames_.at(i), SIGNAL(hasValuesChanged(bool)), this, SLOT(onHasValuesChanged()));
	}
}

void AMROI::setNamePV(AMProcessVariable *namePV)
{
	QList<AMProcessVariable *> list;
	list << namePV;
	setNamePVs(list);
}

void AMROI::setLowerBoundPVs(QList<AMProcessVariable *> lowPVs)
{
	// Disconnecting all the old PVs from the slots here and then connecting the new PVs to those slots.
	for (int i = 0; i < pvLowerBounds_.size(); i++){

		disconnect(pvLowerBounds_.at(i), SIGNAL(valueChanged(int)), this, SLOT(onLowPVChanged(int)));
		disconnect(pvLowerBounds_.at(i), SIGNAL(connected(bool)), this, SLOT(connected()));
		disconnect(pvLowerBounds_.at(i), SIGNAL(hasValuesChanged(bool)), this, SLOT(onHasValuesChanged()));
	}

	pvLowerBounds_ = lowPVs;

	for (int i = 0; i < pvLowerBounds_.size(); i++){

		connect(pvLowerBounds_.at(i), SIGNAL(valueChanged(int)), this, SLOT(onLowPVChanged(int)));
		connect(pvLowerBounds_.at(i), SIGNAL(connected(bool)), this, SLOT(connected()));
		connect(pvLowerBounds_.at(i), SIGNAL(hasValuesChanged(bool)), this, SLOT(onHasValuesChanged()));
	}
}

void AMROI::setLowerBoundPV(AMProcessVariable *lowPV)
{
	QList<AMProcessVariable *> list;
	list << lowPV;
	setLowerBoundPVs(list);
}

void AMROI::setHigherBoundPVs(QList<AMProcessVariable *> highPVs)
{
	// Disconnecting all the old PVs from the slots here and then connecting the new PVs to those slots.
	for (int i = 0; i < pvHigherBounds_.size(); i++){

		disconnect(pvHigherBounds_.at(i), SIGNAL(valueChanged(int)), this, SLOT(onHighPVChanged(int)));
		disconnect(pvHigherBounds_.at(i), SIGNAL(connected(bool)), this, SLOT(connected()));
		disconnect(pvHigherBounds_.at(i), SIGNAL(hasValuesChanged(bool)), this, SLOT(onHasValuesChanged()));
	}

	pvHigherBounds_ = highPVs;

	for (int i = 0; i < pvHigherBounds_.size(); i++){

		connect(pvHigherBounds_.at(i), SIGNAL(valueChanged(int)), this, SLOT(onHighPVChanged(int)));
		connect(pvHigherBounds_.at(i), SIGNAL(connected(bool)), this, SLOT(connected()));
		connect(pvHigherBounds_.at(i), SIGNAL(hasValuesChanged(bool)), this, SLOT(onHasValuesChanged()));
	}
}

void AMROI::setHigherBoundPV(AMProcessVariable *highPV)
{
	QList<AMProcessVariable *> list;
	list << highPV;
	setHigherBoundPVs(list);
}

void AMROI::setValuePVs(QList<AMProcessVariable *> valuePVs)
{
	// Disconnecting all the old PVs from the slots here and then connecting the new PVs to those slots.
	for (int i = 0; i < pvValues_.size(); i++){

		disconnect(pvValues_.at(i), SIGNAL(valueChanged()), this, SLOT(updateValue()));
		disconnect(pvValues_.at(i), SIGNAL(connected(bool)), this, SLOT(connected()));
		disconnect(pvValues_.at(i), SIGNAL(hasValuesChanged(bool)), this, SLOT(onHasValuesChanged()));
	}

	pvValues_ = valuePVs;

	for (int i = 0; i < pvValues_.size(); i++){

		connect(pvValues_.at(i), SIGNAL(valueChanged()), this, SLOT(updateValue()));
		connect(pvValues_.at(i), SIGNAL(connected(bool)), this, SLOT(connected()));
		connect(pvValues_.at(i), SIGNAL(hasValuesChanged(bool)), this, SLOT(onHasValuesChanged()));
	}
}

void AMROI::setValuePV(AMProcessVariable *valuePV)
{
	QList<AMProcessVariable *> list;
	list << valuePV;
	setValuePVs(list);
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

void AMROI::connected()
{
	connected_ = true;

	for (int i = 0; i < pvNames_.size(); i++)
		connected_ = connected_ && pvNames_.at(i)->isConnected();
	for (int i = 0; i < pvLowerBounds_.size(); i++)
		connected_ = connected_ && pvLowerBounds_.at(i)->isConnected();
	for (int i = 0; i < pvHigherBounds_.size(); i++)
		connected_ = connected_ && pvHigherBounds_.at(i)->isConnected();
	for (int i = 0; i < pvValues_.size(); i++)
		connected_ = connected_ && pvValues_.at(i)->isConnected();

	emit roiConnected(connected_);
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

	emit roiHasValues(hasValues_);
}
