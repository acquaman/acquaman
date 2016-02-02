#include "BioXASSideFilterFlipper.h"

BioXASSideFilterFlipper::BioXASSideFilterFlipper(QObject *parent) :
	BioXASFilterFlipper("SideFilterFlipper", parent)
{
	setSlides(new AMPVControl("BFF1607-6-01:Slide:Current", "BFF1607-6-01:Slide:Current", "BFF1607-6-01:Slide:Select", "", this, 0.1, 230));
	setFilters(new BioXASFilterFlipperFilters(name()+"Filters", this));
	setSlideChangeStatus(new AMReadOnlyPVControl("BFF1607-6-01:Status", "BFF1607-6-01:Status", this));
	setRunMode(new AMSinglePVControl("BFF1607-6-01:RunMode", "BFF1607-6-01:RunMode", this));
	setNextStepTrigger(new AMSinglePVControl("BFF1607-6-01:goToNextStep", "BFF1607-6-01:goToNextStep", this));
	setReceiverStatus(new AMReadOnlyPVControl("BFF1607-6-01:Slide:InReceiver", "BFF1607-6-01:Slide:InReceiver", this));
	setReceiverStageCWLimit(new AMReadOnlyPVControl("SMTR1607-6-I22-20:cw", "SMTR1607-6-I22-20:cw", this));
	setReceiverStageCCWLimit(new AMReadOnlyPVControl("SMTR1607-6-I22-20:ccw", "SMTR1607-6-I22-20:ccw", this));
	setCartridgeStageCWLimit(new AMReadOnlyPVControl("SMTR1607-6-I22-21:cw", "SMTR1607-6-I22-21:cw", this));
	setCartridgeStageCCWLimit(new AMReadOnlyPVControl("SMTR1607-6-I22-21:ccw", "SMTR1607-6-I22-21:ccw", this));
}

BioXASSideFilterFlipper::~BioXASSideFilterFlipper()
{

}
