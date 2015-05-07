#include "BioXASSideMonochromatorRegionControl.h"
#include "beamline/BioXAS/BioXASSideBeamline.h"
#include "beamline/AMPVControl.h"

BioXASSideMonochromatorRegionControl::BioXASSideMonochromatorRegionControl(QObject *parent) :
	BioXASSSRLMonochromatorRegionControl(parent)
{


	updateValue();
}

BioXASSideMonochromatorRegionControl::~BioXASSideMonochromatorRegionControl()
{

}
