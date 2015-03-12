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

#include "beamline/BioXAS/BioXASSSRLMonochromator.h"
#include "beamline/BioXAS/BioXASMainMonochromatorControl.h"
#include "beamline/BioXAS/BioXASMainMonochromatorRegionControl.h"

class BioXASMainMonochromator : public BioXASSSRLMonochromator
{
    Q_OBJECT

public:
    /// Constructor.
    explicit BioXASMainMonochromator(QObject *parent = 0);
    /// Destructor.
    virtual ~BioXASMainMonochromator();

    /// Returns true if the mono is connected to all of its pvs, false otherwise.
	virtual bool isConnected() const { return connected_; }
    /// Returns the current region.
	virtual double region() const { return region_->value(); }
	/// Returns the energy feedback.
	virtual double energy() const { return energy_->value(); }

    /// Returns the upper slit blade motor.
	CLSMAXvMotor* upperSlitBladeMotor() const { return upperSlitMotor_; }
    /// Returns the lower slit blade motor.
	CLSMAXvMotor* lowerSlitBladeMotor() const { return lowerSlitMotor_; }
	/// Returns the phosphor paddle motor.
	CLSMAXvMotor* paddleMotor() { return paddleMotor_; }
    /// Returns the bragg motor.
	CLSMAXvMotor* braggMotor() const { return braggMotor_; }
    /// Returns the vertical motor.
	CLSMAXvMotor* verticalMotor() const { return verticalMotor_; }
    /// Returns the lateral motor.
	CLSMAXvMotor* lateralMotor() const { return lateralMotor_; }
    /// Returns the crystal change motor.
	CLSMAXvMotor* crystalChangeMotor() const { return crystalChangeMotor_; }
    /// Returns the crystal 1 pitch motor.
	CLSMAXvMotor* crystal1PitchMotor() const { return crystal1PitchMotor_; }
    /// Returns the crystal 1 roll motor.
	CLSMAXvMotor* crystal1RollMotor() const { return crystal1RollMotor_; }
    /// Returns the crystal 2 pitch motor.
	CLSMAXvMotor* crystal2PitchMotor() const { return crystal2PitchMotor_; }
    /// Returns the crystal 2 roll motor.
	CLSMAXvMotor* crystal2RollMotor() const { return crystal2RollMotor_; }

	/// Returns the mono's region control.
	BioXASSSRLMonochromatorRegionControl* regionControl() const { return region_; }
	/// Returns the energy setpoint control.
	AMControl* energyControl() const { return energy_; }
	/// Returns the slits status control.
	virtual AMControl* slitsStatusControl() const { return slitsStatus_; }
	/// Returns the paddle status control.
	virtual AMControl* paddleStatusControl() const { return paddleStatus_; }
	/// Returns the key status control.
	virtual AMControl* keyStatusControl() const { return keyStatus_; }
	/// Returns the brake status control.
	virtual AMControl* brakeStatusControl() const { return brakeStatus_; }
	/// Returns the bragg at crystal change position status control.
	virtual AMControl* braggAtCrystalChangePositionStatusControl() const { return braggAtCrystalChangePositionStatus_; }

	/// Returns a new action that sets the region, 0 if not connected.
	virtual AMAction3* createSetRegionAction(double newRegion);
    /// Returns a new set energy action, 0 if not connected.
	virtual AMAction3* createSetEnergyAction(double newEnergy);
	/// Returns a new action that adjusts the bragg motor offset such that the current energy matches the desired energy.
	virtual AMAction3* createSetEnergyCalibrationAction(double newEnergy) { Q_UNUSED(newEnergy) return 0; }

public slots:
	/// Sets the region to the given region, performing a crystal change if necessary.
	virtual void setRegion(double newRegion);
    /// Sets the energy setpoint.
	virtual void setEnergy(double newEnergy);
	/// Sets the bragg motor offset such that the current energy matches the desired energy.
	virtual void setEnergyCalibration(double newEnergy) { Q_UNUSED(newEnergy) return; }

protected slots:
    /// Updates the mono's general connected state based on the connected state of each of its pvs.
    void onConnectedChanged();
	/// Emits the slitsStatusChanged(bool) signal once the slits status has changed.
	void onSlitsStatusChanged(double status) { emit slitsStatusChanged(status == BioXASSSRLMonochromator::Slits::Closed); }
	/// Emits the paddleStatusChanged(bool) signal once the slits status has changed.
	void onPaddleStatusChanged(double status) { emit paddleStatusChanged(status == BioXASSSRLMonochromator::Paddle::Out); }
	/// Emits the keyStatusChanged(bool) signal once the key status has changed.
	void onKeyStatusChanged(double status) { emit keyStatusChanged(status == BioXASSSRLMonochromator::Key::Enabled); }
	/// Emits the brakeStatusChanged(bool) signal once the brake status has changed.
	void onBrakeStatusChanged(double status) { emit brakeStatusChanged(status == BioXASSSRLMonochromator::Brake::Enabled); }
	/// Emits the braggAtCrystalChangePositionStatusChanged(bool) signal once the bragg position status has changed.
	void onBraggAtCrystalChangePositionStatusChanged(double status) { emit braggAtCrystalChangePositionStatusChanged(status == BioXASSSRLMonochromator::Bragg::InPosition); }

protected:
	/// Connected state.
	bool connected_;

	// Controls

	/// Region control.
	BioXASMainMonochromatorRegionControl* region_;
	/// Energy control.
	BioXASMainMonochromatorControl* energy_;
	/// Slits status control.
	AMControl *slitsStatus_;
	/// Paddle status control.
	AMControl *paddleStatus_;
	/// Key status control.
	AMControl *keyStatus_;
	/// Brake status control.
	AMControl *brakeStatus_;
	/// Bragg at crystal change position status control.
	AMControl* braggAtCrystalChangePositionStatus_;

	// Physical motors

	/// Upper slit blade motor.
	CLSMAXvMotor *upperSlitMotor_;
	/// Lower slit blade motor.
	CLSMAXvMotor *lowerSlitMotor_;
	/// Paddle motor.
	CLSMAXvMotor *paddleMotor_;
	/// Bragg motor.
	CLSMAXvMotor *braggMotor_;
	/// Vertical motor.
	CLSMAXvMotor *verticalMotor_;
	/// Lateral motor.
	CLSMAXvMotor *lateralMotor_;
	/// Crystal change motor.
	CLSMAXvMotor *crystalChangeMotor_;
	/// Crystal 1 pitch motor.
	CLSMAXvMotor *crystal1PitchMotor_;
	/// Crystal 1 roll motor.
	CLSMAXvMotor *crystal1RollMotor_;
	/// Crystal 2 pitch motor.
	CLSMAXvMotor *crystal2PitchMotor_;
	/// Crystal 2 roll motor.
	CLSMAXvMotor *crystal2RollMotor_;
};


#endif // BIOXASMAINMONOCHROMATOR_H
