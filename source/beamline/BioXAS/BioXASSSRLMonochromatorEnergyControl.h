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
	virtual double hc() const = 0;
	/// Returns the crystal 2D spacing.
	virtual double crystal2D() const = 0;
	/// Returns the physical bragg angle.
	virtual double braggAngle() const = 0;
	/// Returns the energy offset. This value is what's adjusted during energy calibration.
	virtual double angleOffset() const = 0;

	/// Returns the hc constant control.
	virtual AMControl* hcControl() const = 0;
	/// Returns the crystal 2D control.
	virtual AMControl* crystal2DControl() const = 0;
	/// Returns the goniometer bragg angle control.
	virtual AMControl* braggAngleControl() const = 0;
	/// Returns the energy offset control. Its value is increased or decreased with energy calibration.
	virtual AMControl* angleOffsetControl() const = 0;

	/// Returns a new action that adjusts the angle offset s.t. the mono energy matches the desired energy.
	AMAction3* createSetEnergyCalibrationAction(double newEnergy);

public slots:
//	AMControl::FailureExplanation move(double setpoint);
	/// Sets the bragg offset such that the mono energy matches the desired energy.
	void setEnergyCalibration(double newEnergy);

protected:
	/// Returns the bragg motor angle offset needed in order for the old energy to match the desired new energy.
	double calibrateEnergy(double oldEnergy, double newEnergy) const;
};

#endif // BIOXASSSRLMONOCHROMATORENERGYCONTROL_H
