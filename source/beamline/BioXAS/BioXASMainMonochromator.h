#ifndef BIOXASMAINMONOCHROMATOR_H
#define BIOXASMAINMONOCHROMATOR_H

#include <QObject>

#include "beamline/AMControl.h"
#include "beamline/AMPVControl.h"
#include "actions3/AMAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "actions3/actions/AMControlWaitAction.h"
#include "actions3/AMListAction3.h"
#include "beamline/CLS/CLSMAXvMotor.h"

#include "beamline/BioXAS/BioXASMainMonochromatorControl.h"
#include "beamline/BioXAS/BioXASMainMonochromatorRegionControl.h"

class BioXASMainMonochromator : public QObject
{
    Q_OBJECT

public:
	/// Enumerates the possible slits closed states.
	class Slits { public: enum Status { NotClosed = 0, Closed = 1 }; };
	/// Enumerates the possible paddle states.
	class Paddle { public: enum Status { NotOut = 0, Out = 1}; };
	/// Enumerates the possible key status states.
	class Key { public: enum Status { Disabled = 0, Enabled = 1 }; };
	/// Enumerates the possible brake status states.
	class Brake { public: enum Status { Disabled = 0, Enabled = 1 }; };
	/// Enumerates the bragg motor position status.
	class Bragg { public: enum CrystalChangePositionStatus { InPosition = 0, NotInPosition = 1 }; };
	/// Enumerates the crystal change motor limit status.
	class CrystalChange { public: enum Limit { NotAtLimit = 0, AtLimit = 1 }; };

    /// Constructor.
    explicit BioXASMainMonochromator(QObject *parent = 0);
    /// Destructor.
    virtual ~BioXASMainMonochromator();

    /// Returns true if the mono is connected to all of its pvs, false otherwise.
    bool isConnected() const { return connected_; }
    /// Returns the current region.
	double region() const { return regionControl_->value(); }
	/// Returns the energy feedback.
	double energy() const { return energy_->value(); }

	/// Returns the region A status control.
	AMControl* regionAStatus() const { return regionAStatus_; }
	/// Returns the region B status control.
	AMControl* regionBStatus() const { return regionBStatus_; }
    /// Returns the upper slit blade motor.
	AMControl* upperSlitBladeMotor() const { return upperSlitMotor_; }
    /// Returns the lower slit blade motor.
	AMControl* lowerSlitBladeMotor() const { return lowerSlitMotor_; }
	/// Returns the slits status control.
	AMControl* slitsStatus() const { return slitsStatus_; }
	/// Returns the phosphor paddle motor.
	AMControl* paddleMotor() { return paddleMotor_; }
	/// Returns the phosphor paddle status control.
	AMControl* paddleStatus() { return paddleStatus_; }
	/// Returns the key status control.
	AMControl* keyStatus() const { return keyStatus_; }
    /// Returns the bragg motor.
	AMControl* braggMotor() const { return braggMotor_; }
	/// Returns the bragg motor status, whether or not it is at the crystal change position.
	AMControl* braggMotorCrystalChangeStatus() const { return braggMotorCrystalChangeStatus_; }
    /// Returns the vertical motor.
	AMControl* verticalMotor() const { return verticalMotor_; }
    /// Returns the lateral motor.
	AMControl* lateralMotor() const { return lateralMotor_; }
	/// Returns the brake status control.
	AMControl* brakeStatus() const { return brakeStatus_; }
    /// Returns the crystal change motor.
	AMControl* crystalChangeMotor() const { return crystalChangeMotor_; }
    /// Returns the crystal 1 pitch motor.
	AMControl* crystal1PitchMotor() const { return crystal1PitchMotor_; }
    /// Returns the crystal 1 roll motor.
	AMControl* crystal1RollMotor() const { return crystal1RollMotor_; }
    /// Returns the crystal 2 pitch motor.
	AMControl* crystal2PitchMotor() const { return crystal2PitchMotor_; }
    /// Returns the crystal 2 roll motor.
	AMControl* crystal2RollMotor() const { return crystal2RollMotor_; }

	/// Returns the mono's crystal change control.
	BioXASMainMonochromatorRegionControl* regionControl() const { return regionControl_; }
    /// Returns the energy setpoint control.
    AMControl* energyControl() const { return energy_; }

	/// Returns a new action that sets the region, 0 if not connected.
	AMAction3* createSetRegionAction(double newRegion);
    /// Returns a new set energy action, 0 if not connected.
    AMAction3* createSetEnergyAction(double newEnergy);

signals:
    /// Notifier that the mono's connections with its pvs have changed.
    void connected(bool isConnected);
    /// Notifier that the mono's region has changed.
	void regionChanged(double newRegion);
    /// Notifier that the energy setpoint has changed.
    void energySetpointChanged(double newSetpoint);
    /// Notifier that the energy feedback has changed.
    void energyFeedbackChanged(double newFeedback);

public slots:
	/// Sets the region to the given region, performing a crystal change if necessary.
	void setRegion(double newRegion);
    /// Sets the energy setpoint.
	void setEnergy(double newEnergy);

protected slots:
    /// Updates the mono's general connected state based on the connected state of each of its pvs.
    void onConnectedChanged();

protected:
	/// Creates the region control.
	void initializeRegionControl();

protected:
	/// Connected state.
    bool connected_;
	/// Region control.
	BioXASMainMonochromatorRegionControl* regionControl_;
	/// Energy control.
	BioXASMainMonochromatorControl* energy_;

	// Controls

	AMControl* regionAStatus_;
	AMControl* regionBStatus_;
	CLSMAXvMotor *upperSlitMotor_;
	CLSMAXvMotor *lowerSlitMotor_;
	AMControl* slitsStatus_;
	CLSMAXvMotor *paddleMotor_;
	AMControl* paddleStatus_;
	AMControl* keyStatus_;
    CLSMAXvMotor *braggMotor_;
	AMControl* braggMotorCrystalChangeStatus_;
    CLSMAXvMotor *verticalMotor_;
    CLSMAXvMotor *lateralMotor_;
	AMControl* brakeStatus_;
	CLSMAXvMotor *crystalChangeMotor_;
    CLSMAXvMotor *crystal1PitchMotor_;
    CLSMAXvMotor *crystal1RollMotor_;
    CLSMAXvMotor *crystal2PitchMotor_;
    CLSMAXvMotor *crystal2RollMotor_;

};


#endif // BIOXASMAINMONOCHROMATOR_H
