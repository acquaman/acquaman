#include "BioXASSideShutters.h"
#include "beamline/BioXAS/BioXASFrontEndShutters.h"
#include "beamline/CLS/CLSBiStateControl.h"

BioXASSideShutters::BioXASSideShutters(const QString &name, QObject *parent) :
	BioXASShutters(name, parent)
{
	// Initialize class variables.

	frontEndShutters_ = 0;
	endstationShutter_ = 0;
}

BioXASSideShutters::~BioXASSideShutters()
{

}

void BioXASSideShutters::setFrontEndShutters(BioXASFrontEndShutters *newShutters)
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

void BioXASSideShutters::setEndstationShutter(CLSBiStateControl *newShutter)
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
