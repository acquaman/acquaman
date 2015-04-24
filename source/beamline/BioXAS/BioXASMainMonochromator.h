#ifndef BIOXASMAINMONOCHROMATOR_H
#define BIOXASMAINMONOCHROMATOR_H

#include <QObject>

#include "beamline/BioXAS/BioXASSSRLMonochromator.h"
#include "beamline/BioXAS/BioXASMainMonochromatorEnergyControl.h"
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
	CLSMAXvMotor* paddleMotor() const { return paddleMotor_; }
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
	virtual BioXASSSRLMonochromatorRegionControl* regionControl() const { return region_; }
	/// Returns the energy setpoint control.
	virtual BioXASSSRLMonochromatorEnergyControl* energyControl() const { return energy_; }

protected slots:
	/// Updates the mono's general connected state based on the connected state of each of its pvs.
	void onConnectedChanged();

protected:
	// Variables

	/// Connected state.
	bool connected_;

	// Controls

	/// Region control.
	BioXASMainMonochromatorRegionControl *region_;
	/// Energy control.
	BioXASMainMonochromatorEnergyControl *energy_;

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
