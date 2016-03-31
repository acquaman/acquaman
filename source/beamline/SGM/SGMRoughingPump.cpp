#include "SGMRoughingPump.h"
#include "beamline/SGM/SGMRoughingPumpRunningStatus.h"

SGMRoughingPump::SGMRoughingPump(const QString &name, const QString &baseName, QObject *parent) :
	AMConnectedControl(name, "", parent)
{
	// Setup child controls.

	running_ = new SGMRoughingPumpRunningStatus(baseName, baseName, this);
	addChildControl(running_);
}

SGMRoughingPump::~SGMRoughingPump()
{

}

