#include "Beamline.h"

AMBeamline* AMBeamline::instance_ = 0;


AMBeamline::AMBeamline() : AMControl("beamline", "n/a") {
	
    ringCurrent_ = new AMReadOnlyPVControl("ringCurrent", AMPVNames::ringCurrent, "", this);
        children_ << ringCurrent_;

                spectrometer_ = new AMSpectrometer("spectrometer", this);
		children_ << spectrometer_;
	
}

AMBeamline::~AMBeamline()
{
}

AMBeamline* AMBeamline::bl() {

	if(instance_ == 0)
                instance_ = new AMBeamline();
		
	return instance_;	
	
}

void AMBeamline::releaseBl() {

	if(instance_) {
		delete instance_;
		instance_ = 0;
	}	
	
}


