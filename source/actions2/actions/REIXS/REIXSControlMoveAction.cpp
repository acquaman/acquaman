#include "REIXSControlMoveAction.h"

#include "beamline/REIXS/REIXSBeamline.h"

AMControl * REIXSControlMoveAction::getControlForAction(const AMControlInfo &controlInfo) const
{
	// returns 0 if the name of the control is not found in the beamline's allControlsSet().
	return REIXSBeamline::bl()->allControlsSet()->controlNamed(controlInfo.name());
}
