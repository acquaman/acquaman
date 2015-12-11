#include "BioXASSideXIAFilters.h"
#include "beamline/BioXAS/BioXASXIAFiltersActuator.h"

BioXASSideXIAFilters::BioXASSideXIAFilters(QObject *parent) :
	BioXASXIAFilters("BioXASSideXIAFilters", parent)
{
	// Initialize inherited variables.

	filter1_ = new BioXASXIAFiltersActuator(name()+"1", "PFIL1607-6-I22-01", this);
	connect( filter1_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	filter2_ = new BioXASXIAFiltersActuator(name()+"2", "PFIL1607-6-I22-02", this);
	connect( filter2_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	filter3_ = new BioXASXIAFiltersActuator(name()+"3", "PFIL1607-6-I22-03", this);
	connect( filter3_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	filter4_ = new BioXASXIAFiltersActuator(name()+"4", "PFIL1607-6-I22-04", this);
	connect( filter4_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

//	filtersControl_ = new BioXASXIAFiltersControl(name() + " Control", this);
//	filtersControl_->addFilterActuatorControl(filter1_, 0.25);
//	filtersControl_->addFilterActuatorControl(filter2_, 0.5);
//	filtersControl_->addFilterActuatorControl(filter3_, 1.0);
//	filtersControl_->addFilterActuatorControl(filter4_, 10.0);

	// Current settings.

	updateConnected();
}

BioXASSideXIAFilters::~BioXASSideXIAFilters()
{

}
