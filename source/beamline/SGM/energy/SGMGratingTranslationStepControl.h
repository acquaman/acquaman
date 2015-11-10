#ifndef SGMGRATINGTRANSLATIONSTEPCONTROL_H
#define SGMGRATINGTRANSLATIONSTEPCONTROL_H

#include "beamline/AMPseudoMotorControl.h"
/*!
  * Pseudo-motor class which acts as a wrapper for the grating translation PV,
  * which does not update its moving status if asked to move within tolerance.
  */
class SGMGratingTranslationStepControl : public AMPseudoMotorControl
{
	Q_OBJECT
public:
	/*!
	  * Creates an instance of an SGM grating translation step control.
	  */
	explicit SGMGratingTranslationStepControl(QObject *parent = 0);

	/*!
	  * Whether the control is designed to be able to measure values.
	  */
	bool shouldMeasure() const;

	/*!
	  * Whether the control is designed to be able to move.
	  */
	bool shouldMove() const;

	/*!
	  * Whether the control is designed to be able to stop.
	  */
	bool shouldStop() const;

	/*!
	  * Whether the control can measure values in its current state.
	  */
	bool canMeasure() const;

	/*!
	  * Whether the control can move in its current state.
	  */
	bool canMove() const;

	/*!
	  * Whether the control can stop in its current state.
	  */
	bool canStop() const;
signals:

public slots:
protected slots:

	/*!
	 * Slot which updates the connected state of the control.
	 */
	void updateConnected();

	/*!
	 * Slot which updates the moving state of the control.
	 */
	void updateMoving();

	/*!
	 * Slot which updates the value of the control.
	 */
	void updateValue();
protected:

	/*!
	 * Creates a move action to send the grating translation step PV to the
	 * provided value.
	 * \param setpoint ~ The energy value to move the controls to.
	 */
	virtual AMAction3* createMoveAction(double setpoint);
	AMControl* gratingTranslationStepPV_;
};

#endif // SGMGRATINGTRANSLATIONSTEPCONTROL_H
