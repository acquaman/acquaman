#include "BioXASSideFilterFlipper.h"

BioXASSideFilterFlipper::BioXASSideFilterFlipper(QObject *parent) :
	BioXASFilterFlipper("SideFilterFlipper", parent)
{
	setSlides(new AMPVControl("PFIL1607-6-I22-05:Slide:Current", "PFIL1607-6-I22-05:Slide:Current", "PFIL1607-6-I22-05:Slide:Select", "", this, 0.1, 230));
	setFilters(new BioXASFilterFlipperFilters(name()+"Filters", this));
	setSlideChangeStatus(new AMReadOnlyPVControl("PFIL1607-6-I22-05:Status", "PFIL1607-6-I22-05:Status", this));
	setRunMode(new AMSinglePVControl("PFIL1607-6-I22-05:RunMode", "PFIL1607-6-I22-05:RunMode", this));
	setNextStepTrigger(new AMSinglePVControl("PFIL1607-6-I22-05:goToNextStep", "PFIL1607-6-I22-05:goToNextStep", this));
	setReceiverStatus(new AMReadOnlyPVControl("BFF1607-6-01:Slide:InReceiver", "BFF1607-6-01:Slide:InReceiver", this));
//	setReceiverStageCWLimit(new AMReadOnlyPVControl("SMTR1607-6-I22-20:cw", "SMTR1607-6-I22-20:cw", this));
//	setReceiverStageCCWLimit(new AMReadOnlyPVControl("SMTR1607-6-I22-20:ccw", "SMTR1607-6-I22-20:ccw", this));
//	setCartridgeStageCWLimit(new AMReadOnlyPVControl("SMTR1607-6-I22-21:cw", "SMTR1607-6-I22-21:cw", this));
//	setCartridgeStageCCWLimit(new AMReadOnlyPVControl("SMTR1607-6-I22-21:ccw", "SMTR1607-6-I22-21:ccw", this));
}

BioXASSideFilterFlipper::~BioXASSideFilterFlipper()
{

}
