#include "SGMVATValve.h"

SGMVATValve::SGMVATValve(const QString &name, QObject *parent) :
	AMControl(name, "", parent)
{
	// Initialize class variables.

	connected_ = false;

	position_ = new AMSinglePVControl("vatValvePosition", "VVR1611-4-I10-11:ctrl:posn", this);
	connect( position_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	speed_ = new AMSinglePVControl("vatValveSpeed", "VVR1611-4-I10-11:valveSpeed", this);
	connect( speed_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
}

SGMVATValve::~SGMVATValve()
{

}

bool SGMVATValve::isConnected() const
{
	bool connected = (
				position_->isConnected() &&
				speed_->isConnected()
			);

	return connected;
}

void SGMVATValve::setConnected(bool isConnected)
{
	if (connected_ != isConnected) {
		connected_ = isConnected;
		emit connected(connected_);
	}
}

void SGMVATValve::updateConnected()
{
	setConnected( isConnected() );
}
