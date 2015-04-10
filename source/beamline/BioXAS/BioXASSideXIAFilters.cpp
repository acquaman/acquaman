#include "BioXASSideXIAFilters.h"

BioXASSideXIAFilters::BioXASSideXIAFilters(QObject *parent) :
    BioXASXIAFilters(parent)
{
	// Initialize inherited variables.

	filter1_ = new AMPVControl("XIA Filter 1", "PFIL1607-6-I22-01:ctl", "PFIL1607-6-I22-01:ctl", QString(), this);
	filter2_ = new AMPVControl("XIA Filter 2", "PFIL1607-6-I22-02:ctl", "PFIL1607-6-I22-02:ctl", QString(), this);
	filter3_ = new AMPVControl("XIA Filter 3", "PFIL1607-6-I22-03:ctl", "PFIL1607-6-I22-03:ctl", QString(), this);
	filter4_ = new AMPVControl("XIA Filter 4", "PFIL1607-6-I22-04:ctl", "PFIL1607-6-I22-04:ctl", QString(), this);
}

BioXASSideXIAFilters::~BioXASSideXIAFilters()
{

}
