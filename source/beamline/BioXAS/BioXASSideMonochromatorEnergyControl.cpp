#include "BioXASSideMonochromatorEnergyControl.h"

BioXASSideMonochromatorEnergyControl::BioXASSideMonochromatorEnergyControl(QObject *parent) :
	BioXASSSRLMonochromatorEnergyControl("EnergyEV", "BL1607-5-I22:Energy:EV:fbk", "BL1607-5-I22:Energy:EV", "BL1607-5-I22:Energy:status", "BL1607-5-I22:Energy:stop", parent, 0.5)
{
	// Initialize member variables.

	hc_ = new AMReadOnlyPVControl(QString("hcConstant"), QString("BL1607-5-I22:Energy:EV:fbk:tr.A"), this);
	crystal2D_ = new AMReadOnlyPVControl(QString("Crystal2DSpacing"), QString("BL1607-5-I22:Energy:EV:fbk:tr.B"), this);
	braggAngle_ = new AMReadOnlyPVControl(QString("BraggAngle"), QString("BL1607-5-I22:Energy:EV:fbk:tr.K"), this);
	angleOffset_ = new AMPVControl(QString("BraggAngleOffset"), QString("BL1607-5-I22:Energy:EV:fbk:tr.C"), QString("BL1607-5-I22:Energy:EV:fbk:tr.C"), QString(), this);

}

BioXASSideMonochromatorEnergyControl::~BioXASSideMonochromatorEnergyControl()
{

}
