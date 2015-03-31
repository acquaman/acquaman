#ifndef BIOXASSSRLMONOCHROMATOR_H
#define BIOXASSSRLMONOCHROMATOR_H

#include <QObject>

#include "actions3/AMActionSupport.h"
#include "beamline/AMControl.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorEnergyControl.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorRegionControl.h"

class BioXASSSRLMonochromator : public QObject
{
	Q_OBJECT

public:
	/// Enumerates the different region states.
	class Region { public: enum State { A = 0, B, None }; enum Status { NotIn = 0, In = 1}; };
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
	class CrystalChange { public: enum Limit { NotAtLimit = 0, AtLimit }; };

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

	/// Returns the upper slit blade motor.
	virtual CLSMAXvMotor* upperSlitBladeMotor() const = 0;
	/// Returns the lower slit blade motor.
	virtual CLSMAXvMotor* lowerSlitBladeMotor() const = 0;
	/// Returns the phosphor paddle motor.
	virtual CLSMAXvMotor* paddleMotor() const = 0;
	/// Returns the bragg motor.
	virtual CLSMAXvMotor* braggMotor() const = 0;
	/// Returns the vertical motor.
	virtual CLSMAXvMotor* verticalMotor() const = 0;
	/// Returns the lateral motor.
	virtual CLSMAXvMotor* lateralMotor() const = 0;
	/// Returns the crystal change motor.
	virtual CLSMAXvMotor* crystalChangeMotor() const = 0;
	/// Returns the crystal 1 pitch motor.
	virtual CLSMAXvMotor* crystal1PitchMotor() const = 0;
	/// Returns the crystal 1 roll motor.
	virtual CLSMAXvMotor* crystal1RollMotor() const = 0;
	/// Returns the crystal 2 pitch motor.
	virtual CLSMAXvMotor* crystal2PitchMotor() const = 0;
	/// Returns the crystal 2 roll motor.
	virtual CLSMAXvMotor* crystal2RollMotor() const = 0;

	/// Returns the energy control.
	virtual BioXASSSRLMonochromatorEnergyControl* energyControl() const = 0;
	/// Returns the region control.
	virtual BioXASSSRLMonochromatorRegionControl* regionControl() const = 0;

	/// Returns a new 'set energy' action, 0 if not connected. The argument is the desired energy.
	virtual AMAction3* createSetEnergyAction(double newEnergy);
	/// Returns a new 'set region' action, 0 if not connected. The argument is the desired region.
	virtual AMAction3* createSetRegionAction(double newRegion);

	/// Returns a new action that calibrates the bragg motor. The argument is the desired position.
	virtual AMAction3* createCalibrateBraggPositionAction(double newPosition);

signals:
	/// Notifier that the connected state has changed.
	void connected(bool isConnected);
	/// Notifier that the energy has changed.
	void energyChanged(double newEnergy);
	/// Notifier that the current region has changed.
	void regionChanged(double newRegion);
	/// Notifier that the slits status has changed
	void slitsStatusChanged(double status);
	/// Notifier that the paddle status has changed.
	void paddleStatusChanged(double status);
	/// Notifier that the key status has changed.
	void keyStatusChanged(double status);
	/// Notifier that the brake status has changed.
	void brakeStatusChanged(double status);
	/// Notifier that the bragg angle just reached or just left the crystal change position.
	void braggAtCrystalChangePositionStatusChanged(double status);

public slots:
	/// Sets the energy setpoint.
	void setEnergy(double newEnergy);
	/// Sets the region.
	void setRegion(double newRegion);

	/// Sets the calibrated bragg position.
	void calibrateBraggPosition(double newPosition);

private:
	/// Returns the encoder calibration offset that would be needed to change the current bragg motor position to the newPosition.
	static double calculateBraggEncoderOffset(double oldPosition, double oldEncoderOffset, double newPosition, double encoderSlope);
};

#endif // BIOXASSSRLMONOCHROMATOR_H
