#include "BioXASScanConfigurationDbObject.h"

BioXASScanConfigurationDbObject::BioXASScanConfigurationDbObject(QObject *parent)
	: AMDbObject(parent)
{
	energy_ = 0.0;
	edge_ = "";
	collectSpectraPreference_ = false;
	collectICRPreference_ = false;
}

BioXASScanConfigurationDbObject::BioXASScanConfigurationDbObject(const BioXASScanConfigurationDbObject &original)
	: AMDbObject(original)
{
	energy_ = original.energy();
	edge_ = original.edge();
	collectSpectraPreference_ = original.collectSpectraPreference();
	collectICRPreference_ = original.collectICRPreference();
}

BioXASScanConfigurationDbObject::~BioXASScanConfigurationDbObject()
{

}

void BioXASScanConfigurationDbObject::setEnergy(double newEnergy)
{
	if (energy_ != newEnergy){
		energy_ = newEnergy;
		emit energyChanged(energy_);
		setModified(true);
	}
}

void BioXASScanConfigurationDbObject::setEdge(const QString &newEdge)
{
	if (edge_ != newEdge){
		edge_ = newEdge;
		emit edgeChanged(edge_);
		setModified(true);
	}
}

void BioXASScanConfigurationDbObject::setCollectSpectraPreference(bool spectraCollected)
{
	if (collectSpectraPreference_ != spectraCollected) {
		collectSpectraPreference_ = spectraCollected;
		emit collectSpectraPreferenceChanged(collectSpectraPreference_);
		setModified(true);
	}
}

void BioXASScanConfigurationDbObject::setCollectICRPreference(bool icrCollected)
{
	if (collectICRPreference_ != icrCollected) {
		collectICRPreference_ = icrCollected;
		emit collectICRPreferenceChanged(collectICRPreference_);
		setModified(true);
	}
}
