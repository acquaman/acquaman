#include "BioXASMainXIAFilters.h"

BioXASMainXIAFilters::BioXASMainXIAFilters(QObject *parent) :
	BioXASXIAFilters("MainXIAFilters", parent)
{
	// Initialize inherited variables.

	filter1_ = new BioXASXIAFiltersActuator(name()+"1", "PFIL1607-7-I21-01", this);
	filter2_ = new BioXASXIAFiltersActuator(name()+"2", "PFIL1607-7-I21-02", this);
	filter3_ = new BioXASXIAFiltersActuator(name()+"3", "PFIL1607-7-I21-03", this);
	filter4_ = new BioXASXIAFiltersActuator(name()+"4", "PFIL1607-7-I21-04", this);

	filtersControl_ = new BioXASXIAFiltersControl(name() + "Control", this);
	filtersControl_->addFilterActuatorControl(filter1_, 0.25);
	filtersControl_->addFilterActuatorControl(filter2_, 0.5);
	filtersControl_->addFilterActuatorControl(filter3_, 1.0);
	filtersControl_->addFilterActuatorControl(filter4_, 10.0);

	// Make connections.

	connect( filtersControl_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// Current settings.

	updateConnected();
}

BioXASMainXIAFilters::~BioXASMainXIAFilters()
{

}
