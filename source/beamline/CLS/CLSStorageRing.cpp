#include "CLSStorageRing.h"

#include "beamline/AMPVControl.h"

CLSStorageRing::CLSStorageRing(QObject *parent)
	: AMStorageRing(parent)
{
	ringCurrent_ = new AMReadOnlyPVControl("Ring Current", "PCT1402-01:mA:fbk", this);
	srStatus_ = new AMReadOnlyPVControl("SR Status", "SRStatus:shutters", this);

	connect(ringCurrent_, SIGNAL(valueChanged(double)), this, SIGNAL(ringCurrentChanged(double)));
	connect(ringCurrent_, SIGNAL(valueChanged(double)), this, SLOT(determineBeamAvailability()));
	connect(srStatus_, SIGNAL(valueChanged(double)), this, SLOT(determineBeamAvailability()));
}

CLSStorageRing::~CLSStorageRing()
{

}

CLSStorageRing *CLSStorageRing::sr1()
{
	if (!instance_)
		instance_ = new CLSStorageRing;

	return static_cast<CLSStorageRing *>(instance_);
}

void CLSStorageRing::determineBeamAvailability()
{
	bool beamDump = (ringCurrent_->value() < 1.0 || int(srStatus_->value()) == 0);

	if (beamAvailable_ == beamDump){

		beamAvailable_ = !beamDump;
		emit beamAvaliability(beamAvailable_);

		if (beamAvailable_)
			emit beamRestored();

		else
			emit beamDumped();
	}
}
