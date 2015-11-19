#include "SGMTurboPump.h"

SGMTurboPump::SGMTurboPump(const QString &deviceName, const QString &baseName, QObject *parent) :
	AMControl(deviceName, "", parent)
{
	// Initialize class variables.

	connected_ = false;

	// Create children.

	running_ = new AMSinglePVControl(name()+"Running", baseName+":start", this);

	// Current settings.

	updateConnected();
}

SGMTurboPump::~SGMTurboPump()
{

}

bool SGMTurboPump::isConnected() const
{
	bool connected = (
				running_->isConnected()
			);

	return connected;
}

void SGMTurboPump::setConnected(bool isConnected)
{
	if (connected_ != isConnected) {
		connected_ = isConnected;
		emit connected(connected_);
	}
}

void SGMTurboPump::updateConnected()
{
	setConnected( isConnected() );
}


