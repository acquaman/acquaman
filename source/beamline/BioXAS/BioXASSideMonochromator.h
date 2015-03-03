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
	virtual Region::State region() const { return region_; }
    /// Returns true if both upper and lower slits are closed, false otherwise.
	virtual bool slitsClosed() const { return ((int)slitsStatus_->value() == Slits::Closed); }
    /// Returns true if the paddle is out, false otherwise.
	virtual bool paddleOut() const { return ((int)paddleStatus_->value() == Paddle::Out); }
	/// Returns true if the key is enabled, false otherwise.
	virtual bool keyEnabled() const { return ((int)keyStatus_->value() == Key::Enabled); }
	/// Returns true if the bragg motor is at the crystal change position, false otherwise.
	virtual bool braggAtCrystalChangePosition() const { return ((int)braggAtCrystalChangePositionStatus_->value() == Bragg::InPosition); }
    /// Returns true if the brake is enabled, false otherwise.
	virtual bool brakeEnabled() const { return ((int)brakeStatus_->value() == Brake::Enabled); }
	/// Returns true if the crystal change motor is at its clockwise limit, false otherwise.
	virtual bool crystalChangeAtCWLimit() const { return ((int)crystalChangeMotorCWLimitStatus_->value() == CrystalChangeMotor::AtLimit); }
	/// Returns true if the crystal change motor is at its counter-clockwise limit, false otherwise.
	virtual bool crystalChangeAtCCWLimit() const { return ((int)crystalChangeMotorCCWLimitStatus_->value() == CrystalChangeMotor::AtLimit); }

    /// Returns the phosphor paddle motor.
    CLSMAXvMotor* phosphorPaddleMotor() const { return phosphorPaddleMotor_; }
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

	/// Returns the slits status control.
	AMControl* slitsStatusControl() const { return slitsStatus_; }
	/// Returns the paddle status control.
	AMControl* paddleStatusControl() const { return paddleStatus_; }
    /// Returns the control for whether the crystal change operation is enabled.
	AMControl* keyStatusControl() const { return keyStatus_; }
	/// Returns the control for whether the bragg motor is at the crystal change position.
	AMControl* braggAtCrystalChangePositionStatusControl() const { return braggAtCrystalChangePositionStatus_; }
	/// Returns the bragg motor absolute position control.
	AMControl* braggAbsolutePositionControl() const { return braggAbsolutePosition_; }
    /// Returns the relative crystal change motor control.
	AMControl* crystalChangeRelativePositionControl() const { return crystalChangeMotorRel_; }
	/// Returns the crystal change motor clockwise limit status control.
	AMControl* crystalChangeCWLimitStatusControl() const { return crystalChangeMotorCWLimitStatus_; }
	/// Returns the crystal change motor counterclockwise limit status control.
	AMControl* crystalStageCCWLimitStatusControl() const { return crystalChangeMotorCCWLimitStatus_; }
	/// Returns the brake status control.
	AMControl* brakeStatusControl() const { return brakeStatus_; }
    /// Returns the region A status control.
    AMControl* regionAStatusControl() const { return regionAStatus_; }
    /// Returns the region B status control.
    AMControl* regionBStatusControl() const { return regionBStatus_; }
    /// Returns the energy setpoint control.
    AMControl* energyControl() const { return energy_; }

	/// Returns a new set energy action, 0 if not connected.
	virtual AMAction3* createSetEnergyAction(double newEnergy);
    /// Returns a new close slits action, 0 if not connected.
	virtual AMAction3* createCloseSlitsAction();
    /// Returns a new remove paddle action, 0 if not connected.
	virtual AMAction3* createRemovePaddleAction();
	/// Returns a new action that sets the crystal stage to the given absolute destination angle.
	virtual AMAction3* createMoveBraggMotorAction(double degDestination);
	/// Returns a new action that moves the crystal change motor by the given degrees (relative).
	virtual AMAction3* createMoveCrystalChangeMotorAction(int relDestination);

    /// Returns a new action that turns on the bragg motor, 0 if not connected.
    AMAction3* createSetBraggMotorPowerOnAction();
    /// Returns a new action that sets the bragg motor power to auto-software.
    AMAction3* createSetBraggMotorPowerAutoAction();

