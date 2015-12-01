#ifndef SGMNEWBEAMONCONTROLS_H
#define SGMNEWBEAMONCONTROLS_H

#include "beamline/AMControlSet.h"
class AMControl;
/*!
 * A control set class which contains all the individual PVs added for the Beam On procedure for the SGM Beamline
 */
class SGMNewBeamOnControls : public AMControlSet
{
	Q_OBJECT
public:
	/*!
	 * Creates an instace of the SGM's new Beam On Pvs
	 */
	SGMNewBeamOnControls(QObject* parent = 0);

	/*!
	 * The setpoint PV for the beam on procedure
	 */
	AMControl* beamOnSetpointControl() const;

	/*!
	 * The feedback PV for the beam on procedure
	 */
	AMControl* beamOnFeedbackControl() const;

	/*!
	 * The moving status PV for the beam on procedure
	 */
	AMControl* beamOnStatusControl() const;
};

#endif // SGMNEWBEAMONCONTROLS_H
