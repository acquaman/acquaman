#include "BioXASMasterValves.h"
#include "beamline/BioXAS/BioXASFrontEndValves.h"
#include "beamline/BioXAS/BioXASSideValves.h"
#include "beamline/BioXAS/BioXASMainValves.h"
#include "beamline/BioXAS/BioXASImagingValves.h"

BioXASMasterValves::BioXASMasterValves(QObject *parent) :
	BioXASValves("BioXASMasterValves", parent)
{
	// Initialize class variables.

	frontEndValves_ = 0;
	sideValves_ = 0;
	mainValves_ = 0;
	imagingValves_ = 0;
}

BioXASMasterValves::~BioXASMasterValves()
{

}

void BioXASMasterValves::setFrontEndValves(BioXASFrontEndValves *newValves)
{
	if (frontEndValves_ != newValves) {

		if (frontEndValves_)
			removeValve(frontEndValves_);

		frontEndValves_ = newValves;

		if (frontEndValves_)
			addValve(frontEndValves_, Open, Closed);

		emit frontEndValvesChanged(frontEndValves_);
	}
}

void BioXASMasterValves::setSideValves(BioXASSideValves *newValves)
{
	if (sideValves_ != newValves) {

		if (sideValves_)
			removeValve(sideValves_);

		sideValves_ = newValves;

		if (sideValves_)
			addValve(sideValves_, Open, Closed);

		emit sideValvesChanged(sideValves_);
	}
}

void BioXASMasterValves::setMainValves(BioXASMainValves *newValves)
{
	if (mainValves_ != newValves) {

		if (mainValves_)
			removeValve(mainValves_);

		mainValves_ = newValves;

		if (mainValves_)
			addValve(mainValves_, Open, Closed);

		emit mainValvesChanged(mainValves_);
	}
}

void BioXASMasterValves::setImagingValves(BioXASImagingValves *newValves)
{
	if (imagingValves_ != newValves) {

		if (imagingValves_)
			removeValve(imagingValves_);

		imagingValves_ = newValves;

		if (imagingValves_)
			addValve(imagingValves_, Open, Closed);

		emit imagingValvesChanged(imagingValves_);
	}
}

void BioXASMasterValves::updateConnected()
{
	bool connected = (
				frontEndValves_ && frontEndValves_->isConnected() &&
				sideValves_ && sideValves_->isConnected() &&
				mainValves_ && mainValves_->isConnected() &&
				imagingValves_ && imagingValves_->isConnected()
				);

	setConnected(connected);
}
