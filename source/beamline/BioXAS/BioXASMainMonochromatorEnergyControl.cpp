#include "BioXASMainMonochromatorEnergyControl.h"

BioXASMainMonochromatorEnergyControl::BioXASMainMonochromatorEnergyControl(QObject *parent) :
	BioXASSSRLMonochromatorEnergyControl("EnergyEV", "BL1607-5-I21:Energy:EV:fbk", "BL1607-5-I21:Energy:EV", "BL1607-5-I21:Energy:status", QString("BL1607-5-I21:Energy:stop"), parent)
{
	// Initialize member variables.

	hc_ = new AMReadOnlyPVControl(QString("hcConstant"), QString("BL1607-5-I21:Energy:EV:fbk:tr.A"), this);
	crystal2D_ = new AMReadOnlyPVControl(QString("Crystal2DSpacing"), QString("BL1607-5-I21:Energy:EV:fbk:tr.B"), this);
	braggAngle_ = new AMReadOnlyPVControl(QString("BraggAngle"), QString("BL1607-5-I21:Energy:EV:fbk:tr.K"), this);
	angleOffset_ = new AMReadOnlyPVControl(QString("EnergyOffset"), QString("BL1607-5-I21:Energy:EV:fbk:tr.C"), this);
}

BioXASMainMonochromatorEnergyControl::~BioXASMainMonochromatorEnergyControl()
{

}

