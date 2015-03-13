#ifndef BIOXASSIDEMONOCHROMATOR_H
#define BIOXASSIDEMONOCHROMATOR_H

#include <QObject>

#include "beamline/AMControl.h"
#include "beamline/AMPVControl.h"
#include "actions3/AMAction3.h"
#include "actions3/AMActionSupport.h"
#include "actions3/actions/AMControlWaitAction.h"
#include "actions3/AMListAction3.h"
#include "beamline/CLS/CLSMAXvMotor.h"

#include "beamline/BioXAS/BioXASSSRLMonochromator.h"
#include "beamline/BioXAS/BioXASSideMonochromatorControl.h"
#include "beamline/BioXAS/BioXASSideMonochromatorRegionControl.h"

class BioXASSideMonochromator : public BioXASSSRLMonochromator
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSideMonochromator(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideMonochromator();

	/// Returns true if the mono is connected, false otherwise.
	virtual bool isConnected() const { return connected_; }
	/// Returns the energy feedback.
	virtual double energy() const { return energy_->value(); }
	/// Returns the current region.
	virtual double region() const { return region_->value(); }
	/// Returns the hc constant.
	virtual double hc() const { return hc_->value(); }
	/// Returns the crystal 2D spacing.
	virtual double crystal2D() const { return crystal2D_->value(); }
	/// Returns the physical bragg angle.
	virtual double braggAngle() const { return braggAngle_->value(); }

	/// Returns the upper slit blade motor.
	virtual CLSMAXvMotor* upperSlitBladeMotor() const { return upperSlitMotor_; }
	/// Returns the lower slit blade motor.
	virtual CLSMAXvMotor* lowerSlitBladeMotor() const { return lowerSlitMotor_; }
	/// Returns the phosphor paddle motor.
	virtual CLSMAXvMotor* paddleMotor() const { return paddleMotor_; }
	/// Returns the bragg motor.
	virtual CLSMAXvMotor* braggMotor() const { return braggMotor_; }
	/// Returns the vertical motor.
	virtual CLSMAXvMotor* verticalMotor() const { return verticalMotor_; }
	/// Returns the lateral motor.
	virtual CLSMAXvMotor* lateralMotor() const { return lateralMotor_; }
	/// Returns the crystal change motor.
	virtual CLSMAXvMotor* crystalChangeMotor() const { return crystalChangeMotor_; }
	/// Returns the crystal 1 pitch motor.
	virtual CLSMAXvMotor* crystal1PitchMotor() const { return crystal1PitchMotor_; }
	/// Returns the crystal 1 roll motor.
	virtual CLSMAXvMotor* crystal1RollMotor() const { return crystal1RollMotor_; }
	/// Returns the crystal 2 pitch motor.
	virtual CLSMAXvMotor* crystal2PitchMotor() const { return crystal2PitchMotor_; }
	/// Returns the crystal 2 roll motor.
	virtual CLSMAXvMotor* crystal2RollMotor() const { return crystal2RollMotor_; }

	/// Returns the energy setpoint control.
	virtual AMControl* energyControl() const { return energy_; }
	/// Returns the region control.
	virtual BioXASSSRLMonochromatorRegionControl* regionControl() const { return region_; }

protected slots:
	/// Updates the mono's general connected state based on the connected state of each of its pvs.
	void onConnectedChanged();

protected:
	// Variables

	/// Connected state.
	bool connected_;

	// Controls

	/// Energy control.
	AMControl* energy_;
	/// Region control.
	BioXASSideMonochromatorRegionControl *region_;
	/// The hc constant.
	AMControl *hc_;
	/// The crystal 2D spacing.
	AMControl *crystal2D_;
	/// The physical bragg angle.
	AMControl *braggAngle_;

	// Motors

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

#endif // BIOXASSIDEMONOCHROMATOR_H
