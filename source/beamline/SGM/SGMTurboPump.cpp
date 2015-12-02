#include "SGMTurboPump.h"

SGMTurboPump::SGMTurboPump(const QString &deviceName, const QString &baseName, QObject *parent) :
	AMConnectedControl(deviceName, "", parent)
{
	// Create children.

	running_ = new AMSinglePVControl("turboRunning", baseName+":Start", this);
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

	return result;
}

