#include "Beamline.h"

Beamline* Beamline::instance_ = 0;


Beamline::Beamline() : Control("beamline", "n/a") {
	
    ringCurrent_ = new ReadOnlyPVControl("ringCurrent", PVNames::ringCurrent, "", this);
        children_ << ringCurrent_;

		spectrometer_ = new Spectrometer("spectrometer", this);
		children_ << spectrometer_;
	
}

Beamline::~Beamline()
{
}

Beamline* Beamline::bl() {

	if(instance_ == 0)
		instance_ = new Beamline();
		
	return instance_;	
	
}

void Beamline::releaseBl() {

	if(instance_) {
		delete instance_;
		instance_ = 0;
	}	
	
}


