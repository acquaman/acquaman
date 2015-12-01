#include "SGMRoughingPumpRunningStatus.h"
#include "beamline/AMPVControl.h"

SGMRoughingPumpRunningStatus::SGMRoughingPumpRunningStatus(const QString &name, const QString &baseName, QObject *parent) :
	AMExclusiveStatesEnumeratedControl(name, "", parent)
{
	// Setup the running state options.

	setStatus(new AMReadOnlyPVControl(baseName+":ctl", baseName+":ctl", this));
	addRunningState(NotRunning, "Not Running", new AMSinglePVControl(baseName+":opr:Stop", baseName+":opr:Stop", this));
	addRunningState(Running, "Running", new AMSinglePVControl(baseName+":opr:Start", baseName+":opr:Start", this));
}

SGMRoughingPumpRunningStatus::~SGMRoughingPumpRunningStatus()
{

}

void SGMRoughingPumpRunningStatus::addRunningState(int index, const QString &stateName, AMControl *control)
{
	addState(index, stateName, index, control, 1);
}

