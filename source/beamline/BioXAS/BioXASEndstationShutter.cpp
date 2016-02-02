#include "BioXASEndstationShutter.h"
#include "beamline/AMPVControl.h"
#include "beamline/CLS/CLSBiStateControl.h"

BioXASEndstationShutter::BioXASEndstationShutter(const QString &name, const QString &baseName, QObject *parent) :
	AMExclusiveStatesEnumeratedControl(name, "", parent)
{
	setStatusControl(new AMReadOnlyPVControl(name+"Status", baseName+":state", this));
	addState(Closed, "Closed", CLSBiStateControl::Closed, new AMSinglePVControl(name+"OprClose", baseName+":opr:close", this), 1);
	addState(Open, "Open", CLSBiStateControl::Open, new AMSinglePVControl(name+"OprOpen", baseName+":opr:open", this), 1);
}

BioXASEndstationShutter::~BioXASEndstationShutter()
{

}
