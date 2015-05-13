#include "BioXASMainMonochromatorEnergyControl.h"
#include "beamline/BioXAS/BioXASMainBeamline.h"

BioXASMainMonochromatorEnergyControl::BioXASMainMonochromatorEnergyControl(QObject *parent) :
	BioXASSSRLMonochromatorEnergyControl("MainEnergy", parent)
{
//	// Initialize member variables.

//	bragg_ = BioXASMainBeamline::bioXAS()->mono()->braggMotor();
//	braggSetPosition_ = new AMSinglePVControl("BraggSetPositionControl", "SMTR1607-5-I21-12:deg:setPosn", this);
//	region_ = BioXASMainBeamline::bioXAS()->mono()->regionControl();
//	m1Mirror_ = new AMSinglePVControl("M1MirrorOffset", "BL1607-5-I21:Energy:EV:fbk:tr.H", this);
}

BioXASMainMonochromatorEnergyControl::~BioXASMainMonochromatorEnergyControl()
{

}

