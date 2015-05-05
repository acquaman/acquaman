#ifndef BIOXASSSRLMONOCHROMATORENERGYCONTROL_H
#define BIOXASSSRLMONOCHROMATORENERGYCONTROL_H

#include <QObject>

#include "beamline/AMPVControl.h"
#include "actions3/AMActionSupport.h"

class BioXASSSRLMonochromatorEnergyControl : public AMPVwStatusControl
{
	Q_OBJECT
public:
	explicit BioXASSSRLMonochromatorEnergyControl(const QString& name,
				       const QString& readPVname,
				       const QString& writePVname,
				       const QString& movingPVname,
				       const QString& stopPVname = QString(),
				       QObject* parent = 0,
				       double tolerance = AMCONTROL_TOLERANCE_DONT_CARE,
				       double moveStartTimeoutSeconds = 2.0,
				       AMAbstractControlStatusChecker* statusChecker = new AMControlStatusCheckerDefault(1),
				       int stopValue = 1,
				       const QString &description = "");
	virtual ~BioXASSSRLMonochromatorEnergyControl();

	/// Returns true if all subcontrols are connected, false otherwise.
	virtual bool isConnected() const;

	/// Returns the hc constant.
	double hc() const { return hc_->value(); }
	/// Returns the crystal 2D spacing.
	double crystal2D() const { return crystal2D_->value(); }
	/// Returns the physical bragg angle.
	double braggAngle() const { return braggAngle_->value(); }
	/// Returns the bragg angle offset. This value is what's adjusted during energy calibration.
	double angleOffset() const { return angleOffset_->value(); }

	/// Returns the hc constant control.
	virtual AMControl* hcControl() const { return hc_; }
	/// Returns the crystal 2D control.
	virtual AMControl* crystal2DControl() const { return crystal2D_; }
	/// Returns the goniometer bragg angle control.
	virtual AMControl* braggAngleControl() const { return braggAngle_; }
	/// Returns the bragg angle offset control. Its value is increased or decreased with energy calibration.
	virtual AMControl* angleOffsetControl() const { return angleOffset_; }

	/// Returns a new action that adjusts the angle offset s.t. the mono energy matches the desired energy.
	AMAction3* createSetEnergyCalibrationAction(double newEnergy);

public slots:
	/// Sets the bragg offset such that the mono energy matches the desired energy.
	void setEnergyCalibration(double newEnergy);

protected:
	/// Returns the bragg motor angle offset needed in order for the old energy to match the desired new energy.
	double calibrateEnergy(double oldEnergy, double newEnergy) const;

protected:
	/// The hc constant control.
	AMReadOnlyPVControl *hc_;
	/// The crystal 2D control.
	AMReadOnlyPVControl *crystal2D_;
	/// The goniometer bragg angle control.
	AMReadOnlyPVControl *braggAngle_;
	/// The bragg angle offset control.
	AMPVControl *angleOffset_;
};

#endif // BIOXASSSRLMONOCHROMATORENERGYCONTROL_H
