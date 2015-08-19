#include "BioXASDBHRMirrors.h"

BioXASDBHRMirrors::BioXASDBHRMirrors(const QString &name, QObject *parent) :
	BioXASBeamlineComponent(name, parent)
{
	// Initialize class variables.

	m1Vertical_ = 0;
	m2Vertical_ = 0;
	pitch_ = 0;
}

BioXASDBHRMirrors::~BioXASDBHRMirrors()
{

}

bool BioXASDBHRMirrors::isConnected() const
{
	bool connected = (
				m1Vertical_ && m1Vertical_->isConnected() &&
				m2Vertical_ && m2Vertical_->isConnected() &&
				pitch_ && pitch_->isConnected()
				);

	return connected;
}
