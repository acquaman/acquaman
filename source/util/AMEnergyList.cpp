#include "AMEnergyList.h"

AMEnergyList::AMEnergyList(QObject *parent) :
	QObject(parent)
{
}

AMEnergyList::AMEnergyList(const AMEnergyList &original) :
	QObject()
{
	energies_ = original.energies();
}

void AMEnergyList::insertEnergy(int index, double energy)
{
	energies_.insert(index, energy);
	emit energyAdded(energy);
}

void AMEnergyList::removeEnergy(int index)
{
	double energy = energies_.at(index);
	energies_.removeAt(index);
	emit energyRemoved(energy);
}

void AMEnergyList::setEnergy(int index, double energy)
{
	energies_[index] = energy;
	emit energyChanged(index);
}
