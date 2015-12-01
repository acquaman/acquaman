#include "SGMNewBeamOnControls.h"

#include "beamline/AMPVControl.h"
SGMNewBeamOnControls::SGMNewBeamOnControls(QObject* parent)
        :AMControlSet(parent)
{
	addControl(new AMSinglePVControl("Beam on Setpoint", "AM1611-4-I10:beam", this, 0.5));
	addControl(new AMSinglePVControl("Beam on Feedback", "AM1611-4-I10:beam:fbk", this, 0.5));
	addControl(new AMSinglePVControl("Beam on Status", "AM1611-4-I10:beam:status", this, 0.5));
}

AMControl*SGMNewBeamOnControls::beamOnSetpointControl() const
{
	return controlNamed("Beam on Setpoint");
}

AMControl*SGMNewBeamOnControls::beamOnFeedbackControl() const
{
	return controlNamed("Beam on Feedback");
}

AMControl*SGMNewBeamOnControls::beamOnStatusControl() const
{
	return controlNamed("Beam on Status");
}

