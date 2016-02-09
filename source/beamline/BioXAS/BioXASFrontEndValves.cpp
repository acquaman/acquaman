#include "BioXASFrontEndValves.h"
#include "beamline/AMPVControl.h"
#include "beamline/CLS/CLSExclusiveStatesControl.h"

BioXASFrontEndValves::BioXASFrontEndValves(QObject *parent) :
	BioXASValves("BioXASFrontEndValves", parent)
{
	addValve(new AMReadOnlyPVControl("VVR1407-I00-01", "VVR1407-I00-01:state", this), BIOXASFRONTENDVALVES_VALVE_OPEN, BIOXASFRONTENDVALVES_VALVE_CLOSED);
	addValve(new AMReadOnlyPVControl("VVF1407-I00-01", "VVF1407-I00-01:state", this), BIOXASFRONTENDVALVES_FASTVALVE_OPEN, BIOXASFRONTENDVALVES_FASTVALVE_CLOSED);
	addValve(new CLSExclusiveStatesControl("VVR1607-5-I00-01", "VVR1607-5-I00-01:state", "VVR1607-5-I00-01:opr:open", "VVR1607-5-I00-01:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addValve(new CLSExclusiveStatesControl("VVR1607-5-I22-01", "VVR1607-5-I22-01:state", "VVR1607-5-I22-01:opr:open", "VVR1607-5-I22-01:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addValve(new CLSExclusiveStatesControl("VVR1607-5-I21-01", "VVR1607-5-I21-01:state", "VVR1607-5-I21-01:opr:open", "VVR1607-5-I21-01:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addValve(new CLSExclusiveStatesControl("VVR1607-5-I10-01", "VVR1607-5-I10-01:state", "VVR1607-5-I10-01:opr:open", "VVR1607-5-I10-01:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
}

BioXASFrontEndValves::~BioXASFrontEndValves()
{

}
