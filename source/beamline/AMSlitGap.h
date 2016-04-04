#ifndef AMSLITGAP_H
#define AMSLITGAP_H

#include "beamline/AMSlitControl.h"

class AMSlitGap : public AMSlitControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit AMSlitGap(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~AMSlitGap();

protected slots:
	/// Updates the value.
	virtual void updateValue();

protected:
	/// Creates and returns a new move action.
	virtual AMAction3* createMoveAction(double setpoint);

};

#endif // AMSLITGAP_H
