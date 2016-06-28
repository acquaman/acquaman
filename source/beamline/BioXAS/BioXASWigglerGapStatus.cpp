#include "BioXASWigglerGapStatus.h"
#include "beamline/AMPVControl.h"

BioXASWigglerGapStatus::BioXASWigglerGapStatus(const QString &name, const QString &baseName, QObject *parent) :
	AMSingleEnumeratedControl(name, "", parent)
{
	setBaseControl(new AMReadOnlyPVControl(baseName, baseName, this));

	addValueOption(Closed, "Closed", BIOXASWIGGLERGAPSTATUS_GAP_CLOSED_MIN, BIOXASWIGGLERGAPSTATUS_GAP_CLOSED_MAX, true);
	addValueOption(Open, "Open", BIOXASWIGGLERGAPSTATUS_GAP_OPEN_MIN, BIOXASWIGGLERGAPSTATUS_GAP_OPEN_MAX, true);
}

BioXASWigglerGapStatus::~BioXASWigglerGapStatus()
{

}

