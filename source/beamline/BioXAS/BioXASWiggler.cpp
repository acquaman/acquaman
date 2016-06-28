#include "BioXASWiggler.h"

BioXASWiggler::BioXASWiggler(const QString &name, QObject *parent) :
	AMConnectedControl(name, "", parent)
{
	gap_ = new AMPVControl(QString("%1Gap").arg(name), "WIG1407-01:gap:mm:fbk", "WIG1407-01:gap:mm", "WIG1407-01:gap:stop", this);
	addChildControl(gap_);

	gapStatus_ = new BioXASWigglerGapStatus(QString("%1GapStatus").arg(name), "WIG1407-01:gap:mm:fbk", this);
	addChildControl(gapStatus_);
}

BioXASWiggler::~BioXASWiggler()
{

}

