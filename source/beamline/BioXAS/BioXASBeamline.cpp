#include "BioXASBeamline.h"
#include "beamline/BioXAS/BioXASMonochromator.h"
#include "beamline/BioXAS/BioXASM2Mirror.h"

BioXASBeamline::BioXASBeamline(const QString &controlName) :
	CLSBeamline(controlName)
{

}

BioXASBeamline::~BioXASBeamline()
{

}
