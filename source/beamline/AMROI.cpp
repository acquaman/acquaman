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
	}

	pvNames_ = namePVs;

	for (int i = 0; i < pvNames_.size(); i++){

		connect(pvNames_.at(i), SIGNAL(valueChanged(QString)), this, SLOT(onNamePVChanged(QString)));
		connect(pvNames_.at(i), SIGNAL(connected(bool)), this, SLOT(connected()));
	}
}

void AMROI::setNamePV(AMProcessVariable *namePV)
{
	// This is meant as a convenience function for single element detectors.  If the previous list did not have one element then there was an error somewhere, either coding or human.
	if (pvNames_.size() != 1)
		return;

	disconnect(pvNames_.first(), SIGNAL(valueChanged(QString)), this, SLOT(setName(QString)));
	disconnect(pvNames_.first(), SIGNAL(connected(bool)), this, SLOT(connected()));
	pvNames_.replace(0, namePV);
	connect(pvNames_.first(), SIGNAL(connected(bool)), this, SLOT(connected()));
	connect(pvNames_.first(), SIGNAL(valueChanged(QString)), this, SLOT(setName(QString)));
}

void AMROI::setLowerBoundPVs(QList<AMProcessVariable *> lowPVs)
{
	// Disconnecting all the old PVs from the slots here and then connecting the new PVs to those slots.
	for (int i = 0; i < pvLowerBounds_.size(); i++){

		disconnect(pvLowerBounds_.at(i), SIGNAL(valueChanged(int)), this, SLOT(onLowPVChanged(int)));
		disconnect(pvLowerBounds_.at(i), SIGNAL(connected(bool)), this, SLOT(connected()));
	}

	pvLowerBounds_ = lowPVs;

	for (int i = 0; i < pvLowerBounds_.size(); i++){

		connect(pvLowerBounds_.at(i), SIGNAL(valueChanged(int)), this, SLOT(onLowPVChanged(int)));
		connect(pvLowerBounds_.at(i), SIGNAL(connected(bool)), this, SLOT(connected()));
	}
}

void AMROI::setLowerBoundPV(AMProcessVariable *lowPV)
{
	// This is meant as a convenience function for single element detectors.  If the previous list did not have one element then there was an error somewhere, either coding or human.
	if (pvLowerBounds_.size() != 1)
		return;

	disconnect(pvLowerBounds_.first(), SIGNAL(valueChanged(int)), this, SLOT(onLowPVChanged(int)));
	disconnect(pvLowerBounds_.first(), SIGNAL(connected(bool)), this, SLOT(connected()));
	pvLowerBounds_.replace(0, lowPV);
	connect(pvLowerBounds_.first(), SIGNAL(connected(bool)), this, SLOT(connected()));
	connect(pvLowerBounds_.first(), SIGNAL(valueChanged(int)), this, SLOT(onLowPVChanged(int)));
}

void AMROI::setHigherBoundPVs(QList<AMProcessVariable *> highPVs)
{
	// Disconnecting all the old PVs from the slots here and then connecting the new PVs to those slots.
	for (int i = 0; i < pvHigherBounds_.size(); i++){

		disconnect(pvHigherBounds_.at(i), SIGNAL(valueChanged(int)), this, SLOT(onHighPVChanged(int)));
		disconnect(pvHigherBounds_.at(i), SIGNAL(connected(bool)), this, SLOT(connected()));
	}

	pvHigherBounds_ = highPVs;

	for (int i = 0; i < pvHigherBounds_.size(); i++){

		connect(pvHigherBounds_.at(i), SIGNAL(valueChanged(int)), this, SLOT(onHighPVChanged(int)));
		connect(pvHigherBounds_.at(i), SIGNAL(connected(bool)), this, SLOT(connected()));
	}
}

void AMROI::setHigherBoundPV(AMProcessVariable *highPV)
{
	// This is meant as a convenience function for single element detectors.  If the previous list did not have one element then there was an error somewhere, either coding or human.
	if (pvHigherBounds_.size() != 1)
		return;

	disconnect(pvHigherBounds_.first(), SIGNAL(valueChanged(int)), this, SLOT(onHighPVChanged(int)));
	disconnect(pvHigherBounds_.first(), SIGNAL(connected(bool)), this, SLOT(connected()));
	pvHigherBounds_.replace(0, highPV);
	connect(pvHigherBounds_.first(), SIGNAL(connected(bool)), this, SLOT(connected()));
	connect(pvHigherBounds_.first(), SIGNAL(valueChanged(int)), this, SLOT(onHighPVChanged(int)));
}

void AMROI::setValuePVs(QList<AMProcessVariable *> valuePVs)
{
	// Disconnecting all the old PVs from the slots here and then connecting the new PVs to those slots.
	for (int i = 0; i < pvValues_.size(); i++){

		disconnect(pvValues_.at(i), SIGNAL(valueChanged()), this, SLOT(updateValue()));
		disconnect(pvValues_.at(i), SIGNAL(connected(bool)), this, SLOT(connected()));
	}

	pvValues_ = valuePVs;

	for (int i = 0; i < pvValues_.size(); i++){

		connect(pvValues_.at(i), SIGNAL(valueChanged()), this, SLOT(updateValue()));
		connect(pvValues_.at(i), SIGNAL(connected(bool)), this, SLOT(connected()));
	}
}

void AMROI::setValuePV(AMProcessVariable *valuePV)
{
	// This is meant as a convenience function for single element detectors.  If the previous list did not have one element then there was an error somewhere, either coding or human.
	if (pvValues_.size() != 1)
		return;

	disconnect(pvValues_.first(), SIGNAL(valueChanged()), this, SLOT(updateValue()));
	disconnect(pvValues_.first(), SIGNAL(connected(bool)), this, SLOT(connected()));
	pvValues_.replace(0, valuePV);
	connect(pvValues_.first(), SIGNAL(connected(bool)), this, SLOT(connected()));
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
