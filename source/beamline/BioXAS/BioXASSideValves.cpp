#include "BioXASSideValves.h"

BioXASSideValves::BioXASSideValves(QObject *parent) :
	BioXASValvesSetControl("BioXASSideValves", parent)
{
	vvrSide1_ = new CLSBiStateControl("VVR1607-5-I22-02", "VVR1607-5-I22-02", "VVR1607-5-I22-02:state", "VVR1607-5-I22-02:opr:open", "VVR1607-5-I22-02:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrSide2_ = new CLSBiStateControl("VVR1607-5-I22-03", "VVR1607-5-I22-03", "VVR1607-5-I22-03:state", "VVR1607-5-I22-03:opr:open", "VVR1607-5-I22-03:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrSide3_ = new CLSBiStateControl("VVR1607-5-I22-04", "VVR1607-5-I22-04", "VVR1607-5-I22-04:state", "VVR1607-5-I22-04:opr:open", "VVR1607-5-I22-04:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrSide4_ = new CLSBiStateControl("VVR1607-5-I22-06", "VVR1607-5-I22-06", "VVR1607-5-I22-06:state", "VVR1607-5-I22-06:opr:open", "VVR1607-5-I22-06:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrSide5_ = new CLSBiStateControl("VVR1607-5-I22-07", "VVR1607-5-I22-07", "VVR1607-5-I22-07:state", "VVR1607-5-I22-07:opr:open", "VVR1607-5-I22-07:opr:close", new AMControlStatusCheckerDefault(4), this);

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
