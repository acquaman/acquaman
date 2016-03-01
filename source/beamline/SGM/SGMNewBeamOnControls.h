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
	 * Creates an instace of the SGM's new Beam operation controls
	 */
	SGMNewBeamOnControls(QObject* parent = 0);

	/*!
	 * The operation control for the beam on operation
	 */
	AMControl* beamOnOperationControl() const;

	/*!
	 * The operation control for the beam off operation
	 */
	AMControl* beamOffOperationControl() const;

	/*!
	 * The current beam status control
	 */
	AMControl* beamStatusControl() const;
};

#endif // SGMNEWBEAMONCONTROLS_H
