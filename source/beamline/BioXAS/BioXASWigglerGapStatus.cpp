#include "BioXASWigglerGapStatus.h"
#include "beamline/AMPVControl.h"

BioXASWigglerGapStatus::BioXASWigglerGapStatus(const QString &name, const QString &baseName, QObject *parent) :
	AMSingleEnumeratedControl(name, "", parent)
{
	gap_ = 0;

	addValueOption(Closed, "Closed", BIOXASWIGGLERGAPSTATUS_GAP_CLOSED_MIN, BIOXASWIGGLERGAPSTATUS_GAP_CLOSED_MAX);
	addValueOption(Open, "Open", BIOXASWIGGLERGAPSTATUS_GAP_OPEN_MIN, BIOXASWIGGLERGAPSTATUS_GAP_OPEN_MAX);

	// Current settings.

	setGap(new AMReadOnlyPVControl(baseName, baseName, this));
}

BioXASWigglerGapStatus::~BioXASWigglerGapStatus()
{

}

bool BioXASWigglerGapStatus::isConnected() const
{
	bool connected = (
				AMSingleEnumeratedControl::isConnected() &&
				gap_ && gap_->isConnected()
				);

	return connected;
}

void BioXASWigglerGapStatus::setGap(AMControl *newControl)
{
	if (gap_ != newControl) {

		if (removeChildControl(gap_))
			setBaseControl(0);

		gap_ = newControl;

		if (addChildControl(gap_))
			setBaseControl(gap_);

		emit gapChanged(gap_);
	}
}

