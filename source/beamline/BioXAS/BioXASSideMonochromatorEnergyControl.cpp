#include "BioXASSideMonochromatorEnergyControl.h"
#include "beamline/BioXAS/BioXASSideBeamline.h"

BioXASSideMonochromatorEnergyControl::BioXASSideMonochromatorEnergyControl(QObject *parent) :
	BioXASSSRLMonochromatorEnergyControl("SideEnergy", parent)
{
	// Initialize inherited variables.

	bragg_ = BioXASSideBeamline::bioXAS()->mono()->braggMotor();
	braggSetPosition_ = new AMSinglePVControl("BraggSetPositionControl", "SMTR1607-5-I22-12:deg:setPosn", this);
	region_ = BioXASSideBeamline::bioXAS()->mono()->regionControl();
	m1Mirror_ = new AMSinglePVControl("M1MirrorOffset", "BL1607-5-I22:Energy:EV:fbk:tr.H", this);
}

BioXASSideMonochromatorEnergyControl::~BioXASSideMonochromatorEnergyControl()
{

}
