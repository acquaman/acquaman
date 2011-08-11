#include "VESPERSBeamSelector.h"

VESPERSBeamSelector::VESPERSBeamSelector(QObject *parent) :
	QObject(parent)
{
	positions_.insert(Pink, -54278);
	positions_.insert(TenPercent, -103803);
	positions_.insert(Si, -123612);
	positions_.insert(OnePointSixPercent, -143422);

	motor_ = new CLSVMEMotor("MonoBeamSelectionMotor", "SMTR1607-1-B20-21", "Motor that controls which beam makes it down the pipe.", false, 0.1, 2.0, this);
	connect(motor_, SIGNAL(movingChanged(bool)), this, SLOT(determineBeam(bool)));
}

void VESPERSBeamSelector::changeBeam(Beam beam)
{
	// If we are already at the new beam position and the internal state of the beam is the same, then don't do anything.
	if (beam_ == beam && isAtBeam(beam))
		return;


}

void VESPERSBeamSelector::determineBeam()
{
	Beam temp;

	if (isAtBeam(Pink))
		temp = Pink;
	else if (isAtBeam(TenPercent))
		temp = TenPercent;
	else if (isAtBeam(OnePointSixPercent))
		temp = OnePointSixPercent;
	else if (isAtBeam(Si))
		temp = Si;
	else
		temp = None;

	if (temp != beam_){

		beam_ = temp;
		emit currentBeamChanged(beam_);
	}
}

