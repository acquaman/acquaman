#ifndef AMSLITCENTER_H
#define AMSLITCENTER_H

#include "beamline/AMSlitControl.h"

class AMSlitCenter : public AMSlitControl
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMSlitCenter(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~AMSlitCenter();

protected slots:
	/// Updates the value.
	virtual void updateValue();

protected:
	/// Creates and returns a move action.
	virtual AMAction3* createMoveAction(double setpoint);
};

#endif // AMSLITCENTER_H
