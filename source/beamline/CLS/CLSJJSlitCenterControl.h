#ifndef CLSJJSLITCENTERCONTROL_H
#define CLSJJSLITCENTERCONTROL_H

#include "beamline/CLS/CLSJJSlitBladesControl.h"

class CLSJJSlitCenterControl : public CLSJJSlitBladesControl
{
	Q_OBJECT

public:
	/// Constructor.
	explicit CLSJJSlitCenterControl(const QString &name, AMControl *upperBladeControl, AMControl *lowerBladeControl, QObject *parent = 0);
	/// Destructor.
	virtual ~CLSJJSlitCenterControl();

protected slots:
	/// Handles updating the saved center position value, according to the current motor control values.
	virtual void updateValue();
	/// Handles updating the saved minimum value.
	virtual void updateMinimumValue();
	/// Handles updating the saved maximum value.
	virtual void updateMaximumValue();

protected:
	/// Creates and returns a move action. Returns 0 if not connected.
	virtual AMAction3* createMoveAction(double centerPosition);

	/// Calculates and returns the minimum possible gap, for the given minimum motor position and gap.
	static double calculateMinimumValue(double minimumPosition, double gap);
	/// Calculates and returns the maximum possible gap, for the given maximum motor position and gap.
	static double calculateMaximumValue(double maximumPosition, double gap);
};

#endif // CLSJJSLITCENTERCONTROL_H
