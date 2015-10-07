#include "BioXASFilterFlipper.h"

BioXASFilterFlipper::BioXASFilterFlipper(QObject *parent) :
	BioXASBeamlineComponent("BioXASFilterFlipper", parent)
{
	// Create member variables.

	status_ = new AMReadOnlyPVControl(name()+"Status", "BFF2023-01:Status", this);
	runMode_ = new AMSinglePVControl(name()+"RunMode", "BFF2023-01:RunMode", this);
	goToNextStep_ = new AMSinglePVControl(name()+"NextStep", "BFF2023-01:goToNextStep", this);

	currentSlide_ = new AMPVControl(name()+"CurrentSlide", "BFF2023-01:Slide:Current", "BFF2023-01:Slide:Select", "", this);
	currentSlideStatus_ = new AMReadOnlyPVControl(name()+"CurrentSlideStatus", "BFF2023-01:Slide:InReceiver", this);

	slide1CartridgeStatus_ = new AMReadOnlyPVControl(name()+"Slide1Status", "BFF2023-01:Slide:1", this);
	slide2CartridgeStatus_ = new AMReadOnlyPVControl(name()+"Slide2Status", "BFF2023-01:Slide:2", this);
	slide3CartridgeStatus_ = new AMReadOnlyPVControl(name()+"Slide3Status", "BFF2023-01:Slide:3", this);
	slide4CartridgeStatus_ = new AMReadOnlyPVControl(name()+"Slide4Status", "BFF2023-01:Slide:4", this);
	slide5CartridgeStatus_ = new AMReadOnlyPVControl(name()+"Slide5Status", "BFF2023-01:Slide:5", this);
	slide6CartridgeStatus_ = new AMReadOnlyPVControl(name()+"Slide6Status", "BFF2023-01:Slide:6", this);
	slide7CartridgeStatus_ = new AMReadOnlyPVControl(name()+"Slide7Status", "BFF2023-01:Slide:7", this);
	slide8CartridgeStatus_ = new AMReadOnlyPVControl(name()+"Slide8Status", "BFF2023-01:Slide:8", this);
	slide9CartridgeStatus_ = new AMReadOnlyPVControl(name()+"Slide9Status", "BFF2023-01:Slide:9", this);
	slide10CartridgeStatus_ = new AMReadOnlyPVControl(name()+"Slide10Status", "BFF2023-01:Slide:10", this);
}
