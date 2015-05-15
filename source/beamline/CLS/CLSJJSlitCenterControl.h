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

protected:
	/// Creates and returns a move action. Returns 0 if not connected.
	virtual AMAction3* createMoveAction(double centerPosition);
};

#endif // CLSJJSLITCENTERCONTROL_H
