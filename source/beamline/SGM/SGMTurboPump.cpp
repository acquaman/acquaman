#include "SGMTurboPump.h"

SGMTurboPump::SGMTurboPump(const QString &deviceName, const QString &baseName, QObject *parent) :
	SGMControl(deviceName, "", parent)
{
	// Create children.

	running_ = new AMSinglePVControl(name()+"Running", baseName+":start", this);
	addChildControl(running_);
}

SGMTurboPump::~SGMTurboPump()
{

}

