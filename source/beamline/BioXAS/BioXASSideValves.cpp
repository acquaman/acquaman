#include "BioXASSideValves.h"

BioXASSideValves::BioXASSideValves(QObject *parent) :
	BioXASValvesSetControl("BioXASSideValves", parent)
{
	vvrSide1_ = new CLSBiStateControl("SideVVR1", "SideVVR1", "VVR1607-5-I22-02:state", "VVR1607-5-I22-02:opr:open", "VVR1607-5-I22-02:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrSide2_ = new CLSBiStateControl("SideVVR2", "SideVVR2", "VVR1607-5-I22-03:state", "VVR1607-5-I22-03:opr:open", "VVR1607-5-I22-03:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrSide3_ = new CLSBiStateControl("SideVVR3", "SideVVR3", "VVR1607-5-I22-04:state", "VVR1607-5-I22-04:opr:open", "VVR1607-5-I22-04:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrSide4_ = new CLSBiStateControl("SideVVR4", "SideVVR4", "VVR1607-5-I22-06:state", "VVR1607-5-I22-06:opr:open", "VVR1607-5-I22-06:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrSide5_ = new CLSBiStateControl("SideVVR5", "SideVVR5", "VVR1607-5-I22-07:state", "VVR1607-5-I22-07:opr:open", "VVR1607-5-I22-07:opr:close", new AMControlStatusCheckerDefault(4), this);

	AMControlSet *sideValves = new AMControlSet(this);
	sideValves->addControl(vvrSide1_);
	sideValves->addControl(vvrSide2_);
	sideValves->addControl(vvrSide3_);
	sideValves->addControl(vvrSide4_);
	sideValves->addControl(vvrSide5_);

	setValveSet(sideValves);
}

BioXASSideValves::~BioXASSideValves()
{

}
