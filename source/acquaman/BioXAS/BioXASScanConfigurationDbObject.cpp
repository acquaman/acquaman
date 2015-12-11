#include "BioXASScanConfigurationDbObject.h"

BioXASScanConfigurationDbObject::BioXASScanConfigurationDbObject(QObject *parent)
	: AMDbObject(parent)
{
	energy_ = 0.0;
	edge_ = "";
}

BioXASScanConfigurationDbObject::BioXASScanConfigurationDbObject(const BioXASScanConfigurationDbObject &original)
	: AMDbObject(original)
{
	energy_ = original.energy();
	edge_ = original.edge();


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
