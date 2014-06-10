#ifndef VESPERSATTOCUBEHACK_H
#define VESPERSATTOCUBEHACK_H

#include <QObject>

#include "beamline/AMControl.h"

/// This is a hack that detects when the Rz motor reaches it's designated point and then stops the motor.
class VESPERSAttoCubeHack : public QObject
{
	Q_OBJECT

public:
	/// Constructor.  Takes the control to the Rz motor.
 	virtual ~VESPERSAttoCubeHack();
	explicit VESPERSAttoCubeHack(AMControl *Rz, QObject *parent = 0);

signals:

public slots:

protected slots:
	/// Handles the value changed of the motor.
	void onValueChanged();

protected:
	/// Holds the control.
	AMControl *Rz_;
	/// Holds the feedback PV in particular.
	AMControl *Rzfbk_;
};

#endif // VESPERSATTOCUBEHACK_H
