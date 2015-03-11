#ifndef BIOXASSSRLMONOCHROMATOR_H
#define BIOXASSSRLMONOCHROMATOR_H

#include <QObject>

#include "actions3/AMAction3.h"

class BioXASSSRLMonochromator : public QObject
{
	Q_OBJECT

public:
	/// Enumerates the different region states.
	class Region { public: enum State { None = 0, A, B }; enum Status { NotIn = 0, In = 1}; };
	/// Enumerates the paddle status options.
	class Paddle { public: enum Status { NotOut = 0, Out }; };
	/// Enumerates the slits status options.
	class Slits { public: enum Status { NotClosed = 0, Closed }; };
	/// Enumerates the key status options.
	class Key { public: enum Status { Disabled = 0, Enabled }; };
	/// Enumerates the brake status options.
	class Brake { public: enum Status { Enabled = 0, Disabled }; };
	/// Enumerates the position status of the bragg motor, whether it is at the crystal change position.
	class Bragg { public: enum CrystalChangePosition { NotInPosition = 0, InPosition = 1 }; };
	/// Enumerates the limit options for the crystal change motor.
	class CrystalChange { public: enum Limit { AtLimit = 0, NotAtLimit }; };

	/// Constructor.
	explicit BioXASSSRLMonochromator(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromator();
	/// Returns true if the mono is connected, false otherwise.
	virtual bool isConnected() const = 0;
	/// Returns the current energy feedback.
	virtual double energy() const = 0;
	/// Returns the current region.
	virtual double region() const = 0;

	/// Returns a new 'set energy' action, 0 if not connected. The argument is the desired energy.
	virtual AMAction3* createSetEnergyAction(double newEnergy) = 0;
	/// Returns a new action that adjusts the bragg motor offset s.t. the mono energy matches the desired energy.
	virtual AMAction3* createSetEnergyCalibrationAction(double newEnergy) = 0;
	/// Returns a new 'set region' action, 0 if not connected. The argument is the desired region.
	virtual AMAction3* createSetRegionAction(double newRegion) = 0;
//	/// Returns a new 'close slits' action, 0 if not connected.
//	virtual AMAction3* createCloseSlitsAction() = 0;
//	/// Returns a new 'remove paddle' action, 0 if not connected.
//	virtual AMAction3* createRemovePaddleAction() = 0;
//	/// Returns a new 'move bragg motor' action, 0 if not connected. The argument is the desired destination.
//	virtual AMAction3* createMoveBraggMotorAction(double degDestination) = 0;
//	/// Returns a new 'move crystal change motor' action, 0 if not connected. The argument is the desired relative move.
//	virtual AMAction3* createMoveCrystalChangeMotorAction(int relDestination) = 0;

signals:
	/// Notifier that the connected state has changed.
	void connected(bool isConnected);
	/// Notifier that the energy has changed.
	void energyChanged(double newEnergy);
	/// Notifier that the current region has changed.
	void regionChanged(double newRegion);
	/// Notifier that the slits status has changed
	void slitsStatusChanged(bool closed);
	/// Notifier that the paddle status has changed.
	void paddleStatusChanged(bool removed);
	/// Notifier that the key status has changed.
	void keyStatusChanged(bool enabled);
	/// Notifier that the brake status has changed.
	void brakeStatusChanged(bool enabled);
	/// Notifier that the bragg angle just reached or just left the crystal change position.
	void braggAtCrystalChangePositionStatusChanged(bool inPosition);

public slots:
	/// Sets the energy setpoint.
	virtual void setEnergy(double newEnergy) = 0;
	/// Sets the bragg offset such that the mono energy matches the desired energy.
	virtual void setEnergyCalibration(double newEnergy) = 0;
	/// Sets the region.
	virtual void setRegion(double newRegion) = 0;
};

#endif // BIOXASSSRLMONOCHROMATOR_H
