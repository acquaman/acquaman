#include "BioXASImagingValves.h"
#include "beamline/AMPVControl.h"
#include "beamline/CLS/CLSBiStateControl.h"

BioXASImagingValves::BioXASImagingValves(QObject *parent) :
	BioXASValves("BioXASImagingValves", parent)
{
	addValve(new CLSBiStateControl("VVR1607-5-I10-02", "VVR1607-5-I10-02", "VVR1607-5-I10-02:state", "VVR1607-5-I10-02:opr:open", "VVR1607-5-I10-02:opr:close", new AMControlStatusCheckerDefault(2), this), CLSBiStateControl::Open, CLSBiStateControl::Closed);
	addValve(new CLSBiStateControl("VVR1607-5-I10-03", "VVR1607-5-I10-03", "VVR1607-5-I10-03:state", "VVR1607-5-I10-03:opr:open", "VVR1607-5-I10-03:opr:close", new AMControlStatusCheckerDefault(2), this), CLSBiStateControl::Open, CLSBiStateControl::Closed);
	addValve(new CLSBiStateControl("VVR1607-5-I10-04", "VVR1607-5-I10-04", "VVR1607-5-I10-04:state", "VVR1607-5-I10-04:opr:open", "VVR1607-5-I10-04:opr:close", new AMControlStatusCheckerDefault(2), this), CLSBiStateControl::Open, CLSBiStateControl::Closed);
	addValve(new CLSBiStateControl("VVR1607-5-I10-05", "VVR1607-5-I10-05", "VVR1607-5-I10-05:state", "VVR1607-5-I10-05:opr:open", "VVR1607-5-I10-05:opr:close", new AMControlStatusCheckerDefault(2), this), CLSBiStateControl::Open, CLSBiStateControl::Closed);
	addValve(new CLSBiStateControl("VVR1607-5-I10-07", "VVR1607-5-I10-07", "VVR1607-5-I10-07:state", "VVR1607-5-I10-07:opr:open", "VVR1607-5-I10-07:opr:close", new AMControlStatusCheckerDefault(2), this), CLSBiStateControl::Open, CLSBiStateControl::Closed);
	addValve(new CLSBiStateControl("VVR1607-5-I10-08", "VVR1607-5-I10-08", "VVR1607-5-I10-08:state", "VVR1607-5-I10-08:opr:open", "VVR1607-5-I10-08:opr:close", new AMControlStatusCheckerDefault(2), this), CLSBiStateControl::Open, CLSBiStateControl::Closed);
	addValve(new CLSBiStateControl("VVR1607-5-I10-10", "VVR1607-5-I10-10", "VVR1607-5-I10-10:state", "VVR1607-5-I10-10:opr:open", "VVR1607-5-I10-10:opr:close", new AMControlStatusCheckerDefault(2), this), CLSBiStateControl::Open, CLSBiStateControl::Closed);
}

BioXASImagingValves::~BioXASImagingValves()
{

}
