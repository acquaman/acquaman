#include "VESPERSEndstationConfiguration.h"

VESPERSEndstationConfiguration::VESPERSEndstationConfiguration(QObject *parent)
	: QObject(parent)
{
	current_ = Invalid;

	xAndZ_ = qMakePair(false, false);
	hAndV_ = qMakePair(false, false);
	bigBeam_ = qMakePair(false, false);
	xas_ = qMakePair(false, false);
	xrf_ = qMakePair(false, false);
	xrd_ = qMakePair(false, false);
	ionChambers_ = qMakePair(false, false);
	vortex1E_ = qMakePair(false, false);
	vortex4E_ = qMakePair(false, false);
	roperCCD_ = qMakePair(false, false);
}

void VESPERSEndstationConfiguration::setGeometry(Geometry newGeometry)
{
	switch(newGeometry){

	case Invalid:
		xAndZ_ = qMakePair(false, false);
		hAndV_ = qMakePair(false, false);
		bigBeam_ = qMakePair(false, false);
		xas_ = qMakePair(false, false);
		xrf_ = qMakePair(false, false);
		xrd_ = qMakePair(false, false);
		ionChambers_ = qMakePair(false, false);
		vortex1E_ = qMakePair(false, false);
		vortex4E_ = qMakePair(false, false);
		roperCCD_ = qMakePair(false, false);
		break;

	case StraightOn:
		xAndZ_ = qMakePair(true, false);
		hAndV_ = qMakePair(false, false);
		bigBeam_ = qMakePair(false, false);
		xas_ = qMakePair(true, false);
		xrf_ = qMakePair(true, false);
		xrd_ = qMakePair(false, false);
		ionChambers_ = qMakePair(true, false);
		vortex1E_ = qMakePair(true, false);
		vortex4E_ = qMakePair(false, false);
		roperCCD_ = qMakePair(false, false);
		break;

	case Single45Vertical:
		xAndZ_ = qMakePair(false, false);
		hAndV_ = qMakePair(true, false);
		bigBeam_ = qMakePair(false, false);
		xas_ = qMakePair(true, false);
		xrf_ = qMakePair(true, false);
		xrd_ = qMakePair(true, false);
		ionChambers_ = qMakePair(true, false);
		vortex1E_ = qMakePair(true, false);
		vortex4E_ = qMakePair(false, false);
		roperCCD_ = qMakePair(true, false);
		break;

	case Single45Horizontal:
		xAndZ_ = qMakePair(true, false);
		hAndV_ = qMakePair(false, false);
		bigBeam_ = qMakePair(false, false);
		xas_ = qMakePair(true, false);
		xrf_ = qMakePair(true, false);
		xrd_ = qMakePair(false, false);
		ionChambers_ = qMakePair(true, false);
		vortex1E_ = qMakePair(false, false);
		vortex4E_ = qMakePair(true, false);
		roperCCD_ = qMakePair(false, false);
		break;

	case Double45:
		xAndZ_ = qMakePair(false, false);
		hAndV_ = qMakePair(true, false);
		bigBeam_ = qMakePair(false, false);
		xas_ = qMakePair(true, false);
		xrf_ = qMakePair(true, false);
		xrd_ = qMakePair(true, false);
		ionChambers_ = qMakePair(true, false);
		vortex1E_ = qMakePair(false, false);
		vortex4E_ = qMakePair(true, false);
		roperCCD_ = qMakePair(true, false);
		break;

	case BigBeam:
		xAndZ_ = qMakePair(false, false);
		hAndV_ = qMakePair(false, false);
		bigBeam_ = qMakePair(true, false);
		xas_ = qMakePair(true, false);
		xrf_ = qMakePair(true, false);
		xrd_ = qMakePair(false, false);
		ionChambers_ = qMakePair(true, false);
		vortex1E_ = qMakePair(false, false);
		vortex4E_ = qMakePair(true, false);
		roperCCD_ = qMakePair(false, false);
		break;
	}

	emit geometryChanged();
}

void VESPERSEndstationConfiguration::setUsingXAndZ(bool use)
{
	bool newState = use && xAndZ_.first;

	if (newState != xAndZ_.second){

		xAndZ_.second = newState;
		emit usingXAndZChanged(newState);
	}
}

void VESPERSEndstationConfiguration::setUsingHAndV(bool use)
{
	bool newState = use && hAndV_.first;

	if (newState != hAndV_.second){

		hAndV_.second = newState;
		emit usingHAndVChanged(newState);
	}
}

void VESPERSEndstationConfiguration::setUsingBigBeam(bool use)
{
	bool newState = use && bigBeam_.first;

	if (newState != bigBeam_.second){

		bigBeam_.second = newState;
		emit usingBigBeamChanged(newState);
	}
}

void VESPERSEndstationConfiguration::setUsingXAS(bool use)
{
	bool newState = use && xas_.first;

	if (newState != xas_.second){

		xas_.second = newState;
		emit usingXASChanged(newState);
	}
}

void VESPERSEndstationConfiguration::setUsingXRF(bool use)
{
	bool newState = use && xrf_.first;

	if (newState != xrf_.second){

		xrf_.second = newState;
		emit usingXRFChanged(newState);
	}
}

void VESPERSEndstationConfiguration::setUsingXRD(bool use)
{
	bool newState = use && xrd_.first;

	if (newState != xrd_.second){

		xrd_.second = newState;
		emit usingXRDChanged(newState);
	}
}

void VESPERSEndstationConfiguration::setUsingIonChambers(bool use)
{
	bool newState = use && ionChambers_.first;

	if (newState != ionChambers_.second){

		ionChambers_.second = newState;
		emit usingIonChambersChanged(newState);
	}
}

void VESPERSEndstationConfiguration::setUsingSingleElementVortex(bool use)
{
	bool newState = use && vortex1E_.first;

	if (newState != vortex1E_.second){

		vortex1E_.second = newState;
		emit usingSingleElementVortexChanged(newState);
	}
}

void VESPERSEndstationConfiguration::setUsingFourElementVortex(bool use)
{
	bool newState = use && vortex4E_.first;

	if (newState != vortex4E_.second){

		vortex4E_.second = newState;
		emit usingFourElementVortexChanged(newState);
	}
}

void VESPERSEndstationConfiguration::setUsingRoperCCD(bool use)
{
	bool newState = use && roperCCD_.first;

	if (newState != roperCCD_.second){

		roperCCD_.second = newState;
		emit usingRoperCCDChanged(newState);
	}
}
