#include "BioXASBeamStatus.h"

BioXASBeamStatus::BioXASBeamStatus(const QString &name, QObject *parent) :
	AMEnumeratedControl(name, "", parent)
{
	// Setup the basic value options.

	addOption(On, "On");
	addOption(Off, "Off");
}

BioXASBeamStatus::~BioXASBeamStatus()
{

}

bool BioXASBeamStatus::canMeasure() const
{
	return false;
}

bool BioXASBeamStatus::canMove() const
{
	return false;
}

bool BioXASBeamStatus::canStop() const
{
	return false;
}

void BioXASBeamStatus::setFrontEndValves(BioXASFrontEndValves *newValves)
{
	if (frontEndValves_ != newValves) {

		if (frontEndValves_)
			removeChildControl(frontEndValves_);

		frontEndValves_ = newValves;

		if (frontEndValves_)
			addChildControl(frontEndValves_);

		emit frontEndValvesChanged(frontEndValves_);
	}
}

void BioXASBeamStatus::setSideValves(BioXASSideValves *newValves)
{
	if (sideValves_ != newValves) {

		if (sideValves_)
			removeChildControl(sideValves_);

		sideValves_ = newValves;

		if (sideValves_)
			addChildControl(sideValves_);

		emit sideValvesChanged(sideValves_);
	}
}

void BioXASBeamStatus::setMainValves(BioXASMainValves *newValves)
{
	if (mainValves_ != newValves) {

		if (mainValves_)
			removeChildControl(mainValves_);

		mainValves_ = newValves;

		if (mainValves_)
			addChildControl(mainValves_);

		emit mainValvesChanged(mainValves_);
	}
}

void BioXASBeamStatus::setImagingValves(BioXASImagingValves *newValves)
{
	if (imagingValves_ != newValves) {

		if (imagingValves_)
			removeChildControl(imagingValves_);

		imagingValves_ = newValves;

		if (imagingValves_)
			addChildControl(imagingValves_);

		emit imagingValvesChanged(imagingValves_);
	}
}

int BioXASBeamStatus::currentIndex() const
{
	int result = enumNames().indexOf("Unknown");

	if (isOn())
		result = On;
	else if (isOff())
		result = Off;

	return result;
}
