#ifndef BIOXASSIDEMONOCHROMATORENERGYCONTROL_H
#define BIOXASSIDEMONOCHROMATORENERGYCONTROL_H

#include "beamline/BioXAS/BioXASSSRLMonochromatorEnergyControl.h"

class BioXASSideMonochromatorEnergyControl : public BioXASSSRLMonochromatorEnergyControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSideMonochromatorEnergyControl(QObject* parent = 0);
	/// Destructor.
	virtual ~BioXASSideMonochromatorEnergyControl();

	/// Returns the hc constant.
	virtual double hc() const { return hc_->value(); }
	/// Returns the crystal 2D spacing.
	virtual double crystal2D() const { return crystal2D_->value(); }
	/// Returns the physical bragg angle.
	virtual double braggAngle() const { return braggAngle_->value(); }
	/// Returns the energy offset. This value is what's adjusted during energy calibration.
	virtual double angleOffset() const { return angleOffset_->value(); }

	/// Returns the hc constant control.
	virtual AMControl* hcControl() const { return hc_; }
	/// Returns the crystal 2D control.
	virtual AMControl* crystal2DControl() const { return crystal2D_; }
	/// Returns the goniometer bragg angle control.
	virtual AMControl* braggAngleControl() const { return braggAngle_; }
	/// Returns the energy offset control. Its value is increased or decreased with energy calibration.
	virtual AMControl* angleOffsetControl() const { return angleOffset_; }

protected:
	/// The hc constant control.
	AMReadOnlyPVControl *hc_;
	/// The crystal 2D control.
	AMReadOnlyPVControl *crystal2D_;
	/// The goniometer bragg angle control.
	AMReadOnlyPVControl *braggAngle_;
	/// The energy offset control.
	AMReadOnlyPVControl *angleOffset_;
};

#endif // BIOXASSIDEMONOCHROMATORENERGYCONTROL_H
