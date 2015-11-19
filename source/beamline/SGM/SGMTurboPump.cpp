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

bool SGMTurboPump::isRunning() const
{
	bool result = false;

	if (running_ && running_->isConnected())
		result = (running_->value() == Running);
}

