#include "BioXASMainShutters.h"
#include "beamline/BioXAS/BioXASFrontEndShutters.h"
#include "beamline/CLS/CLSBiStateControl.h"

BioXASMainShutters::BioXASMainShutters(const QString &name, QObject *parent) :
	BioXASShutters(name, parent)
{
	// Initialize class variables.

	frontEndShutters_ = 0;
	endstationShutter_ = 0;
}

BioXASMainShutters::~BioXASMainShutters()
{

}

void BioXASMainShutters::setFrontEndShutters(BioXASFrontEndShutters *newShutters)
{
	if (frontEndShutters_ != newShutters) {

		if (frontEndShutters_)
			removeShutter(frontEndShutters_);

		frontEndShutters_ = newShutters;

		if (frontEndShutters_)
			addShutter(frontEndShutters_, BioXASFrontEndShutters::Open, BioXASFrontEndShutters::Closed);

		emit frontEndShuttersChanged(frontEndShutters_);
	}
}

void BioXASMainShutters::setEndstationShutter(CLSBiStateControl *newShutter)
{
	if (endstationShutter_ != newShutter) {

		if (endstationShutter_)
			removeShutter(endstationShutter_);

		endstationShutter_ = newShutter;

		if (endstationShutter_)
			addShutter(endstationShutter_, CLSBiStateControl::Open, CLSBiStateControl::Closed);

		emit endstationShutterChanged(endstationShutter_);
	}
}
