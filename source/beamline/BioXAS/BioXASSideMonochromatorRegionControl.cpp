#include "BioXASSideMonochromatorRegionControl.h"
#include "beamline/AMPVControl.h"

BioXASSideMonochromatorRegionControl::BioXASSideMonochromatorRegionControl(QObject *parent) :
	BioXASSSRLMonochromatorRegionControl(parent)
{
	setUpperSlitControl(new AMPVControl("UpperSlitBlade", "SMTR1607-5-I22-09:mm:fbk", "SMTR1607-5-I22-09:mm", QString(), this));
	setLowerSlitControl(new AMPVControl("LowerSlitBlade", "SMTR1607-5-I22-10:mm:fbk", "SMTR1607-5-I22-10:mm", QString(), this));
	setSlitsStatusControl(new AMReadOnlyPVControl(QString("SlitsStatus"), QString("BL1607-5-I22:Mono:SlitsClosed"), this));
	setPaddleControl(new AMPVControl("Paddle", "SMTR1607-5-I22-11:mm:fbk", "SMTR1607-5-I22-11:mm", QString(), this));
	setPaddleStatusControl(new AMReadOnlyPVControl(QString("PaddleStatus"), QString("BL1607-5-I22:Mono:PaddleExtracted"), this));
	setKeyStatusControl(new AMReadOnlyPVControl(QString("KeyStatus"), QString("BL1607-5-I22:Mono:KeyStatus"), this));
	setBrakeStatusControl(new AMReadOnlyPVControl(QString("BrakeStatus"), QString("BL1607-5-I22:Mono:BrakeOff"), this));
	setBraggControl(new AMPVControl("Bragg", "SMTR1607-5-I22-12:deg:fbk", "SMTR1607-5-I22-12:deg", QString(), this));
	setBraggAtCrystalChangePositionStatusControl(new AMReadOnlyPVControl("AtCrystalChangePosition", "BL1607-5-I22:Mono:XtalChangePos", this));
	setCrystalChangeControl(new AMPVControl("CrystalChange", "SMTR1607-5-I22-22:mm:fbk", "SMTR1607-5-I22-22:mm", QString(), this));
	setCrystalChangeCWLimitStatusControl(new AMReadOnlyPVControl("CrystalChangeCWStatus", "SMTR1607-5-I22-22:cw", this));
	setCrystalChangeCCWLimitStatusControl(new AMReadOnlyPVControl("CrystalChangeCCWStatus", "SMTR1607-5-I22-22:ccw", this));
	setRegionAStatusControl(new AMReadOnlyPVControl("RegionAStatus", "BL1607-5-I22:Mono:Region:A", this));
	setRegionBStatusControl(new AMReadOnlyPVControl("RegionBStatus", "BL1607-5-I22:Mono:Region:B", this));
}

BioXASSideMonochromatorRegionControl::~BioXASSideMonochromatorRegionControl()
{

}
