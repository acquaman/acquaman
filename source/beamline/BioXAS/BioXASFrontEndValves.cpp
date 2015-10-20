#include "BioXASFrontEndValves.h"

BioXASFrontEndValves::BioXASFrontEndValves(QObject *parent) :
	BioXASValvesSetControl("BioXASFrontEndValves", parent)
{
	vvr1_ = new AMReadOnlyPVControl("VVR1 - VacuumValve", "VVR1407-I00-01:state", this);
	vvr2_ = new AMReadOnlyPVControl("VVR2 - FastValve", "VVF1407-I00-01:state", this);
	vvr3_ = new CLSBiStateControl("VVR3", "VVR3", "VVR1607-5-I00-01:state", "VVR1607-5-I00-01:opr:open", "VVR1607-5-I00-01:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvr4_ = new CLSBiStateControl("VVR4", "VVR4", "VVR1607-5-I22-01:state", "VVR1607-5-I22-01:opr:open", "VVR1607-5-I22-01:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvr5_ = new CLSBiStateControl("VVR5", "VVR5", "VVR1607-5-I21-01:state", "VVR1607-5-I21-01:opr:open", "VVR1607-5-I21-01:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvr6_ = new CLSBiStateControl("VVR6", "VVR6", "VVR1607-5-I10-01:state", "VVR1607-5-I10-01:opr:open", "VVR1607-5-I10-01:opr:close", new AMControlStatusCheckerDefault(4), this);

	AMControlSet *frontEndValves = new AMControlSet(this);
	frontEndValves->addControl(vvr1_);
	frontEndValves->addControl(vvr2_);
	frontEndValves->addControl(vvr3_);
	frontEndValves->addControl(vvr4_);
	frontEndValves->addControl(vvr5_);
	frontEndValves->addControl(vvr6_);

	setValveSet(frontEndValves);
}

BioXASFrontEndValves::~BioXASFrontEndValves()
{

}
