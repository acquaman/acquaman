#include "BioXASMainValves.h"
#include "beamline/AMPVControl.h"
#include "beamline/CLS/CLSBiStateControl.h"

BioXASMainValves::BioXASMainValves(QObject *parent) :
	BioXASValves("BioXASMainValves", parent)
{
	addValve(new CLSBiStateControl("VVR1607-5-I21-02", "VVR1607-5-I21-02", "VVR1607-5-I21-02:state", "VVR1607-5-I21-02:opr:open", "VVR1607-5-I21-02:opr:close", new AMControlStatusCheckerDefault(2), this), CLSBiStateControl::Open, CLSBiStateControl::Closed);
	addValve(new CLSBiStateControl("VVR1607-5-I21-03", "VVR1607-5-I21-03", "VVR1607-5-I21-03:state", "VVR1607-5-I21-03:opr:open", "VVR1607-5-I21-03:opr:close", new AMControlStatusCheckerDefault(2), this), CLSBiStateControl::Open, CLSBiStateControl::Closed);
	addValve(new CLSBiStateControl("VVR1607-5-I21-04", "VVR1607-5-I21-04", "VVR1607-5-I21-04:state", "VVR1607-5-I21-04:opr:open", "VVR1607-5-I21-04:opr:close", new AMControlStatusCheckerDefault(2), this), CLSBiStateControl::Open, CLSBiStateControl::Closed);
	addValve(new CLSBiStateControl("VVR1607-5-I21-06", "VVR1607-5-I21-06", "VVR1607-5-I21-06:state", "VVR1607-5-I21-06:opr:open", "VVR1607-5-I21-06:opr:close", new AMControlStatusCheckerDefault(2), this), CLSBiStateControl::Open, CLSBiStateControl::Closed);
	addValve(new CLSBiStateControl("VVR1607-5-I21-07", "VVR1607-5-I21-07", "VVR1607-5-I21-07:state", "VVR1607-5-I21-07:opr:open", "VVR1607-5-I21-07:opr:close", new AMControlStatusCheckerDefault(2), this), CLSBiStateControl::Open, CLSBiStateControl::Closed);
}

BioXASMainValves::~BioXASMainValves()
{

}
