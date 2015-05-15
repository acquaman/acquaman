#ifndef CLSJJSLITGAPCONTROL_H
#define CLSJJSLITGAPCONTROL_H

#include "beamline/CLS/CLSJJSlitBladesControl.h"

class CLSJJSlitGapControl : public CLSJJSlitBladesControl
{
	Q_OBJECT

public:
	/// Constructor.
	explicit CLSJJSlitGapControl(const QString &name, AMControl *firstMotorControl, AMControl *secondMotorControl, QObject *parent = 0);
	/// Destructor.
	virtual ~CLSJJSlitGapControl();

protected slots:
	/// Handles updating the saved gap value, according to the current motor control values.
	virtual void updateValue();
	/// Handles updating the control's minimum value.
	virtual void updateMinimumValue();
	/// Handles updating the control's maximum value.
	virtual void updateMaximumValue();

protected:
	/// Creates and returns a move action. Returns 0 if not connected.
	virtual AMAction3* createMoveAction(double gap);

};

#endif // CLSJJSLITGAPCONTROL_H
