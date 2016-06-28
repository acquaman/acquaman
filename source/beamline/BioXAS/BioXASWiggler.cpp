#include "BioXASWiggler.h"

BioXASWiggler::BioXASWiggler(const QString &name, QObject *parent) :
	AMConnectedControl(name, "", parent)
{
	gap_ = new AMPVControl("WIG1407-01:gap:mm", "WIG1407-01:gap:mm:fbk", "WIG1407-01:gap:mm", "", this);
	addChildControl(gap_);

	gapStatus_ = new BioXASWigglerGapStatus("WIG1407-01:gap:mm", "WIG1407-01:gap:mm", this);
	addChildControl(gapStatus_);
}

BioXASWiggler::~BioXASWiggler()
{

}

