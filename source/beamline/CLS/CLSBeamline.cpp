#include "CLSBeamline.h"

#include "beamline/CLS/CLSStorageRing.h"

CLSBeamline::CLSBeamline(const QString &controlName) :
	AMBeamline(controlName)
{
	wasBeamAvailable_ = false;
}

CLSBeamline::~CLSBeamline()
{

}

/**
 * ================= implementation of public interface / slots =======================
 **/

void CLSBeamline::initializeBeamline()
{
	AMBeamline::initializeBeamline();

	connect(CLSStorageRing::sr1(), SIGNAL(beamAvaliabilityChanged(bool)), this, SLOT(updateBeamAvailabilityStatus()));

	updateBeamAvailabilityStatus();
}

/**
 * ================= implementation of protected slots / functions =======================
 **/
void CLSBeamline::updateBeamAvailabilityStatus()
{
	bool beamAvailable = CLSStorageRing::sr1()->beamAvailable() && isBeamlineBeamAvailable();

	if (wasBeamAvailable_ != beamAvailable) {
		wasBeamAvailable_ = beamAvailable;
		emit beamAvaliabilityChanged(wasBeamAvailable_);
	}
}
