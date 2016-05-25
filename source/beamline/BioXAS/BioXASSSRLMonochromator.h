#ifndef BIOXASSSRLMONOCHROMATOR_H
#define BIOXASSSRLMONOCHROMATOR_H

#include <QObject>

#include "actions3/AMActionSupport.h"
#include "beamline/AMControl.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/BioXAS/BioXASMAXvMotor.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorEnergyControl.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorRegionControl.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorMaskState.h"
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
	/// Enumerates the options for calculating the current bragg position and energy.
	class Mode { public: enum Value { Encoder = 0, Step = 1, None = 2 }; };

	/// Constructor.
	explicit BioXASSSRLMonochromator(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromator();

	/// Returns true if the mono is connected, false otherwise.
	virtual bool isConnected() const;

	/// Returns true if this control can be stopped right now, false otherwise. Finds this out be examining all child controls. Subclasses can reimplement to achieve their particular behavior.
	virtual bool canStop() const;

	/// Returns the mask upper blade control.
	BioXASMAXvMotor* maskUpperBlade() const { return maskUpperBlade_; }
	/// Returns the mask lower blade control.
	BioXASMAXvMotor* maskLowerBlade() const { return maskLowerBlade_; }
	/// Returns the mask blades state control.
	AMControl* bladesState() const { return bladesState_; }
	/// Returns the mask state control.
	BioXASSSRLMonochromatorMaskState* maskState() const { return maskState_; }

	/// Returns the paddle control.
	BioXASMAXvMotor* paddle() const { return paddle_; }
	/// Returns the paddle status control.
	AMControl* paddleStatus() const { return paddleStatus_; }
	/// Returns the key status control.
	AMControl* keyStatus() const { return keyStatus_; }
	/// Returns the brake status control.
	AMControl* brakeStatus() const { return brakeStatus_; }
	/// Returns the bragg motor at crystal change position status control.
	AMControl* braggAtCrystalChangePositionStatus() const { return braggAtCrystalChangePositionStatus_; }
	/// Returns the crystal change control.
	BioXASMAXvMotor* crystalChange() const { return crystalChange_; }
	/// Returns the region A status control.
	AMControl* regionAStatus() const { return regionAStatus_; }
	/// Returns the region B status control.
	AMControl* regionBStatus() const { return regionBStatus_; }
	/// Returns the vertical motor.
	CLSMAXvMotor* vertical() const { return vertical_; }
	/// Returns the lateral motor.
	CLSMAXvMotor* lateral() const { return lateral_; }
	/// Returns the crystal 1 pitch motor.
	CLSMAXvMotor* crystal1Pitch() const { return crystal1Pitch_; }
	/// Returns the crystal 1 roll motor.
	CLSMAXvMotor* crystal1Roll() const { return crystal1Roll_; }
	/// Returns the crystal 2 pitch motor.
	CLSMAXvMotor* crystal2Pitch() const { return crystal2Pitch_; }
	/// Returns the crystal 2 roll motor.
	CLSMAXvMotor* crystal2Roll() const { return crystal2Roll_; }

	/// Returns the m1 mirror pitch control.
	AMControl* m1MirrorPitch() const { return m1Pitch_; }

	/// Returns the step-based bragg position control.
	BioXASMAXvMotor* stepBragg() const { return stepBragg_; }
	/// Returns the encoder-based bragg position control.
	BioXASMAXvMotor* encoderBragg() const { return encoderBragg_; }
	/// Returns the bragg motor corresponding to the current mode.
	BioXASMAXvMotor* bragg() const { return bragg_; }

	/// Returns the bragg encoder-based energy control.
	BioXASSSRLMonochromatorEnergyControl* encoderEnergy() const { return encoderEnergy_; }
	/// Returns the bragg step-based energy control.
	BioXASSSRLMonochromatorEnergyControl* stepEnergy() const { return stepEnergy_; }

	/// Returns the region control.
	BioXASSSRLMonochromatorRegionControl* region() const { return region_; }

	/// Returns the mono move settling time.
	double settlingTime() const { return settlingTime_; }
	/// Returns the mode.
	double mode() const { return mode_; }

signals:
	/// Notifier that the upper slit blade control has changed.
	void maskUpperBladeChanged(BioXASMAXvMotor *newControl);
	/// Notifier that the lower slit blade control has changed.
	void maskLowerBladeChanged(BioXASMAXvMotor *newControl);
	/// Notifier that the blades state control has changed.
	void bladesStateChanged(AMControl *newControl);
	/// Notifier that the mask state control has changed.
	void maskStateChanged(BioXASSSRLMonochromatorMaskState *newControl);
	/// Notifier that the paddle control has changed.
	void paddleChanged(BioXASMAXvMotor *newControl);
	/// Notifier that the paddle status control has changed.
	void paddleStatusChanged(AMControl *newControl);
	/// Notifier that the key status control has changed.
	void keyStatusChanged(AMControl *newControl);
	/// Notifier that the brake status control has changed.
	void brakeStatusChanged(AMControl *newControl);
	/// Notifier that the bragg-at-crystal-change-position-status control has changed.
	void braggAtCrystalChangePositionStatusChanged(AMControl *newControl);
	/// Notifier that the crystal change control has changed.
	void crystalChangeChanged(BioXASMAXvMotor *newControl);
	/// Notifier that the region A status control has changed.
	void regionAStatusChanged(AMControl *newControl);
	//// Notifier that the region B status control has changed.
	void regionBStatusChanged(AMControl *newControl);
	/// Notifier that the vertical control has changed.
	void verticalChanged(CLSMAXvMotor *newControl);
	/// Notifier that the lateral control has changed.
	void lateralChanged(CLSMAXvMotor *newControl);
	/// Notifier that the crystal 1 pitch control has changed.
	void crystal1PitchChanged(CLSMAXvMotor *newControl);
	/// Notifier that the crystal 1 roll control has changed.
	void crystal1RollChanged(CLSMAXvMotor *newControl);
	/// Notifier that the crystal 2 pitch control has changed.
	void crystal2PitchChanged(CLSMAXvMotor *newControl);
	/// Notifier that the crystal 2 roll control has changed.
	void crystal2RollChanged(CLSMAXvMotor *newControl);

	/// Notifier that the m1 mirror pitch control has changed.
	void m1MirrorPitchControlChanged(AMControl *newControl);

	/// Notifier that the step-based bragg control has changed.
	void stepBraggChanged(BioXASMAXvMotor *newControl);
	/// Notifier that the encoder-based bragg control has changed.
	void encoderBraggChanged(BioXASMAXvMotor *newControl);
	/// Notifier that the bragg control has changed.
	void braggChanged(BioXASMAXvMotor *newControl);

	/// Notifier that the step-based energy control has changed.
	void stepEnergyChanged(BioXASSSRLMonochromatorEnergyControl *newControl);
	/// Notifier that the encoder-based energy control has changed.
	void encoderEnergyChanged(BioXASSSRLMonochromatorEnergyControl *newControl);

	/// Notifier that the region control has changed.
	void regionChanged(AMControl *newControl);

	/// Notifier that the mono move settling time has changed.
	void settlingTimeChanged(double newTimeSeconds);

	/// Notifier that the mode has changed.
	void modeChanged(double newMode);

public slots:
	/// Sets the mono move settling time.
	void setSettlingTime(double newTimeSeconds);
	/// Sets the m1 mirror pitch control.
	void setM1MirrorPitchControl(AMControl* newControl);
	/// Sets the mode.
	void setMode(Mode::Value newMode);

protected slots:
	/// Sets the upper blade control.
	void setUpperBlade(BioXASMAXvMotor *newControl);
	/// Sets the lower blade control.
	void setLowerBlade(BioXASMAXvMotor *newControl);
	/// Sets the blades state control.
	void setBladesState(AMControl *newControl);
	/// Sets the mask state control.
	void setMaskState(BioXASSSRLMonochromatorMaskState *newControl);
	/// Sets the paddle control.
	void setPaddle(BioXASMAXvMotor *newControl);
	/// Sets the paddle status control.
	void setPaddleStatus(AMControl *newControl);
	/// Sets the key status control.
	void setKeyStatus(AMControl *newControl);
	/// Sets the brake status control.
	void setBrakeStatus(AMControl *newControl);
	/// Sets the bragg-at-crystal-change-position status control.
	void setBraggAtCrystalChangePositionStatus(AMControl *newControl);
	/// Sets the crystal change control.
	void setCrystalChange(BioXASMAXvMotor *newControl);
	/// Sets the region A status control.
	void setRegionAStatus(AMControl *newControl);
	/// Sets the region B status control.
	void setRegionBStatus(AMControl *newControl);
	/// Sets the vertical control.
	void setVertical(CLSMAXvMotor *newControl);
	/// Sets the lateral control.
	void setLateral(CLSMAXvMotor *newControl);
	/// Sets the crystal 1 pitch control.
	void setCrystal1Pitch(CLSMAXvMotor *newControl);
	/// Sets the crystal 1 roll control.
	void setCrystal1Roll(CLSMAXvMotor *newControl);
	/// Sets the crystal 2 pitch control.
	void setCrystal2Pitch(CLSMAXvMotor *newControl);
	/// Sets the crystal 2 roll control.
	void setCrystal2Roll(CLSMAXvMotor *newControl);

	/// Sets the step-based bragg control.
	void setStepBragg(BioXASMAXvMotor *newControl);
	/// Sets the encoder-based bragg control.
	void setEncoderBragg(BioXASMAXvMotor *newControl);
	/// Sets the bragg control.
	void setBragg(BioXASMAXvMotor *newControl);

	/// Sets the step-based energy control.
	void setStepEnergy(BioXASSSRLMonochromatorEnergyControl *newControl);
	/// Sets the encoder-based energy control.
	void setEncoderEnergy(BioXASSSRLMonochromatorEnergyControl *newControl);

	/// Sets the region control. Reimplemented to include updating the control with other mono controls.
	void setRegion(BioXASSSRLMonochromatorRegionControl *newControl);

	/// Handles updating the mask state with the latest upper and lower mask blade controls.
	void updateMaskState();
	/// Handles updating the step-based bragg control with the latest settling time.
	void updateStepBragg();
	/// Handles updating the encoder-based bragg control with the latest settling time.
	void updateEncoderBragg();
	/// Handles updating the bragg control with the desired control (either step-based or encoder-based).
	void updateBragg();
	/// Handles updating the step-based energy control with the m1 mirror pitch.
	void updateStepEnergy();
	/// Handles updating the encoder-based energy control with the m1 mirror pitch.
	void updateEncoderEnergy();
	/// Handles updating the energy control with the desired control (either step-based or encoder-based).
	void updateEnergy();
	/// Handles updating the region control.
	void updateRegion();

protected:
	/// The upper blade control.
	BioXASMAXvMotor *maskUpperBlade_;
	/// The lower blade control.
	BioXASMAXvMotor *maskLowerBlade_;
	/// The state PV control.
	AMControl *bladesState_;
	/// The state control.
	BioXASSSRLMonochromatorMaskState *maskState_;
	/// The paddle motor control.
	BioXASMAXvMotor *paddle_;
	/// The paddle status control.
	AMControl *paddleStatus_;
	/// The key status control.
	AMControl *keyStatus_;
	/// The brake status control.
	AMControl *brakeStatus_;
	/// The bragg motor at crystal change position status control.
	AMControl *braggAtCrystalChangePositionStatus_;
	/// The crystal change motor control.
	BioXASMAXvMotor *crystalChange_;
	/// The region A status control.
	AMControl *regionAStatus_;
	/// The region B status control.
	AMControl *regionBStatus_;
	/// Vertical motor.
	CLSMAXvMotor *vertical_;
	/// Lateral motor.
	CLSMAXvMotor *lateral_;
	/// Crystal 1 pitch motor.
	CLSMAXvMotor *crystal1Pitch_;
	/// Crystal 1 roll motor.
	CLSMAXvMotor *crystal1Roll_;
	/// Crystal 2 pitch motor.
	CLSMAXvMotor *crystal2Pitch_;
	/// Crystal 2 roll motor.
	CLSMAXvMotor *crystal2Roll_;

	/// The m1 mirror pitch control.
	AMControl *m1Pitch_;

	/// The step-based bragg control.
	BioXASMAXvMotor *stepBragg_;
	/// The encoder-based bragg control.
	BioXASMAXvMotor *encoderBragg_;
	/// The bragg motor corresponding to the current mode.
	BioXASMAXvMotor *bragg_;

	/// The step-based energy control.
	BioXASSSRLMonochromatorEnergyControl *stepEnergy_;
	/// The encoder-based energy control.
	BioXASSSRLMonochromatorEnergyControl *encoderEnergy_;

	/// The region control.
	BioXASSSRLMonochromatorRegionControl *region_;

	/// The mono move settling time, in seconds.
	double settlingTime_;
	/// The mode.
	double mode_;
};

#endif // BIOXASSSRLMONOCHROMATOR_H