public slots:
    /// Sets the energy setpoint.
	virtual void setEnergy(double newEnergy);
	/// Sets the region.
	virtual void setRegion(Region::State newRegion);
	/// Closes both slits.
	virtual void setSlitsClosed();
	/// Removes the paddle.
	virtual void setPaddleOut();
	/// Sets the position of the bragg motor.
	virtual void setBraggMotorPosition(double degDestination);
	/// Sets the position of the crystal change motor.
	virtual void setCrystalChangeMotorPosition(double relDestination);

protected slots:
    /// Updates the mono's general connected state based on the connected state of each of its pvs.
    void onConnectedChanged();
    /// Updates the mono's current region state based on the values of regionAStatus_ and regionBStatus_ controls.
    void onRegionChanged();
    /// Emits the appropriate signal when the mono's slits closed status has changed.
	void onSlitsClosedChanged(double value) { emit slitsStatusChanged((int)value == Slits::Closed); }
    /// Emits the appropriate signal when the paddle is out.
	void onPaddleOutChanged(double value) { emit paddleStatusChanged((int)value == Paddle::Out); }
    /// Emits the appropriate signal when the crystal change motor is enabled/disabled.
	void onCrystalChangeEnabled(double value) { emit keyStatusChanged((int)value == Key::Enabled); }
    /// Emits the appropriate signal when the crystal change position status has changed.
	void onCrystalChangePositionStatusChanged(double value) { emit braggCrystalChangePositionStatusChanged((int)value == Bragg::InPosition); }
    /// Emits the appropriate signal when the crystal change brake is enabled/disabled.
	void onCrystalChangeBrakeEnabledChanged(double value) { emit brakeStatusChanged((int) value == Brake::Enabled); }
    /// Emits the appropriate signal when the crystal change motor reaches/leaves the CW limit.
	void onCrystalChangeMotorCWLimitStatusChanged(double value) { emit crystalChangeCWLimitStatusChanged((int)value == CrystalChangeMotor::AtLimit); }
    /// Emits the appropriate signal when the crystal change motor reaches/leaves the CCW limit.
	void onCrystalChangeMotorCCWLimitStatusChanged(double value) { emit crystalChangeCCWLimitStatusChanged((int)value == CrystalChangeMotor::AtLimit); }

protected:
	/// Returns a new action that waits for the region key to be turned CCW to Disabled, 0 if not connected.
	AMAction3* createWaitForKeyEnabledAction();
	/// Returns a new action that sends the crystal motor to the change position.
	AMAction3* createMoveToCrystalChangePositionAction();
	/// Returns a new action that waits for the brake to be disabled, 0 if not connected.
	AMAction3* createWaitForBrakeDisabledAction();
	/// Returns a new action that waits for the brake to be turned on, 0 if not connected.
	AMAction3* createWaitForBrakeEnabledAction();
	/// Returns a new action that waits for the region key to be turned CW to Disabled, 0 if not connected.
	AMAction3* createWaitForKeyDisabledAction();
	/// Returns a new crystal change action that moves to desired region, 0 if not connected.
	AMAction3* createCrystalChangeAction(Region::State newRegion);

protected:
	/// Connected state.
    bool connected_;
	/// Current region.
	Region::State region_;

    // Motors

    CLSMAXvMotor *phosphorPaddleMotor_;
    CLSMAXvMotor *braggMotor_;
    CLSMAXvMotor *verticalMotor_;
    CLSMAXvMotor *lateralMotor_;
	CLSMAXvMotor *crystalChangeMotor_;
    CLSMAXvMotor *crystal1PitchMotor_;
    CLSMAXvMotor *crystal1RollMotor_;
    CLSMAXvMotor *crystal2PitchMotor_;
    CLSMAXvMotor *crystal2RollMotor_;

    // Controls

    AMControl* braggMotorPower_;
	AMControl* slitsStatus_;
	AMControl* paddleStatus_;
	AMControl* keyStatus_;
	AMControl* braggAtCrystalChangePositionStatus_;
	AMControl* brakeStatus_;
	AMControl* braggAbsolutePosition_;
    AMControl* crystalChangeMotorRel_;
	AMControl* crystalChangeMotorCWLimitStatus_;
	AMControl* crystalChangeMotorCCWLimitStatus_;
    AMControl* regionAStatus_;
    AMControl* regionBStatus_;
    AMControl* energy_;
};

#endif // BIOXASSIDEMONOCHROMATOR_H
