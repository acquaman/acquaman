#include "REIXSBeamOnOffActionInfo.h"

REIXSBeamOnOffActionInfo::REIXSBeamOnOffActionInfo(bool beamOn, QObject *parent) :
	AMActionInfo(beamOn ? "Beam On" : "Beam Off",
				 beamOn ? "Opens all valves and shutters to turn the beam on." : "Turns the beam off.",
				 QString(),
				 parent)
{
	beamOn_ = beamOn;
}

void REIXSBeamOnOffActionInfo::setBeamOn(bool beamOn) {
	if(beamOn == beamOn_)
		return;


	beamOn_ = beamOn;

	if(beamOn_) {
		setShortDescription("Beam On");
		setLongDescription("Opens all valves and shutters to turn the beam on.");
	}
	else {
		setShortDescription("Beam Off");
		setLongDescription("Turns the beam off.");
	}

	setModified(true);
}
