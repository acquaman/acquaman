#ifndef BIOXASMONOCHROMATOR_H
#define BIOXASMONOCHROMATOR_H

#include <QObject>

#include "beamline/AMControl.h"
#include "actions3/AMAction3.h"

class BioXASMonochromator : public QObject
{
    Q_OBJECT

public:
	/// Constructor.
    explicit BioXASMonochromator(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASMonochromator();

	/// Returns true if the mono is connected, false otherwise.
	virtual bool isConnected() const { return false; }
	/// Returns the current energy feedback.
	virtual double energy() const = 0;

	/// Returns the energy control.
	virtual AMControl* energyControl() const = 0;

	/// Returns a newly created action that sets the mono's energy setpoint.
	virtual AMAction3* createSetEnergyAction(double newEnergy) = 0;

signals:
	/// Notifier that the mono's connected state has changed.
	void connected(bool isConnected);
	/// Notifier that the mono's energy has changed.
	void energyChanged(double newEnergy);

public slots:
	/// Sets the mono's energy setpoint.
	virtual void setEnergy(double newEnergy) = 0;
};

#endif // BIOXASMONOCHROMATOR_H
