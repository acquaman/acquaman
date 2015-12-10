#include "BioXASMasterValves.h"
#include "beamline/BioXAS/BioXASFrontEndValves.h"
#include "beamline/BioXAS/BioXASSideValves.h"
#include "beamline/BioXAS/BioXASMainValves.h"
#include "beamline/BioXAS/BioXASImagingValves.h"

BioXASMasterValves::BioXASMasterValves(QObject *parent) :
	BioXASBiStateGroup("BioXASMasterValves", parent)
{
	frontEndValves_ = new BioXASFrontEndValves(this);
	sideValves_ = new BioXASSideValves(this);
	mainValves_ = new BioXASMainValves(this);
	imagingValves_ = new BioXASImagingValves(this);
}

BioXASMasterValves::~BioXASMasterValves()
{

}



