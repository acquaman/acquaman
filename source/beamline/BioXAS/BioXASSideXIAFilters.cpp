#include "BioXASSideXIAFilters.h"

BioXASSideXIAFilters::BioXASSideXIAFilters(QObject *parent) :
    BioXASXIAFilters(parent)
{
	// Initialize inherited variables.

	filter1_ = new CLSBiStateControl("XIA Filter 1", "XIA Filter 1", "PFIL1607-6-I22-01:ctl", "PFIL1607-6-I22-01:OprOpen", "PFIL1607-6-I22-01:OprClose", new AMControlStatusCheckerDefault(2), this);
	filter2_ = new CLSBiStateControl("XIA Filter 2", "XIA Filter 2", "PFIL1607-6-I22-02:ctl", "PFIL1607-6-I22-02:OprOpen", "PFIL1607-6-I22-02:OprClose", new AMControlStatusCheckerDefault(2), this);
	filter3_ = new CLSBiStateControl("XIA Filter 3", "XIA Filter 3", "PFIL1607-6-I22-03:ctl", "PFIL1607-6-I22-03:OprOpen", "PFIL1607-6-I22-03:OprClose", new AMControlStatusCheckerDefault(2), this);
	filter4_ = new CLSBiStateControl("XIA Filter 4", "XIA Filter 4", "PFIL1607-6-I22-04:ctl", "PFIL1607-6-I22-04:OprOpen", "PFIL1607-6-I22-04:OprClose", new AMControlStatusCheckerDefault(2), this);
}

BioXASSideXIAFilters::~BioXASSideXIAFilters()
{

}
