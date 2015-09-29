#include "BioXASSideShutters.h"

BioXASSideShutters::BioXASSideShutters(QObject *parent) :
    BioXASShutters(parent)
{
	// Set up endstation shutters.

	safetyShutterSide_ = new CLSBiStateControl("SideSafetyShutter", "SideSafetyShutter", "SSH1607-5-I22-01:state", "SSH1607-5-I22-01:opr:open", "SSH1607-5-I22-01:opr:close", new AMControlStatusCheckerDefault(2), this);
	connect( safetyShutterSide_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	safetyShutterMain_ = new AMReadOnlyPVControl("MainSafetyShutter", "SSH1607-5-I21-01:state", this);
	connect( safetyShutterMain_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	safetyShutterImaging_ = new AMReadOnlyPVControl("ImagingSafetyShutter", "SSH1607-5-I10-01:state", this);
	connect( safetyShutterImaging_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// Set up shutters control.
}

BioXASSideShutters::~BioXASSideShutters()
{

}

bool BioXASSideShutters::isConnected() const
{
	bool connected = (
				BioXASShutters::isConnected() &&
				safetyShutterSide_ && safetyShutterSide_->isConnected() &&
				safetyShutterMain_ && safetyShutterMain_->isConnected() &&
				safetyShutterImaging_ && safetyShutterImaging_->isConnected()
				);

	return connected;
}
