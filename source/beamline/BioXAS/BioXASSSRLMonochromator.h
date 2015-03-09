#ifndef BIOXASSSRLMONOCHROMATOR_H
#define BIOXASSSRLMONOCHROMATOR_H

#include <QObject>

#include "actions3/AMAction3.h"

#define BRAGG_MOTOR_CRYSTAL_CHANGE_POSITION 55.0


class BioXASSSRLMonochromator : public QObject
{
	Q_OBJECT

public:
	/// Enumerates the different region states.
	class Region { public: enum State { None = 0, A, B }; };
	/// Enumerates the paddle status options.
	class Paddle { public: enum Status { NotOut = 0, Out }; };
	/// Enumerates the slits status options.
	class Slits { public: enum Status { NotClosed = 0, Closed }; };
	/// Enumerates the key status options.
	class Key { public: enum Status { Disabled = 0, Enabled }; };
	/// Enumerates the brake status options.
	class Brake { public: enum Status { Disabled = 0, Enabled }; };
	/// Enumerates the position status of the bragg motor, whether it is at the crystal change position.
	class Bragg { public: enum CrystalChangePosition { InPosition = 0, NotInPosition }; };
	/// Enumerates the limit options for the crystal change motor.
	class CrystalChangeMotor { public: enum Limit { AtLimit = 0, NotAtLimit }; };

	/// Constructor.
	explicit BioXASSSRLMonochromator(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromator();
	/// Returns true if the mono is connected, false otherwise.
	virtual bool isConnected() const = 0;
	/// Returns the current energy feedback.
	virtual double energy() const = 0;
	/// Returns the current region.
	virtual Region::State region() const = 0;
	/// Returns true if both the upper and lower slits are closed, false otherwise.
	virtual bool slitsClosed() const = 0;
	/// Returns true if paddle is completely removed, false otherwise.
	virtual bool paddleOut() const = 0;
	/// Returns true if the key is enabled, false otherwise.
	virtual bool keyEnabled() const = 0;
	/// Returns true if the bragg motor is at the crystal change position, false otherwise.
	virtual bool braggAtCrystalChangePosition() const = 0;
	/// Returns true if the brake is enabled, false otherwise.
	virtual bool brakeEnabled() const = 0;
	/// Returns true if the crystal change motor is at its clockwise limit.
	virtual bool crystalChangeAtCWLimit() const = 0;
	/// Returns true if the crystal change motor is at its counter-clockwise limit.
	virtual bool crystalChangeAtCCWLimit() const = 0;
	/// Returns the bragg angle offset.
	virtual double braggMotorAngleOffset() const = 0;
	/// Returns the hc constant, used to convert between energies and angles.
	virtual double hc() const = 0;
	/// Returns the crystal 2D spacing.
	virtual double crystal2D() const = 0;
	/// Returns the current physical bragg angle.
	virtual double braggAngle() const = 0;

	/// Returns a new 'set energy' action, 0 if not connected. The argument is the desired energy.
	virtual AMAction3* createSetEnergyAction(double newEnergy) = 0;
	/// Returns a new 'close slits' action, 0 if not connected.
	virtual AMAction3* createCloseSlitsAction() = 0;
	/// Returns a new 'remove paddle' action, 0 if not connected.
	virtual AMAction3* createRemovePaddleAction() = 0;
	/// Returns a new 'move bragg motor' action, 0 if not connected. The argument is the desired destination.
	virtual AMAction3* createMoveBraggMotorAction(double degDestination) = 0;
	/// Returns a new 'move crystal change motor' action, 0 if not connected. The argument is the desired relative move.
	virtual AMAction3* createMoveCrystalChangeMotorAction(int relDestination) = 0;

signals:
	/// Notifier that the connected state has changed.
	void connected(bool isConnected);
	/// Notifier that the energy has changed.
	void energyChanged(double newEnergy);
	/// Notifier that the current region has changed.
	void regionChanged(Region::State newRegion);
	/// Notifier that the status of the slits has changed, either they are closed (true) or not closed (false).
	void slitsStatusChanged(bool closed);
	/// Notifier that the paddle status has changed, either it is fully removed (true) or not (false).
	void paddleStatusChanged(bool removed);
	/// Notifier that the key status has changed.
	void keyStatusChanged(bool enabled);
	/// Notifier that the bragg crystal change position status has changed, either it is in position (true) or not (false).
	void braggCrystalChangePositionStatusChanged(bool inPosition);
	/// Notifier that the brake status has changed.
	void brakeStatusChanged(bool enabled);
	/// Notifier that the status of the crystal change clockwise limit switch has changed, either the crystal change motor is at the limit (true) or not (false).
	void crystalChangeCWLimitStatusChanged(bool atLimit);
	/// Notifier that the status of the crystal change counter-clockwise limit switch has changed, either the crystal change motor is at the limit (true) or not (false).
	void crystalChangeCCWLimitStatusChanged(bool atLimit);

public slots:
	/// Sets the energy setpoint.
	virtual void setEnergy(double newEnergy) = 0;
	/// Sets the region.
	virtual void setRegion(Region::State newRegion) = 0;
	/// Sets the status of both slits.
	virtual void setSlitsClosed() = 0;
	/// Removes the paddle.
	virtual void setPaddleOut() = 0;
	/// Sets the position of the bragg motor.
	virtual void setBraggMotorPosition(double degDestination) = 0;
	/// Sets the position of the crystal change motor.
	virtual void setCrystalChangeMotorPosition(double relDestination) = 0;
	/// Sets the bragg angle offset.
	virtual void setBraggMotorAngleOffset(double newOffset) = 0;
};

#endif // BIOXASSSRLMONOCHROMATOR_H
