#ifndef BIOXASSSRLMONOCHROMATOR_H
#define BIOXASSSRLMONOCHROMATOR_H

#include <QObject>

#include "actions3/AMActionSupport.h"
#include "beamline/AMControl.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorEnergyControl.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorRegionControl.h"
#include "beamline/BioXAS/BioXASMonochromator.h"

class BioXASSSRLMonochromator : public BioXASMonochromator
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
	explicit BioXASSSRLMonochromator(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromator();

	/// Returns true if the mono is connected, false otherwise.
	virtual bool isConnected() const;

	/// Returns the mono move settling time.
	double settlingTime() const { return settlingTime_; }

	/// Returns the energy control (the encoder-based, by default).
	virtual BioXASSSRLMonochromatorEnergyControl* energyControl() const { return stepEnergy_; }
	/// Returns the bragg encoder-based energy control.
	BioXASSSRLMonochromatorEnergyControl* encoderEnergyControl() const { return encoderEnergy_; }
	/// Returns the bragg step-based energy control.
	BioXASSSRLMonochromatorEnergyControl *stepEnergyControl() const { return stepEnergy_; }
	/// Returns the region control.
	virtual BioXASSSRLMonochromatorRegionControl* regionControl() const { return region_; }

	/// Returns the upper slit control.
	AMControl* upperSlitControl() const { return upperSlit_; }
	/// Returns the lower slit control.
	AMControl* lowerSlitControl() const { return lowerSlit_; }
	/// Returns the slits status control.
	AMControl* slitsStatusControl() const { return slitsStatus_; }
	/// Returns the paddle control.
	AMControl* paddleControl() const { return paddle_; }
	/// Returns the paddle status control.
	AMControl* paddleStatusControl() const { return paddleStatus_; }
	/// Returns the key status control.
	AMControl* keyStatusControl() const { return keyStatus_; }
	/// Returns the brake status control.
	AMControl* brakeStatusControl() const { return brakeStatus_; }
	/// Returns the step-based bragg position control.
	AMControl* stepBraggControl() const { return braggMotor_; }
	/// Returns the encoder-based bragg position control.
	AMControl* encoderBraggControl() const { return encoderBragg_; }
	/// Returns the bragg motor at crystal change position status control.
	AMControl* braggAtCrystalChangePositionStatusControl() const { return braggAtCrystalChangePositionStatus_; }
	/// Returns the crystal change control.
	AMControl* crystalChangeControl() const { return crystalChange_; }
	/// Returns the crystal change cw limit status control.
	AMControl* crystalChangeCWLimitStatusControl() const { return crystalChangeCWLimitStatus_; }
	/// Returns the crystal change ccw limit status control.
	AMControl* crystalChangeCCWLimitStatusControl() const { return crystalChangeCCWLimitStatus_; }
	/// Returns the region A status control.
	AMControl* regionAStatusControl() const { return regionAStatus_; }
	/// Returns the region B status control.
	AMControl* regionBStatusControl() const { return regionBStatus_; }
	/// Returns the m1 mirror pitch control.
	AMControl* m1MirrorPitchControl() const { return m1Pitch_; }

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

	/// Returns a newly created action that calibrates the given mono energy to the desired mono energy.
	AMAction3* createEnergyCalibrationAction(double monoEnergy, double desiredEnergy);
	/// Returns a newly created action that calibrates the mono's current energy to the desired energy.
	AMAction3* createEnergyCalibrationAction(double desiredEnergy);

signals:
	/// Notifier that the m1 mirror pitch control has changed.
	void m1MirrorPitchControlChanged(AMControl *newControl);
	/// Notifier that the mono move settling time has changed.
	void settlingTimeChanged(double newTimeSeconds);

public slots:
	/// Sets the m1 mirror pitch control.
	void setM1MirrorPitchControl(AMControl* newControl);
	/// Sets the mono move settling time.
	void setSettlingTime(double newTimeSeconds);

	/// Sets the calibrated bragg position.
	void calibrateBraggPosition(double newPosition);

protected slots:
	/// Handles updating the motors necessary to produce the desired mono move settling time.
	void updateMotorSettlingTime();

protected:
	/// The mono move settling time, in seconds.
	double settlingTime_;

	/// The bragg encoder-based energy control.
	BioXASSSRLMonochromatorEnergyControl *encoderEnergy_;
	/// The bragg step-based energy control.
	BioXASSSRLMonochromatorEnergyControl *stepEnergy_;
	/// The region control.
	BioXASSSRLMonochromatorRegionControl *region_;

	/// The upper slit motor control.
	AMControl *upperSlit_;
	/// The lower slit motor control.
	AMControl *lowerSlit_;
	/// The slits status control.
	AMControl *slitsStatus_;
	/// The paddle motor control.
	AMControl *paddle_;
	/// The paddle status control.
	AMControl *paddleStatus_;
	/// The key status control.
	AMControl *keyStatus_;
	/// The bragg motor control.
	AMControl *encoderBragg_;
	/// The bragg motor at crystal change position status control.
	AMControl *braggAtCrystalChangePositionStatus_;
	/// The brake status control.
	AMControl *brakeStatus_;
	/// The crystal change motor control.
	AMControl *crystalChange_;
	/// The crystal change clockwise limit status control.
	AMControl *crystalChangeCWLimitStatus_;
	/// The crystal change counter-clockwise limit status control.
	AMControl *crystalChangeCCWLimitStatus_;
	/// The region A status control.
	AMControl *regionAStatus_;
	/// The region B status control.
	AMControl *regionBStatus_;

	/// The bragg motor set position control.
	AMControl *braggSetPosition_;
	/// The m1 mirror pitch control.
	AMControl *m1Pitch_;

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

#endif // BIOXASSSRLMONOCHROMATOR_H
