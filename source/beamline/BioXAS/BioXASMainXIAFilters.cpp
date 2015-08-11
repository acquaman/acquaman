#include "BioXASMainXIAFilters.h"

BioXASMainXIAFilters::BioXASMainXIAFilters(QObject *parent) :
	BioXASXIAFilters("MainXIAFilters", parent)
{
	// Initialize inherited variables.

	filter1_ = new CLSBiStateControl("XIA Filter 1", "XIA Filter 1", "PFIL1607-7-I21-01:ctl", "PFIL1607-7-I21-01:OprOpen", "PFIL1607-7-I21-01:OprClose", new AMControlStatusCheckerDefault(2), this);
	filter2_ = new CLSBiStateControl("XIA Filter 2", "XIA Filter 2", "PFIL1607-7-I21-02:ctl", "PFIL1607-7-I21-02:OprOpen", "PFIL1607-7-I21-02:OprClose", new AMControlStatusCheckerDefault(2), this);
	filter3_ = new CLSBiStateControl("XIA Filter 3", "XIA Filter 3", "PFIL1607-7-I21-03:ctl", "PFIL1607-7-I21-03:OprOpen", "PFIL1607-7-I21-03:OprClose", new AMControlStatusCheckerDefault(2), this);
	filter4_ = new CLSBiStateControl("XIA Filter 4", "XIA Filter 4", "PFIL1607-7-I21-04:ctl", "PFIL1607-7-I21-04:OprOpen", "PFIL1607-7-I21-04:OprClose", new AMControlStatusCheckerDefault(2), this);

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
