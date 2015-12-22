#include "BioXASSideValves.h"
#include "beamline/AMPVControl.h"
#include "beamline/CLS/CLSBiStateControl.h"

BioXASSideValves::BioXASSideValves(QObject *parent) :
	BioXASValves("BioXASSideValves", parent)
{
	addValve(new CLSBiStateControl("VVR1607-5-I22-02", "VVR1607-5-I22-02", "VVR1607-5-I22-02:state", "VVR1607-5-I22-02:opr:open", "VVR1607-5-I22-02:opr:close", new AMControlStatusCheckerDefault(2), this), CLSBiStateControl::Open, CLSBiStateControl::Closed);
	addValve(new CLSBiStateControl("VVR1607-5-I22-03", "VVR1607-5-I22-03", "VVR1607-5-I22-03:state", "VVR1607-5-I22-03:opr:open", "VVR1607-5-I22-03:opr:close", new AMControlStatusCheckerDefault(2), this), CLSBiStateControl::Open, CLSBiStateControl::Closed);
	addValve(new CLSBiStateControl("VVR1607-5-I22-04", "VVR1607-5-I22-04", "VVR1607-5-I22-04:state", "VVR1607-5-I22-04:opr:open", "VVR1607-5-I22-04:opr:close", new AMControlStatusCheckerDefault(2), this), CLSBiStateControl::Open, CLSBiStateControl::Closed);
	addValve(new CLSBiStateControl("VVR1607-5-I22-06", "VVR1607-5-I22-06", "VVR1607-5-I22-06:state", "VVR1607-5-I22-06:opr:open", "VVR1607-5-I22-06:opr:close", new AMControlStatusCheckerDefault(2), this), CLSBiStateControl::Open, CLSBiStateControl::Closed);
	addValve(new CLSBiStateControl("VVR1607-5-I22-07", "VVR1607-5-I22-07", "VVR1607-5-I22-07:state", "VVR1607-5-I22-07:opr:open", "VVR1607-5-I22-07:opr:close", new AMControlStatusCheckerDefault(2), this), CLSBiStateControl::Open, CLSBiStateControl::Closed);
}

BioXASSideValves::~BioXASSideValves()
{

}
