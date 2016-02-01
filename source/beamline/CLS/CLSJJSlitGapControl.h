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
	/// Handles updating the saved minimum value.
	virtual void updateMinimumValue();
	/// Handles updating the saved maximum value.
	virtual void updateMaximumValue();

protected:
	/// Creates and returns a move action. Returns 0 if not connected.
	virtual AMAction3* createMoveAction(double gap);

	/// Calculates and returns the minimum possible gap, provided the minimum motor position and the current center position.
	static double calculateMinimumValue(double minimumPosition, double centerPosition);
	/// Calculates and returns the maximum possible gap, provided the maximum motor position and the current center position.
	static double calculateMaximumValue(double maximumPosition, double centerPosition);
};

#endif // CLSJJSLITGAPCONTROL_H
