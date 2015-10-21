#include "BioXASMainValves.h"

BioXASMainValves::BioXASMainValves(QObject *parent) :
	BioXASValvesSetControl("BioXASMainValves", parent)
{
	vvrMain1_ = new CLSBiStateControl("MainVVR1", "MainVVR1", "VVR1607-5-I21-02:state", "VVR1607-5-I21-02:opr:open", "VVR1607-5-I21-02:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrMain2_ = new CLSBiStateControl("MainVVR2", "MainVVR2", "VVR1607-5-I21-03:state", "VVR1607-5-I21-03:opr:open", "VVR1607-5-I21-03:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrMain3_ = new CLSBiStateControl("MainVVR3", "MainVVR3", "VVR1607-5-I21-04:state", "VVR1607-5-I21-04:opr:open", "VVR1607-5-I21-04:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrMain4_ = new CLSBiStateControl("MainVVR4", "MainVVR4", "VVR1607-5-I21-06:state", "VVR1607-5-I21-06:opr:open", "VVR1607-5-I21-06:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrMain5_ = new CLSBiStateControl("MainVVR5", "MainVVR5", "VVR1607-5-I21-07:state", "VVR1607-5-I21-07:opr:open", "VVR1607-5-I21-07:opr:close", new AMControlStatusCheckerDefault(4), this);

	AMControlSet *mainValves = new AMControlSet(this);
	mainValves->addControl(vvrMain1_);
	mainValves->addControl(vvrMain2_);
	mainValves->addControl(vvrMain3_);
	mainValves->addControl(vvrMain4_);
	mainValves->addControl(vvrMain5_);

	setValveSet(mainValves);
}

BioXASMainValves::~BioXASMainValves()
{

}
