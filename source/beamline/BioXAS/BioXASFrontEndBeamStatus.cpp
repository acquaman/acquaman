#include "BioXASFrontEndBeamStatus.h"

BioXASFrontEndBeamStatus::BioXASFrontEndBeamStatus(BioXASFrontEndShutters *shutters, BioXASValves *valves, QObject *parent) :
	BioXASFrontEndBeamStatusControl("BioXASFrontEndBeamStatus", parent)
{
	setShutters(shutters);
	setValves(valves);
}

BioXASFrontEndBeamStatus::~BioXASFrontEndBeamStatus()
{

}
