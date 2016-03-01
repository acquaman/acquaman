#include "SGMNewBeamOnControls.h"

#include "beamline/AMPVControl.h"
SGMNewBeamOnControls::SGMNewBeamOnControls(QObject* parent)
        :AMControlSet(parent)
{
	addControl(new AMSinglePVControl("Beam On Operation", "AM1611-4-I10:beam:opr:on", this, 0.5));
	addControl(new AMSinglePVControl("Beam Off Operation", "AM1611-4-I10:beam:opr:off", this, 0.5));
	addControl(new AMSinglePVControl("Beam Status", "AM1611-4-I10:beam:status", this, 0.5));
}

AMControl*SGMNewBeamOnControls::beamOnOperationControl() const
{
	return controlNamed("Beam On Operation");
}

AMControl*SGMNewBeamOnControls::beamOffOperationControl() const
{
	return controlNamed("Beam Off Operation");
}

AMControl*SGMNewBeamOnControls::beamStatusControl() const
{
	return controlNamed("Beam Status");
}

